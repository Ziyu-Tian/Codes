import json
import sqlite3
from collections import Counter, defaultdict
from datetime import datetime, timezone
from pathlib import Path

from .config import BALANCES_PATH, CATEGORY_RULES_PATH, DATABASE_PATH
from .csv_importer import import_csv_file


def now_iso():
    return datetime.now(timezone.utc).isoformat(timespec="seconds").replace("+00:00", "Z")


def connect():
    DATABASE_PATH.parent.mkdir(parents=True, exist_ok=True)
    connection = sqlite3.connect(DATABASE_PATH)
    connection.row_factory = sqlite3.Row
    connection.execute("PRAGMA foreign_keys = ON")
    return connection


def init_db(connection=None):
    own_connection = connection is None
    connection = connection or connect()
    try:
        connection.executescript("""
            CREATE TABLE IF NOT EXISTS import_files (
                dataset TEXT NOT NULL,
                source TEXT NOT NULL,
                path TEXT NOT NULL,
                size INTEGER NOT NULL,
                mtime_ns INTEGER NOT NULL,
                imported_at TEXT NOT NULL,
                PRIMARY KEY (dataset, source)
            );

            CREATE TABLE IF NOT EXISTS raw_transactions (
                dataset TEXT NOT NULL,
                id TEXT NOT NULL,
                row_index INTEGER NOT NULL,
                date TEXT NOT NULL,
                account TEXT NOT NULL,
                raw_amount REAL NOT NULL,
                category TEXT NOT NULL,
                memo TEXT NOT NULL,
                source TEXT NOT NULL,
                source_stem TEXT NOT NULL,
                PRIMARY KEY (dataset, id)
            );

            CREATE INDEX IF NOT EXISTS idx_raw_transactions_dataset_date
                ON raw_transactions (dataset, date DESC);

            CREATE TABLE IF NOT EXISTS balance_snapshots (
                dataset TEXT NOT NULL,
                account_key TEXT NOT NULL,
                balance REAL NOT NULL,
                as_of TEXT NOT NULL,
                PRIMARY KEY (dataset, account_key)
            );

            CREATE TABLE IF NOT EXISTS category_transaction_overrides (
                dataset TEXT NOT NULL,
                fingerprint TEXT NOT NULL,
                category TEXT NOT NULL,
                memo TEXT NOT NULL,
                amount REAL NOT NULL,
                account_key TEXT NOT NULL,
                updated_at TEXT NOT NULL,
                PRIMARY KEY (dataset, fingerprint)
            );

            CREATE TABLE IF NOT EXISTS category_amount_rules (
                dataset TEXT NOT NULL,
                rule_key TEXT NOT NULL,
                category TEXT NOT NULL,
                signature TEXT NOT NULL,
                amount REAL NOT NULL,
                direction TEXT NOT NULL,
                last_memo TEXT NOT NULL,
                usage_count INTEGER NOT NULL DEFAULT 0,
                updated_at TEXT NOT NULL,
                PRIMARY KEY (dataset, rule_key)
            );

            CREATE TABLE IF NOT EXISTS category_custom_categories (
                dataset TEXT NOT NULL,
                category TEXT NOT NULL,
                PRIMARY KEY (dataset, category)
            );
        """)
        if own_connection:
            connection.commit()
    finally:
        if own_connection:
            connection.close()


def load_json_file(path):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        return {}


def migrate_legacy_json_state():
    """Import existing JSON state into SQLite once, without deleting the JSON backups."""
    with connect() as connection:
        init_db(connection)
        marker_count = connection.execute(
            "SELECT COUNT(*) FROM balance_snapshots"
        ).fetchone()[0] + connection.execute(
            "SELECT COUNT(*) FROM category_transaction_overrides"
        ).fetchone()[0] + connection.execute(
            "SELECT COUNT(*) FROM category_amount_rules"
        ).fetchone()[0] + connection.execute(
            "SELECT COUNT(*) FROM category_custom_categories"
        ).fetchone()[0]
        if marker_count:
            return

        balances = load_json_file(BALANCES_PATH)
        for dataset, entries in balances.items():
            for account_key, entry in entries.items():
                connection.execute(
                    """
                    INSERT OR REPLACE INTO balance_snapshots
                    (dataset, account_key, balance, as_of)
                    VALUES (?, ?, ?, ?)
                    """,
                    (dataset, account_key, float(entry["balance"]), entry["as_of"]),
                )

        categories = load_json_file(CATEGORY_RULES_PATH)
        for dataset, store in categories.items():
            for fingerprint, entry in store.get("transaction_overrides", {}).items():
                connection.execute(
                    """
                    INSERT OR REPLACE INTO category_transaction_overrides
                    (dataset, fingerprint, category, memo, amount, account_key, updated_at)
                    VALUES (?, ?, ?, ?, ?, ?, ?)
                    """,
                    (
                        dataset,
                        fingerprint,
                        entry["category"],
                        entry.get("memo", ""),
                        float(entry.get("amount", 0)),
                        entry.get("account_key", ""),
                        entry.get("updated_at", now_iso()),
                    ),
                )
            for rule_key, entry in store.get("amount_rules", {}).items():
                connection.execute(
                    """
                    INSERT OR REPLACE INTO category_amount_rules
                    (dataset, rule_key, category, signature, amount, direction, last_memo, usage_count, updated_at)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
                    """,
                    (
                        dataset,
                        rule_key,
                        entry["category"],
                        entry.get("signature", ""),
                        float(entry.get("amount", 0)),
                        entry.get("direction", ""),
                        entry.get("last_memo", ""),
                        int(entry.get("usage_count", 0)),
                        entry.get("updated_at", now_iso()),
                    ),
                )
            for category in store.get("custom_categories", []):
                connection.execute(
                    """
                    INSERT OR REPLACE INTO category_custom_categories
                    (dataset, category)
                    VALUES (?, ?)
                    """,
                    (dataset, category),
                )


def ensure_db():
    init_db()
    migrate_legacy_json_state()


def file_signature(csv_file):
    stat = Path(csv_file).stat()
    return stat.st_size, stat.st_mtime_ns


def sync_csv_dataset(dataset, csv_dir):
    """Incrementally import new/changed CSV files and remove DB rows for deleted files."""
    csv_dir = Path(csv_dir)
    csv_files = sorted(csv_dir.glob("*.csv"))
    if not csv_files:
        raise FileNotFoundError(f"No CSV files found in {csv_dir}")

    ensure_db()
    current_sources = {csv_file.name for csv_file in csv_files}

    with connect() as connection:
        stale_sources = [
            row["source"]
            for row in connection.execute(
                "SELECT source FROM import_files WHERE dataset = ?",
                (dataset,),
            )
            if row["source"] not in current_sources
        ]
        for source in stale_sources:
            connection.execute(
                "DELETE FROM raw_transactions WHERE dataset = ? AND source = ?",
                (dataset, source),
            )
            connection.execute(
                "DELETE FROM import_files WHERE dataset = ? AND source = ?",
                (dataset, source),
            )

        for csv_file in csv_files:
            size, mtime_ns = file_signature(csv_file)
            existing = connection.execute(
                """
                SELECT size, mtime_ns FROM import_files
                WHERE dataset = ? AND source = ?
                """,
                (dataset, csv_file.name),
            ).fetchone()
            if existing and existing["size"] == size and existing["mtime_ns"] == mtime_ns:
                continue

            rows = import_csv_file(csv_file)
            connection.execute(
                "DELETE FROM raw_transactions WHERE dataset = ? AND source = ?",
                (dataset, csv_file.name),
            )
            connection.executemany(
                """
                INSERT OR REPLACE INTO raw_transactions
                (dataset, id, row_index, date, account, raw_amount, category, memo, source, source_stem)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                """,
                [
                    (
                        dataset,
                        row["id"],
                        int(row.get("row_index", 0)),
                        row["date"],
                        row["account"],
                        float(row["raw_amount"]),
                        row["category"],
                        row["memo"],
                        row["source"],
                        row["source_stem"],
                    )
                    for row in rows
                ],
            )
            connection.execute(
                """
                INSERT OR REPLACE INTO import_files
                (dataset, source, path, size, mtime_ns, imported_at)
                VALUES (?, ?, ?, ?, ?, ?)
                """,
                (dataset, csv_file.name, str(csv_file), size, mtime_ns, now_iso()),
            )


def load_raw_transactions(dataset):
    ensure_db()
    with connect() as connection:
        rows = connection.execute(
            """
            SELECT id, row_index, date, account, raw_amount, category, memo, source, source_stem
            FROM raw_transactions
            WHERE dataset = ?
            ORDER BY date DESC, source ASC, row_index ASC
            """,
            (dataset,),
        ).fetchall()
    deduped_rows, _removed_count = deduplicate_raw_transactions([dict(row) for row in rows])
    return deduped_rows


def normalise_dedupe_text(value):
    return " ".join(str(value or "").strip().lower().split())


def transaction_dedupe_key(row):
    """Stable cross-file duplicate key for raw bank export rows."""
    return "|".join([
        row["date"],
        normalise_dedupe_text(row["account"]),
        f"{float(row['raw_amount']):.2f}",
        normalise_dedupe_text(row["category"]),
        normalise_dedupe_text(row["memo"]),
    ])


def deduplicate_raw_transactions(rows):
    """Collapse repeated exports while preserving repeated identical rows within one file.

    If the same canonical transaction appears in overlapping CSV exports, keep only the
    maximum number seen in any single source file. This avoids double-counting overlap
    while still preserving genuinely repeated identical rows that appear multiple times
    inside one statement.
    """
    grouped = defaultdict(list)
    for row in rows:
        grouped[transaction_dedupe_key(row)].append(row)

    deduped = []
    removed_count = 0
    for group_rows in grouped.values():
        by_source = Counter(row["source"] for row in group_rows)
        keep_count = max(by_source.values())
        best_source = sorted(
            by_source,
            key=lambda source: (-by_source[source], source),
        )[0]
        selected = [
            row for row in sorted(group_rows, key=lambda item: (item["row_index"], item["id"]))
            if row["source"] == best_source
        ][:keep_count]
        duplicate_removed = len(group_rows) - keep_count
        for row in selected:
            row["duplicate_group_count"] = len(group_rows)
            row["duplicate_removed_count"] = duplicate_removed
        removed_count += duplicate_removed
        deduped.extend(selected)

    deduped.sort(key=lambda row: (row["date"], row["source"], row["row_index"], row["id"]), reverse=True)
    return deduped, removed_count


def duplicate_transaction_count(dataset):
    ensure_db()
    with connect() as connection:
        rows = connection.execute(
            """
            SELECT id, row_index, date, account, raw_amount, category, memo, source, source_stem
            FROM raw_transactions
            WHERE dataset = ?
            """,
            (dataset,),
        ).fetchall()
    _deduped_rows, removed_count = deduplicate_raw_transactions([dict(row) for row in rows])
    return removed_count


def raw_file_count(dataset):
    ensure_db()
    with connect() as connection:
        return connection.execute(
            "SELECT COUNT(*) FROM import_files WHERE dataset = ?",
            (dataset,),
        ).fetchone()[0]


def load_balance_store_from_db():
    ensure_db()
    store = {}
    with connect() as connection:
        for row in connection.execute(
            "SELECT dataset, account_key, balance, as_of FROM balance_snapshots"
        ):
            dataset_store = store.setdefault(row["dataset"], {})
            dataset_store[row["account_key"]] = {
                "balance": row["balance"],
                "as_of": row["as_of"],
            }
    return store


def save_balance_store_to_db(store):
    ensure_db()
    with connect() as connection:
        connection.execute("DELETE FROM balance_snapshots")
        for dataset, entries in store.items():
            for account_key, entry in entries.items():
                connection.execute(
                    """
                    INSERT OR REPLACE INTO balance_snapshots
                    (dataset, account_key, balance, as_of)
                    VALUES (?, ?, ?, ?)
                    """,
                    (dataset, account_key, float(entry["balance"]), entry["as_of"]),
                )


def load_category_store_from_db():
    ensure_db()
    store = {}
    with connect() as connection:
        for row in connection.execute(
            """
            SELECT dataset, fingerprint, category, memo, amount, account_key, updated_at
            FROM category_transaction_overrides
            """
        ):
            dataset_store = store.setdefault(row["dataset"], {})
            overrides = dataset_store.setdefault("transaction_overrides", {})
            overrides[row["fingerprint"]] = {
                "category": row["category"],
                "memo": row["memo"],
                "amount": row["amount"],
                "account_key": row["account_key"],
                "updated_at": row["updated_at"],
            }
        for row in connection.execute(
            """
            SELECT dataset, rule_key, category, signature, amount, direction, last_memo, usage_count, updated_at
            FROM category_amount_rules
            """
        ):
            dataset_store = store.setdefault(row["dataset"], {})
            amount_rules = dataset_store.setdefault("amount_rules", {})
            amount_rules[row["rule_key"]] = {
                "category": row["category"],
                "signature": row["signature"],
                "amount": row["amount"],
                "direction": row["direction"],
                "last_memo": row["last_memo"],
                "usage_count": row["usage_count"],
                "updated_at": row["updated_at"],
            }
        for row in connection.execute(
            "SELECT dataset, category FROM category_custom_categories"
        ):
            dataset_store = store.setdefault(row["dataset"], {})
            custom_categories = dataset_store.setdefault("custom_categories", [])
            custom_categories.append(row["category"])

    for dataset_store in store.values():
        dataset_store.setdefault("transaction_overrides", {})
        dataset_store.setdefault("amount_rules", {})
        dataset_store["custom_categories"] = sorted(
            set(dataset_store.get("custom_categories", [])),
            key=str.lower,
        )
    return store


def save_category_store_to_db(store):
    ensure_db()
    with connect() as connection:
        connection.execute("DELETE FROM category_transaction_overrides")
        connection.execute("DELETE FROM category_amount_rules")
        connection.execute("DELETE FROM category_custom_categories")
        for dataset, dataset_store in store.items():
            for fingerprint, entry in dataset_store.get("transaction_overrides", {}).items():
                connection.execute(
                    """
                    INSERT OR REPLACE INTO category_transaction_overrides
                    (dataset, fingerprint, category, memo, amount, account_key, updated_at)
                    VALUES (?, ?, ?, ?, ?, ?, ?)
                    """,
                    (
                        dataset,
                        fingerprint,
                        entry["category"],
                        entry.get("memo", ""),
                        float(entry.get("amount", 0)),
                        entry.get("account_key", ""),
                        entry.get("updated_at", now_iso()),
                    ),
                )
            for rule_key, entry in dataset_store.get("amount_rules", {}).items():
                connection.execute(
                    """
                    INSERT OR REPLACE INTO category_amount_rules
                    (dataset, rule_key, category, signature, amount, direction, last_memo, usage_count, updated_at)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
                    """,
                    (
                        dataset,
                        rule_key,
                        entry["category"],
                        entry.get("signature", ""),
                        float(entry.get("amount", 0)),
                        entry.get("direction", ""),
                        entry.get("last_memo", ""),
                        int(entry.get("usage_count", 0)),
                        entry.get("updated_at", now_iso()),
                    ),
                )
            for category in dataset_store.get("custom_categories", []):
                connection.execute(
                    """
                    INSERT OR REPLACE INTO category_custom_categories
                    (dataset, category)
                    VALUES (?, ?)
                    """,
                    (dataset, category),
                )

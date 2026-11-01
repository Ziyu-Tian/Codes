"""Private manual savings accounts, dated balances and confirmed transfer links.

Identifiers stay in SQLite. Public DTOs contain only a masked identifier.
Manual balances are observations, never incremented by imported transfers.
"""
from datetime import date
from decimal import Decimal, InvalidOperation, ROUND_HALF_UP
import json
import re
import uuid

from .database import connect, ensure_db
from .categorizer import transaction_fingerprint
from .transaction_processor import clean_text, memo_account, looks_like_transfer


def initialise():
    ensure_db()
    with connect() as db:
        db.executescript("""
            CREATE TABLE IF NOT EXISTS manual_accounts (
                dataset TEXT NOT NULL, key TEXT NOT NULL, label TEXT NOT NULL,
                type TEXT NOT NULL, identifier TEXT NOT NULL DEFAULT '',
                archived INTEGER NOT NULL DEFAULT 0,
                PRIMARY KEY (dataset, key)
            );
            CREATE TABLE IF NOT EXISTS manual_balance_history (
                dataset TEXT NOT NULL, account_key TEXT NOT NULL, as_of TEXT NOT NULL,
                balance_minor INTEGER NOT NULL, note TEXT NOT NULL DEFAULT '',
                PRIMARY KEY (dataset, account_key, as_of)
            );
            CREATE TABLE IF NOT EXISTS manual_transfer_links (
                dataset TEXT NOT NULL, fingerprint TEXT NOT NULL, account_key TEXT,
                PRIMARY KEY (dataset, fingerprint)
            );
            CREATE TABLE IF NOT EXISTS manual_transfer_rules (
                dataset TEXT NOT NULL, signature TEXT NOT NULL, account_key TEXT NOT NULL,
                PRIMARY KEY (dataset, signature)
            );
        """)


def load_accounts(dataset, private=False):
    initialise()
    with connect() as db:
        rows = [dict(row) for row in db.execute(
            "SELECT * FROM manual_accounts WHERE dataset=? ORDER BY label, key", (dataset,)
        )]
        for row in rows:
            row.pop("dataset")
            identifier = row.pop("identifier")
            row["source"] = "manual"
            row["archived"] = bool(row["archived"])
            row["identifier_hint"] = "•••• " + identifier[-4:] if identifier else ""
            if private:
                row["identifier"] = identifier
    return rows


def balance_store(dataset):
    initialise()
    with connect() as db:
        rows = db.execute(
            "SELECT * FROM manual_balance_history WHERE dataset=? ORDER BY as_of", (dataset,)
        ).fetchall()
    store = {}
    for row in rows:
        entry = store.setdefault(row["account_key"], {"source": "manual", "history": []})
        observation = {"as_of": row["as_of"], "balance": row["balance_minor"] / 100,
                       "note": row["note"]}
        entry["history"].append(observation)
        entry.update(balance=observation["balance"], as_of=observation["as_of"])
    return store


def balance_at(snapshot, target):
    """None means unrecorded, not a zero balance; never backfill an observation."""
    history = [row for row in snapshot.get("history", []) if row["as_of"] <= str(target)]
    return max(history, key=lambda row: row["as_of"])["balance"] if history else None


def save_account(dataset, data, csv_account_keys=()):
    if not isinstance(data, dict):
        raise ValueError("Account must be an object.")
    key = data.get("key")
    existing = next((a for a in load_accounts(dataset, private=True) if a["key"] == key), None)
    if key and not existing:
        raise ValueError("Manual account not found.")
    label = str(data.get("label", "")).strip()
    kind = data.get("type")
    if not 1 <= len(label) <= 60 or kind not in {"cash_isa", "saver"}:
        raise ValueError("Enter an account name (1–60 characters) and a supported type.")
    # Empty edit retains the existing private identifier; clearing is explicit.
    supplied = data.get("identifier", "")
    if not isinstance(supplied, str) or re.search(r"[^\d\s-]", supplied):
        raise ValueError("Use a six-digit sort code and eight-digit account number.")
    identifier = re.sub(r"\D", "", supplied)
    if not identifier and existing and not data.get("clear_identifier", False):
        identifier = existing["identifier"]
    if identifier and len(identifier) != 14:
        raise ValueError("Use a six-digit sort code and eight-digit account number.")
    if identifier and identifier in csv_account_keys:
        raise ValueError("This account is already imported from CSV.")
    if identifier and any(a["key"] != key and a["identifier"] == identifier
                          for a in load_accounts(dataset, private=True)):
        raise ValueError("This identifier is already assigned to another manual account.")
    key = key or "manual:" + uuid.uuid4().hex
    archived = bool(data.get("archived", existing["archived"] if existing else False))
    observation = None
    if data.get("balance") not in (None, ""):
        try:
            amount = Decimal(str(data["balance"]))
            if not amount.is_finite() or amount < 0 or amount > Decimal("1000000000000"):
                raise ValueError("Enter a finite, non-negative balance.")
            minor = int((amount * 100).quantize(Decimal("1"), rounding=ROUND_HALF_UP))
        except (InvalidOperation, TypeError):
            raise ValueError("Enter a valid balance.") from None
        as_of = date.fromisoformat(str(data.get("as_of", ""))).isoformat()
        if as_of > date.today().isoformat():
            raise ValueError("Balance dates cannot be in the future.")
        note = str(data.get("note", "")).strip()
        if len(note) > 200:
            raise ValueError("Notes must be at most 200 characters.")
        observation = (dataset, key, as_of, minor, note)
    elif not existing:
        raise ValueError("An opening balance and date are required.")
    with connect() as db:
        db.execute("INSERT OR REPLACE INTO manual_accounts VALUES (?,?,?,?,?,?)",
                   (dataset, key, label, kind, identifier, int(archived)))
        if observation:
            db.execute("INSERT OR REPLACE INTO manual_balance_history VALUES (?,?,?,?,?)", observation)
    return key


def rule_signature(item):
    return json.dumps([item["account_key"], clean_text(item.get("category", "")),
                       clean_text(item.get("memo", "")), "out" if item["amount"] < 0 else "in"])


def apply_transfers(items, dataset):
    accounts = load_accounts(dataset, private=True)
    account_map = {a["key"]: a for a in accounts}
    with connect() as db:
        links = {r["fingerprint"]: r["account_key"] for r in db.execute(
            "SELECT * FROM manual_transfer_links WHERE dataset=?", (dataset,))}
        rules = {r["signature"]: r["account_key"] for r in db.execute(
            "SELECT * FROM manual_transfer_rules WHERE dataset=?", (dataset,))}
    for item in items:
        if item["account_type"] == "credit_card":
            continue
        fingerprint = transaction_fingerprint(item)
        target = None
        if fingerprint in links:
            target = links[fingerprint]
        elif rule_signature(item) in rules:
            target = rules[rule_signature(item)]
        elif looks_like_transfer(item):
            counterparty = memo_account(item.get("memo", ""))
            target = next((a["key"] for a in accounts if a["identifier"] and
                           a["identifier"] == counterparty), None)
        if target in account_map:
            item.update(transfer_type="internal", internal_reason="manual_account_transfer",
                        transfer_pair_id="manual-" + fingerprint,
                        counterparty_account=account_map[target]["label"],
                        linked_account_key=target, balance_affects_linked_account=False,
                        excluded_from_totals=True)
    return items


def save_transfer_link(dataset, item, target, remember=False):
    keys = {a["key"] for a in load_accounts(dataset)}
    if target is not None and target not in keys:
        raise ValueError("Select a valid manual account.")
    if item["account_type"] == "credit_card" or (
        item.get("excluded_from_totals") and item.get("internal_reason") != "manual_account_transfer"
    ):
        raise ValueError("This transaction is already assigned to another internal movement.")
    signature = rule_signature({**item, "category": item.get("original_category", item["category"])})
    with connect() as db:
        db.execute("INSERT OR REPLACE INTO manual_transfer_links VALUES (?,?,?)",
                   (dataset, item["transaction_fingerprint"], target))
        if target is None:
            db.execute("DELETE FROM manual_transfer_rules WHERE dataset=? AND signature=?",
                       (dataset, signature))
        elif remember:
            if not clean_text(item.get("memo")):
                raise ValueError("An empty description cannot be remembered as a rule.")
            db.execute("INSERT OR REPLACE INTO manual_transfer_rules VALUES (?,?,?)",
                       (dataset, signature, target))

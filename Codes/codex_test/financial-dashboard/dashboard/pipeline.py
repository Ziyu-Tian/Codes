import math
from datetime import datetime

from .categorizer import (
    apply_category_rules,
    category_options_for,
    category_rule_count,
    learn_category_rule,
)
from .database import duplicate_transaction_count, load_raw_transactions, raw_file_count, sync_csv_dataset
from .storage import load_balance_store, save_balance_store
from .transaction_processor import process_transactions


def read_transactions(csv_dir, dataset):
    """Main transformation pipeline: SQLite raw rows -> processed, categorised transactions."""
    sync_csv_dataset(dataset, csv_dir)
    raw_items = load_raw_transactions(dataset)
    processed_items = process_transactions(raw_items)
    return apply_category_rules(processed_items, dataset)


def build_transactions_payload(dataset, csv_dir):
    transactions = read_transactions(csv_dir, dataset)
    account_map = {}
    for item in transactions:
        account_map[item["account_key"]] = {
            "label": item["account_label"],
            "type": item["account_type"],
        }
    internal_pairs = len({
        item["transfer_pair_id"] for item in transactions if item["transfer_pair_id"]
    })
    return {
        "transactions": transactions,
        "accounts": [
            {"key": key, **details}
            for key, details in sorted(account_map.items(), key=lambda value: value[1]["label"])
        ],
        "balances": load_balance_store().get(dataset, {}),
        "category_options": category_options_for(dataset),
        "category_rule_count": category_rule_count(dataset),
        "file_count": raw_file_count(dataset),
        "duplicate_transactions_removed": duplicate_transaction_count(dataset),
        "internal_transfer_pairs": internal_pairs,
        "source": str(csv_dir),
    }


def save_balances(dataset, csv_dir, balance_entries):
    transactions = read_transactions(csv_dir, dataset)
    recognised = {item["account_key"]: item["account_type"] for item in transactions}
    saved = {}
    for entry in balance_entries:
        key = str(entry.get("account_key", ""))
        if key not in recognised:
            raise ValueError(f"Unknown account: {key}")
        balance = float(entry.get("balance"))
        if not math.isfinite(balance):
            raise ValueError("Balance must be a finite number")
        as_of = datetime.strptime(entry.get("as_of", ""), "%Y-%m-%d").strftime("%Y-%m-%d")
        if recognised[key] == "credit_card" and balance < 0:
            raise ValueError("Credit-card debt must be zero or positive.")
        saved[key] = {"balance": round(balance, 2), "as_of": as_of}

    store = load_balance_store()
    store[dataset] = saved
    save_balance_store(store)
    return {"balances": saved}


def save_category_rule_for_transaction(dataset, csv_dir, fingerprint, category, learn_amount_rule=True):
    if not category or len(category) > 60:
        raise ValueError("Category must be 1-60 characters.")
    transactions = read_transactions(csv_dir, dataset)
    transaction = next((item for item in transactions if item["transaction_fingerprint"] == fingerprint), None)
    if not transaction:
        raise LookupError("Transaction not found.")
    return learn_category_rule(dataset, transaction, category, learn_amount_rule)

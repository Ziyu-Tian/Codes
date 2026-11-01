from copy import deepcopy
from pathlib import Path

from dashboard.analytics_metrics import AnalyticsDataset
from dashboard.categorizer import builtin_category
from dashboard.csv_importer import import_csv_transactions
from dashboard.transaction_processor import process_transactions


DEMO_CSV_DIR = Path(__file__).resolve().parents[2] / "csv_test_input"
SENSITIVE_CANARY = "PRIVATE-CANARY-DO-NOT-SEND"


def demo_dataset(selected_account=None, add_sensitive_canary=False):
    transactions = process_transactions(import_csv_transactions(DEMO_CSV_DIR))
    for item in transactions:
        item["original_category"] = item["category"]
        item["category"] = builtin_category(item)
    transactions = deepcopy(transactions)
    if add_sensitive_canary:
        sensitive_item = next(
            item for item in transactions
            if item["amount"] < 0 and not item.get("excluded_from_totals", False)
        )
        sensitive_item.update({
            "memo": SENSITIVE_CANARY,
            "source": f"{SENSITIVE_CANARY}.csv",
            "account": f"{SENSITIVE_CANARY}-account",
            "account_label": SENSITIVE_CANARY,
            "account_id": SENSITIVE_CANARY,
            "category": SENSITIVE_CANARY,
        })

    account_map = {}
    for item in transactions:
        account_map[item["account_key"]] = {
            "key": item["account_key"],
            "type": item["account_type"],
        }
    accounts = list(account_map.values())
    balances = {
        account["key"]: {
            "balance": 1_000 if index == 0 else 2_000,
            "as_of": "2026-06-28",
        }
        for index, account in enumerate(accounts)
        if account["type"] != "credit_card"
    }
    return AnalyticsDataset(
        dataset="demo",
        transactions=transactions,
        accounts=accounts,
        balances=balances,
        selected_account=selected_account,
    )

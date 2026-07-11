import os
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
PUBLIC = PROJECT_ROOT / "public"
DATABASE_PATH = PROJECT_ROOT / "data" / "financial_dashboard.sqlite3"
BALANCES_PATH = PROJECT_ROOT / "data" / "account_balances.json"
CATEGORY_RULES_PATH = PROJECT_ROOT / "data" / "category_rules.json"

DATASETS = {
    "personal": Path(os.getenv("PFD_CSV_DIR", PROJECT_ROOT.parent / "csv_input")).expanduser(),
    "demo": Path(os.getenv("PFD_DEMO_CSV_DIR", PROJECT_ROOT.parent / "csv_test_input")).expanduser(),
}

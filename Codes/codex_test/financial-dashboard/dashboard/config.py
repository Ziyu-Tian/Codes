import os
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
PUBLIC = PROJECT_ROOT / "public"
DATABASE_PATH = Path(
    os.getenv("PFD_DATABASE_PATH", PROJECT_ROOT / "data" / "financial_dashboard.sqlite3")
).expanduser()
BALANCES_PATH = PROJECT_ROOT / "data" / "account_balances.json"
CATEGORY_RULES_PATH = PROJECT_ROOT / "data" / "category_rules.json"
LLM_SETTINGS_PATH = PROJECT_ROOT / "data" / "llm_settings.json"
DEFAULT_PET_SPRITESHEET_PATH = Path.home() / ".codex" / "pets" / "deepseek" / "spritesheet.webp"
PET_SPRITESHEET_PATH = Path(
    os.getenv("PFD_PET_SPRITESHEET") or DEFAULT_PET_SPRITESHEET_PATH
).expanduser()
PET_SPRITESHEET_MAX_BYTES = 16 * 1024 * 1024

DATASETS = {
    "personal": Path(os.getenv("PFD_CSV_DIR", PROJECT_ROOT.parent / "csv_input")).expanduser(),
    "demo": Path(os.getenv("PFD_DEMO_CSV_DIR", PROJECT_ROOT.parent / "csv_test_input")).expanduser(),
}

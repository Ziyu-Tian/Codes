# Agent guide for the financial dashboard

Required first step for future agents: read this file before changing backend code. Keep this file short and update it whenever module boundaries change.

## Architecture goal

The backend is intentionally split into small harness-friendly modules. A future agent should not need to reread the entire app to understand where a change belongs.

Data flow:

```text
CSV folder
  -> dashboard/csv_importer.py
  -> dashboard/database.py SQLite raw cache
  -> dashboard/transaction_processor.py
  -> dashboard/categorizer.py
  -> dashboard/pipeline.py
  -> server.py HTTP JSON response
  -> public/app.js dashboard rendering
```

## Backend modules

### server.py

Thin HTTP adapter only. It serves static frontend files and maps API routes to pipeline functions.

Do not put CSV parsing, account matching, transfer detection, or category learning logic here.

Current API routes:

- `GET /api/transactions?dataset=personal|demo`
- `POST /api/balances`
- `POST /api/category-rule`

After successful API calls, `server.py` triggers a best-effort mobile snapshot export. Export failures should be reported in JSON as `mobile_report_error`, not break the dashboard response.

### dashboard/config.py

Central paths and dataset mapping.

- `DATASETS["personal"]` points to `../csv_input` by default, or `PFD_CSV_DIR`
- `DATASETS["demo"]` points to `../csv_test_input` by default, or `PFD_DEMO_CSV_DIR`
- local SQLite state lives in `financial-dashboard/data/financial_dashboard.sqlite3`

### dashboard/storage.py

Compatibility wrapper for state persistence. It exposes the old load/save store functions, but delegates to SQLite.

Do not add JSON reads/writes here for new state.

### dashboard/database.py

SQLite persistence layer.

Responsibilities:

- create and migrate the local database;
- incrementally import only new or changed CSV files into `raw_transactions`;
- remove cached rows for CSV files deleted from the watched folder;
- deduplicate overlapping CSV exports before transactions are processed;
- preserve old JSON balances/category rules by importing them into SQLite once;
- persist balance snapshots and category learning tables.

CSV files remain the source of truth for transactions. The database is a local cache plus user-state store. Raw rows are kept, but `load_raw_transactions()` returns deduplicated rows: same date + account + raw amount + subcategory + memo are considered the same exported transaction, with repeated identical rows preserved up to the maximum count seen in any single source file.

These files are user-local state and should not be committed.

### dashboard/csv_importer.py

CSV importer layer. It reads all `.csv` files in a dataset folder and returns raw, source-preserving rows.

Responsibilities:

- scan CSV files;
- parse `Date` and `Amount`;
- support `Account` and `Account/Card No`;
- preserve raw `Subcategory`, `Memo`, file name, and source stem.

It should not identify accounts, classify transactions, or detect transfers.

### dashboard/transaction_processor.py

Account and transaction normalization layer.

Responsibilities:

- derive `account_key`, `account_label`, `account_type`;
- normalize credit-card purchase/payment signs;
- detect owned-account transfers from memo account references;
- detect and pair credit-card repayments;
- set `transfer_type`, `internal_reason`, `transfer_pair_id`, `excluded_from_totals`, `linked_account_key`, and `balance_affects_linked_account`.

Keep transfer logic here, not in categorization.

### dashboard/categorizer.py

Classification and learning layer.

Responsibilities:

- define built-in category options and keyword rules;
- create transaction fingerprints;
- create learned amount-rule keys;
- apply manual overrides and learned rules from `category_rules.json`;
- save user category changes.

Rule priority:

```text
manual transaction override
  -> learned amount rule
  -> built-in keyword rule
  -> Income / Other fallback
```

### dashboard/pipeline.py

Main orchestration layer. This is the preferred entrypoint for backend behavior.

Responsibilities:

- `read_transactions(csv_dir, dataset)`: sync CSVs into SQLite -> processor -> categorizer;
- `build_transactions_payload(dataset, csv_dir)`: returns the JSON shape consumed by the frontend;
- `save_balances(...)`: validates and saves balance snapshots;
- `save_category_rule_for_transaction(...)`: validates and saves category learning.

When adding a backend feature, first ask whether it belongs in importer, database, processor, categorizer, storage, or pipeline.

## Frontend files

- `public/index.html`: structure and asset version cache-busting.
- `public/app.js`: fetches JSON, holds UI state, renders metrics/charts/tables, saves category/balance edits.
- `public/styles.css`: visual styling. It is currently compact/minified; prefer small appended overrides unless doing an intentional full reformat.
- `export_mobile_report.py`: generates the offline, read-only mobile HTML report. It defaults to iCloud Drive `~/Library/Mobile Documents/com~apple~CloudDocs/Personal Financial Dashboard/` when available, or `financial-dashboard/export/` otherwise. Set `MOBILE_REPORT_EXPORT_DIR` to override. Reports use one timestamped filename per dataset, such as `mobile-report-personal-YYYYMMDD-HHMMSS.html`, and older generated snapshots for the same dataset are deleted before writing the new one. The changing filename avoids stale iOS Quick Look previews while still keeping only one generated snapshot per dataset. The report must work without JavaScript because some mobile file previews do not execute scripts; use static HTML plus CSS radio/label controls for interactions.

## Local state and privacy

Never modify source CSV files from app code. Treat CSV as read-only input.

The repo is intended to be publishable. Keep demo data fictitious. Do not commit real CSV exports, real account numbers, real learned category rules, balance snapshots, generated SQLite files, logs, or mobile reports.

User-local generated files:

- `data/financial_dashboard.sqlite3`
- `data/financial_dashboard.sqlite3-*`
- `export/*.html`
- iCloud Drive mobile reports, usually `~/Library/Mobile Documents/com~apple~CloudDocs/Personal Financial Dashboard/mobile-report-*.html`
- legacy backups may still exist: `data/account_balances.json`, `data/category_rules.json`

These are intentionally ignored by git.

## Verification checklist

After backend changes:

```bash
python3 -m py_compile financial-dashboard/server.py financial-dashboard/dashboard/*.py
```

If the server is running, restart it before API verification.

Useful API check:

```bash
curl -sS 'http://127.0.0.1:3000/api/transactions?dataset=demo'
```

Frontend changes usually only need a refresh with a new cache-busting query, e.g. `?refresh=14`.

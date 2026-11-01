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

Analytics is a separate, read-only branch:

```text
processed local transactions + balance snapshots
  -> dashboard/analytics_metrics.py deterministic penny aggregates
  -> dashboard/analytics_tools.py allowlisted function tools
  -> dashboard/analytics_context.py one-turn in-memory context
  -> dashboard/analytics_service.py tool-call orchestration / local fallback
  -> dashboard/llm_client.py optional DeepSeek Responses API
  -> server.py -> public/analytics.js
```

## Backend modules

### server.py

Thin HTTP adapter only. It serves static frontend files and maps API routes to pipeline functions.

Do not put CSV parsing, account matching, transfer detection, or category learning logic here.

Current API routes:

- `GET /api/transactions?dataset=personal|demo`
- `GET /api/analytics/status`
- `GET /assets/pet/spritesheet`
- `POST /api/balances`
- `POST /api/category-rule`
- `POST /api/manual-accounts`
- `POST /api/manual-transfers`
- `POST /api/analytics/chat`
- `POST /api/analytics/settings`
- `DELETE /api/analytics/settings`

API calls return local JSON only. Offline HTML snapshots and automatic iCloud exports have been removed; do not recreate financial report files as a side effect of reading or saving data.

### dashboard/config.py

Central paths and dataset mapping.

- `DATASETS["personal"]` points to `../csv_input` by default, or `PFD_CSV_DIR`
- `DATASETS["demo"]` points to `../csv_test_input` by default, or `PFD_DEMO_CSV_DIR`
- local SQLite state lives in `financial-dashboard/data/financial_dashboard.sqlite3`
- `PFD_PET_SPRITESHEET` optionally points to a local 8×9 pet atlas; the default is `~/.codex/pets/deepseek/spritesheet.webp`

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

### dashboard/analytics_metrics.py

The only calculation source for Analytics. It consumes processed local transactions and balance snapshots, applies the existing `excluded_from_totals` decisions, and returns aggregate money as integer pennies. It must never return memos, account identifiers, labels, filenames, paths, or transaction rows.

### dashboard/manual_accounts.py

Owns private manual Cash ISA/saver registration, dated balance history (integer pennies), exact matching identifiers, confirmed per-transaction links and opt-in exact-description rules. Tables are added without rewriting existing balances. Public account DTOs return only an identifier's last four digits. Pipeline applies manual transfer decisions before categorization. Manual balances never change from imported transfers; pending transfers after the latest observation are warnings only. History carries forward and never backfills before the first observation. Archived accounts retain history/matching but are excluded from active totals and exports. Existing CSV balances still use the original snapshot calculation.

When changing this feature, update `public/manual-accounts.js`, the web deposit chart, and `analytics_metrics.py` together. Never seed real account names, identifiers or balances into source, fixtures, or Git. Use `tests/test_manual_accounts.py` for isolated synthetic integration coverage; `tests/manual_accounts_ui.cjs` requires a fresh synthetic fixture server on port 3001.

### dashboard/analytics_privacy.py

Normalizes Unicode digits/formatting, rejects generic eight-digit account numbers in questions, and compares user text against locally known CSV/manual identifiers across datasets (including archived accounts and bank account suffixes). `AnalyticsDataset.private_identifiers` is backend-only; never serialize it. Recheck prior context and all provider input items before either provider request. Block with a generic error or local fallback; never echo an identifier or call the provider to decide whether text is private.

### dashboard/analytics_tools.py

Defines the small read-only tool allowlist and performs exact local argument validation. Never add arbitrary SQL, file-reading, transaction-search, path, or free-form account tools.

### dashboard/analytics_context.py, dashboard/analytics_service.py and dashboard/llm_client.py

`analytics_service.py` validates questions, executes one bounded tool-calling round, and falls back to deterministic local answers. `llm_client.py` is the optional DeepSeek Responses API adapter. The API key stays server-side and is loaded from `DEEPSEEK_API_KEY` or `data/llm_settings.json`; the latter is written through the local Analytics settings endpoint with mode `0600`.

`analytics_context.py` holds one previous verified turn per random conversation ID in process memory only. Entries expire after 30 minutes, the store is capped at 64 entries, and dataset/account scope is matched through a process-local HMAC. Never accept client-provided history or persist these conversations.

DeepSeek receives only the user's checked question, tool schemas, and explicitly allowlisted aggregate results. Responses API tool arguments are not trusted; local validation is mandatory. Do not log request bodies, questions, answers, tool outputs, or keys. Analytics routes must never trigger the mobile/iCloud export.

Explicit user months and unambiguous metrics are resolved locally and override model arguments. Multi-month questions must cover exactly the named months and stay within the four-call bound; otherwise use the bounded local fallback. The server also overwrites model-provided account scope with the active dashboard scope. Narration requests contain no tool definitions; model narration containing DSML/tool protocol markup must never reach the UI, so render the already verified local fact instead.

## Frontend files

- `public/index.html`: structure and asset version cache-busting.
- `public/app.js`: fetches JSON, holds UI state, renders metrics/charts/tables, saves category/balance edits.
- `public/analytics.js`: singleton floating pet/chat controller, one-turn context, status/settings and fact-card UI. Render model text with `textContent`, never as HTML.
- `public/analytics.css`: isolated floating widget, sprite and mobile bottom-sheet styling.
- `public/manual-accounts.js`: manual account/history dialog and owned-savings transfer confirmation UI. `Add new account` is creation-only; existing manual accounts are updated through `Set account balances`, alongside CSV bank/card balances. Returning from the manual editor preserves bank/card drafts.
- `public/styles.css`: visual styling. It is currently compact/minified; prefer small appended overrides unless doing an intentional full reformat.
- Mobile browsers use the live dashboard via the user's private Tailscale setup. No offline snapshot generator or iCloud report export is supported.

## Local state and privacy

Never modify source CSV files from app code. Treat CSV as read-only input.

The repo is intended to be publishable. Keep demo data fictitious. Do not commit real CSV exports, real account numbers, real learned category rules, balance snapshots, generated SQLite files, logs, or mobile reports.

The optional Analytics assistant is hybrid, not fully offline: questions and small aggregate results are sent to DeepSeek when configured. Raw rows remain local. `.env` files and API keys must never be committed.

The configured pet file remains outside the repository. `server.py` may serve only that one configured regular PNG/WebP through `/assets/pet/spritesheet`; do not add path parameters, directory serving, or copy downloaded pet art into Git.

User-local generated files:

- `data/financial_dashboard.sqlite3`
- `data/financial_dashboard.sqlite3-*`
- `data/llm_settings.json`
- `data/backups/`: private SQLite backups before account schema changes; never commit.
- `export/*.html`
- legacy backups may still exist: `data/account_balances.json`, `data/category_rules.json`

These are intentionally ignored by git.

## Verification checklist

After backend changes:

```bash
python3 -m py_compile financial-dashboard/server.py financial-dashboard/dashboard/*.py
python3 -m unittest discover -s financial-dashboard/tests -v
```

If the server is running, restart it before API verification.

Useful API check:

```bash
curl -sS 'http://127.0.0.1:3000/api/transactions?dataset=demo'
```

Frontend changes usually only need a refresh with a new cache-busting query, e.g. `?refresh=14`.

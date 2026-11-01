# Personal Financial Dashboard

Personal Financial Dashboard is a local-first dashboard for bank and credit-card CSV exports. It reads CSV files from your machine, detects internal transfers between your own accounts, excludes those movements from external income/spending, and lets you add balance snapshots to estimate your current position.

The app is designed for private local use:

- no cloud database;
- no account system;
- no telemetry;
- no network access required for your financial data;
- optional cloud-assisted Analytics remains disabled until a DeepSeek API key is configured;
- raw CSVs, learned category rules, balance snapshots, SQLite cache, logs, and mobile exports are ignored by git.

## Quick start

```bash
cd financial-dashboard
python3 server.py
```

Open <http://localhost:3000>.

The server listens on `127.0.0.1`, so it is accessible from this computer only.

## Add your own CSV files

Create a sibling folder named `csv_input` next to `financial-dashboard`:

```text
your-repo/
  csv_input/·
    statement-bank-a.csv
    statement-bank-b.csv
    statement-credit-card.csv
  csv_test_input/
    transaction_a.csv
    transaction_b.csv
    transaction_c.csv
  financial-dashboard/
```

Put your personal `*.csv` statements in `csv_input/`. The folder is gitignored.

You can also override the CSV folder:

```bash
PFD_CSV_DIR="/path/to/my/csvs" python3 server.py
```

## Demo data

The included demo dataset is in `../csv_test_input/`. It uses fictitious data with:

- two bank accounts;
- one credit card;
- salary income;
- external spending;
- bank-to-bank internal transfers;
- credit-card repayment matching.

Select **Sample demo** in the dashboard to try the app without using personal data.

## Supported CSV formats

Personal Financial Dashboard currently supports these schemas:

```csv
Number,Date,Account,Amount,Subcategory,Memo
```

and:

```csv
Date,Account/Card No,Amount,Subcategory,Memo
```

Notes:

- `Number` is optional.
- `Account/Card No` is treated as an alias of `Account`.
- Bank account identifiers are used to match owned-account transfers.
- Masked credit-card values such as `************0003` are identified as credit-card accounts.
- Positive `Purchase` rows on credit-card accounts are normalised into negative spending.

## Transfer matching

Owned-account transfers are detected from memo references such as:

```text
000000 00000002 MOBILE-CHANNEL FT
```

If `00-00-00 00000002` appears as another loaded account, the row is treated as internal and excluded from external income/spending.

Credit-card repayments are also excluded from external spending. A typical bank-side row may look like:

```text
Direct Debit, B/CARD REWARDS 0000000000000003
```

and may be paired with a credit-card-side row such as:

```text
Payment received, PAYMENT BY DIRECT DEBIT
```

## Balance snapshots

Use **Set account balances** to enter:

- current/end-of-day bank balances;
- positive credit-card debt values;
- the date of each balance snapshot.

Snapshots are stored locally in:

```text
financial-dashboard/data/financial_dashboard.sqlite3
```

The dashboard applies later transactions to estimate deposits, credit-card debt, and net position.

## Chart comparisons

The overview chart can switch between income and spending, net income, and total deposits, using either bars or lines. Its comparison control provides:

- **Month vs previous**: the latest imported month beside the immediately preceding calendar month;
- **Since year start**: every month from the first imported month in the latest year through the latest month.

Comparison ranges follow the selected account but are independent of the page's 30-day, 90-day, and single-month filters. Months without external cash flow are shown as zero; total deposits carry the latest estimated bank balance forward. A current, incomplete month is labelled **to date**.

The same two comparison ranges are pre-rendered into the JavaScript-free mobile snapshot, so they remain selectable in file viewers that support basic HTML form controls.

## Manual Cash ISA and savings accounts

Use **Add new account** on Overview and choose Cash ISA or Savings account to register an account. Enter the total confirmed balance (principal plus credited interest) and its date. To update an existing account, open **Set account balances** and select its **Update / history** button. Bank and card balances use the same entry point. No account is prefilled with personal details. Updates retain dated history; saving the same date corrects that observation. Select a history row to edit it. Leave the balance blank when editing account details only. Returning from a manual account editor preserves any unsaved bank/card balance entries.

Manual balances contribute to deposits and net position. Charts carry each observation forward, and do not assume a balance before the first recorded date. Balance updates do not create income or automatically classify a difference as interest. Archive hides an account from active totals and snapshots while retaining its history and transfer matching; uncheck Archive to restore it.

Optionally enter a sort code and account number for exact matching against transfer references. These identifiers remain in the ignored local SQLite database; the page receives only the last four digits. Matching is never based just on a bank name or amount. Alternatively, use **Link to my savings account** in Transactions. You can remember the exact description, source account, original transaction type and direction for future imports. Removing a link also removes its matching remembered rule; other individually confirmed links remain intact.

Confirmed owned-account transfers in either direction are excluded from income and spending. They never increment a manually entered savings balance. Transfers dated after the latest observation appear as **transfers to reconcile**; until you update the confirmed balance, aggregate deposits may be incomplete. Dates represent end-of-day balances, so same-day transactions are considered included in the observation.

The web dashboard, mobile snapshot and deposit analytics use the same observation semantics. Matching identifiers, rules and private history notes are omitted from mobile exports and LLM tool results. Existing CSV accounts keep their current workflow. Manual account registration does not replace an account already imported from CSV.

## Category learning

When you manually change a transaction category, the app stores:

- a transaction-specific override;
- an amount/merchant-based learned rule for future matching.

This learning data is local and ignored by git.

## Optional DeepSeek Analytics

The animated **deepseek酱** button in the lower-right corner opens a read-only financial assistant over both Overview and Transactions. All totals and comparisons are calculated locally from the same processed transactions used by the dashboard. The model can request only four allowlisted aggregate tools; it cannot read CSV files, query arbitrary SQL, inspect transaction memos, or modify local data.

The tool allowlist is:

- `get_period_summary`: exact income, spending, and net income for one month;
- `compare_financial_metric`: month-on-month, year-start, or previous-year comparison;
- `get_monthly_metric_series`: six-month, twelve-month, year-to-date, or available monthly trend;
- `get_category_breakdown`: aggregated income/spending categories for one month.

Explicit months and unambiguous income/spending metrics are resolved locally and take precedence over model-generated arguments. Questions naming two to four months must cover exactly those months; a missing, wrong, or extra model month triggers a bounded local calculation instead. The active dashboard account scope is also enforced locally. Internal model protocol text such as DSML is rejected and replaced with a locally rendered answer from the verified tool result.

Without an API key, common questions still receive deterministic local answers. With DeepSeek configured, the app sends only:

- the checked question;
- the analytics tool definitions;
- the minimum aggregate result needed to explain the answer.

Raw transactions, account/card identifiers, memos, filenames, paths, and custom category names are not sent. The verified number cards shown under each answer come directly from local calculations, not from model prose.

The widget displays only the latest question and answer. A random, process-local conversation ID lets one short follow-up inherit the preceding tool intent—for example, asking “What about the previous month?” after a category question. The server keeps at most one verified turn per conversation for 30 minutes, never writes it to disk, and starts a new context when the dataset or selected account changes. Dashboard time filters are deliberately not inherited; explicit time in the question wins.

The current default model is `deepseek-flash` (DeepSeek V4.1 Flash) through the stateless Responses API. Open the pet, select the settings button, paste the key, and choose **Save on this Mac**. The server stores it at:

```text
financial-dashboard/data/llm_settings.json
```

The file is ignored by Git, is created with owner-only `0600` permissions, and is never returned to the browser. The same panel can remove it. This is plaintext local secret storage, so keep FileVault and the macOS login password enabled.

Override the model when needed:

```bash
DEEPSEEK_MODEL="deepseek-flash" python3 server.py
```

For a temporary terminal-only session, the environment variable still takes priority over the local file:

```zsh
read -s "DEEPSEEK_API_KEY?DeepSeek API key: "
export DEEPSEEK_API_KEY
python3 server.py
```

Environment variables supported by the adapter are:

```text
DEEPSEEK_API_KEY
DEEPSEEK_BASE_URL=https://api.deepseek.com
DEEPSEEK_MODEL=deepseek-flash
DEEPSEEK_REASONING_EFFORT=none
DEEPSEEK_TIMEOUT_SECONDS=20
DEEPSEEK_MAX_OUTPUT_TOKENS=800
PFD_PET_SPRITESHEET=~/.codex/pets/deepseek/spritesheet.webp
```

This feature is hybrid rather than fully offline: the question and exact aggregate amounts used for an answer leave the Mac. Do not paste CSV rows, account numbers, card numbers, filenames, or local paths into the chat. The app blocks common forms of those inputs and does not persist chat history.

The pet spritesheet is loaded through one fixed, read-only local route and is never added to this repository. The default expects an 8-column by 9-row, 1536×1872 PNG or WebP at `~/.codex/pets/deepseek/spritesheet.webp`; use `PFD_PET_SPRITESHEET` to choose another local file. Missing or mismatched art falls back to a static assistant icon without affecting Analytics. The floating assistant is part of the live dashboard only and is not included in offline mobile snapshots.

## Mobile snapshot

Each dashboard refresh generates a read-only mobile HTML snapshot. By default, on macOS it is saved to:

```text
~/Library/Mobile Documents/com~apple~CloudDocs/Personal Financial Dashboard/mobile-report-personal-YYYYMMDD-HHMMSS.html
```

Override the export folder with:

```bash
MOBILE_REPORT_EXPORT_DIR="/path/to/export" python3 server.py
```

The snapshot is a static HTML file with CSS-only controls, so it can show data even when a mobile file preview does not run JavaScript. Each refresh deletes older generated snapshots for that dataset and writes one newly named file. The timestamped filename helps iOS Quick Look avoid showing a stale cached preview.

## Start automatically on macOS login

Install the LaunchAgent:

```bash
cd financial-dashboard
./scripts/install_launch_agent.sh
```

After installation, macOS starts the dashboard automatically when you log in. Open <http://localhost:3000>.

The LaunchAgent is named `com.personalfinancialdashboard.app`, and the launchd-managed wrapper process is named `PersonalFinancialDashboard`. The HTTP server itself still runs through Python underneath.

Uninstall:

```bash
cd financial-dashboard
./scripts/uninstall_launch_agent.sh
```

## Files that should stay private

The repository `.gitignore` excludes:

- `csv_input/`
- `financial-dashboard/data/*.sqlite3`
- `financial-dashboard/data/*.json`
- `financial-dashboard/export/*.html`
- `financial-dashboard/logs/*.log`
- Python caches and macOS metadata

Before publishing to GitHub, run:

```bash
git status --ignored
```

and confirm no real CSV, database, logs, or mobile reports are staged.

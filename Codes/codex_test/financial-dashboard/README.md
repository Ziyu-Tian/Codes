# Personal Financial Dashboard

Personal Financial Dashboard is a local-first dashboard for bank and credit-card CSV exports. It reads CSV files from your machine, detects internal transfers between your own accounts, excludes those movements from external income/spending, and lets you add balance snapshots to estimate your current position.

The app is designed for private local use:

- no cloud database;
- no account system;
- no telemetry;
- no network access required for your financial data;
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

## Category learning

When you manually change a transaction category, the app stores:

- a transaction-specific override;
- an amount/merchant-based learned rule for future matching.

This learning data is local and ignored by git.

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

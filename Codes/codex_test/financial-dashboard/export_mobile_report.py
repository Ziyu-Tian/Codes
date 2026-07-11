#!/usr/bin/env python3
"""Generate a static, read-only mobile financial report.

The report reuses the existing dashboard pipeline, so CSV importing, SQLite
deduplication, transfer detection, balance snapshots, and categorisation all
match the web dashboard.
"""

import argparse
import os
import re
from collections import defaultdict
from datetime import datetime, timedelta
from html import escape
from pathlib import Path

from dashboard.config import DATASETS, PROJECT_ROOT
from dashboard.pipeline import build_transactions_payload


def money(value):
    return f"£{value:,.2f}"


def signed_money(value):
    return f"{'+' if value >= 0 else '-'}{money(abs(value))}"


def month_label(key):
    return datetime.strptime(f"{key}-01", "%Y-%m-%d").strftime("%b %Y")


def date_label(value):
    return datetime.strptime(value, "%Y-%m-%d").strftime("%d %b")


def generated_at_label(moment=None):
    return (moment or datetime.now()).strftime("%d %b %Y, %H:%M")


def snapshot_stamp(moment=None):
    return (moment or datetime.now()).strftime("%Y%m%d-%H%M%S")


def account_label(label):
    """Keep account labels short and mobile-safe."""
    bank_match = re.search(r"Bank account [\d-]+ · (\d{4})", label)
    if bank_match:
        return f"Bank · {bank_match.group(1)}"
    card_match = re.search(r"Credit card · (\d{4})", label)
    if card_match:
        return f"Card · {card_match.group(1)}"
    return label.replace("Bank account ", "Bank · ")


def estimated_position(account, transactions, balances):
    snapshot = balances.get(account["key"])
    if not snapshot:
        return None

    def after_snapshot(item):
        return item["date"] > snapshot["as_of"]

    if account["type"] == "credit_card":
        card_activity = sum(
            item["amount"]
            for item in transactions
            if item["account_key"] == account["key"] and after_snapshot(item)
        )
        repayments = sum(
            item["amount"]
            for item in transactions
            if item.get("linked_account_key") == account["key"]
            and item.get("balance_affects_linked_account") is not False
            and after_snapshot(item)
        )
        current = snapshot["balance"] - card_activity + repayments
    else:
        movement = sum(
            item["amount"]
            for item in transactions
            if item["account_key"] == account["key"] and after_snapshot(item)
        )
        current = snapshot["balance"] + movement

    return {
        "current": current,
        "change": current - snapshot["balance"],
        "snapshot": snapshot,
    }


def portfolio_summary(payload):
    positions = []
    for account in payload["accounts"]:
        position = estimated_position(account, payload["transactions"], payload["balances"])
        positions.append({"account": account, "position": position})

    configured = [row for row in positions if row["position"]]
    deposits = sum(
        row["position"]["current"]
        for row in configured
        if row["account"]["type"] != "credit_card"
    )
    debt = sum(
        row["position"]["current"]
        for row in configured
        if row["account"]["type"] == "credit_card"
    )
    return positions, deposits, debt, deposits - debt


def month_cashflow(transactions):
    groups = defaultdict(lambda: {"income": 0.0, "spending": 0.0, "net": 0.0})
    for item in transactions:
        if item.get("excluded_from_totals"):
            continue
        key = item["date"][:7]
        if item["amount"] > 0:
            groups[key]["income"] += item["amount"]
        elif item["amount"] < 0:
            groups[key]["spending"] += abs(item["amount"])
        groups[key]["net"] += item["amount"]
    return [(key, groups[key]) for key in sorted(groups.keys(), reverse=True)]


def spending_categories(transactions, limit=8):
    groups = defaultdict(float)
    for item in transactions:
        if item.get("excluded_from_totals") or item["amount"] >= 0:
            continue
        groups[item["category"]] += abs(item["amount"])
    return sorted(groups.items(), key=lambda row: row[1], reverse=True)[:limit]


def compact_memo(value, max_length=58):
    text = " ".join(str(value or "").split())
    return text if len(text) <= max_length else f"{text[:max_length - 1]}…"


def render_metric(label, value, subtext="", tone=""):
    return f"""
      <article class="metric {tone}">
        <span>{escape(label)}</span>
        <strong>{escape(value)}</strong>
        <small>{escape(subtext)}</small>
      </article>
    """


def mobile_report_data(payload):
    """Return a privacy-trimmed dataset for offline client-side interactions."""
    key_map = {account["key"]: f"a{index}" for index, account in enumerate(payload["accounts"])}
    balances = {
        key_map[key]: value
        for key, value in payload["balances"].items()
        if key in key_map
    }
    transactions = []
    for item in payload["transactions"]:
        transactions.append({
            "date": item["date"],
            "account_key": key_map.get(item["account_key"], "unknown"),
            "amount": item["amount"],
            "category": item["category"],
            "excluded_from_totals": bool(item.get("excluded_from_totals")),
            "linked_account_key": key_map.get(item.get("linked_account_key")),
            "balance_affects_linked_account": item.get("balance_affects_linked_account") is not False,
        })
    return {
        "accounts": [
            {
                "key": key_map[account["key"]],
                "label": account_label(account["label"]),
                "type": account["type"],
            }
            for account in payload["accounts"]
        ],
        "balances": balances,
        "transactions": transactions,
        "meta": {
            "file_count": payload["file_count"],
            "transaction_count": len(payload["transactions"]),
            "duplicate_transactions_removed": payload.get("duplicate_transactions_removed", 0),
        },
    }


def mobile_estimated_position(account, data):
    snapshot = data["balances"].get(account["key"])
    if not snapshot:
        return None

    def after_snapshot(item):
        return item["date"] > snapshot["as_of"]

    if account["type"] == "credit_card":
        card_activity = sum(
            item["amount"]
            for item in data["transactions"]
            if item["account_key"] == account["key"] and after_snapshot(item)
        )
        repayments = sum(
            item["amount"]
            for item in data["transactions"]
            if item.get("linked_account_key") == account["key"]
            and item.get("balance_affects_linked_account") is not False
            and after_snapshot(item)
        )
        current = snapshot["balance"] - card_activity + repayments
    else:
        movement = sum(
            item["amount"]
            for item in data["transactions"]
            if item["account_key"] == account["key"] and after_snapshot(item)
        )
        current = snapshot["balance"] + movement
    return {"current": current, "change": current - snapshot["balance"], "snapshot": snapshot}


def mobile_portfolio_summary(data):
    positions = [
        {"account": account, "position": mobile_estimated_position(account, data)}
        for account in data["accounts"]
    ]
    configured = [row for row in positions if row["position"]]
    deposits = sum(
        row["position"]["current"]
        for row in configured
        if row["account"]["type"] != "credit_card"
    )
    debt = sum(
        row["position"]["current"]
        for row in configured
        if row["account"]["type"] == "credit_card"
    )
    return positions, deposits, debt, deposits - debt


def mobile_external_rows(rows):
    return [item for item in rows if not item.get("excluded_from_totals")]


def render_initial_metrics(data):
    external = mobile_external_rows(data["transactions"])
    incoming = [item for item in external if item["amount"] > 0]
    outgoing = [item for item in external if item["amount"] < 0]
    income = sum(item["amount"] for item in incoming)
    spending = -sum(item["amount"] for item in outgoing)
    _positions, deposits, debt, net_position = mobile_portfolio_summary(data)
    return "\n".join([
        render_metric("External net flow", money(income - spending), "Income minus spending", "net flow"),
        render_metric("External income", money(income), f"{len(incoming)} incoming payments", "positive income"),
        render_metric("External spending", money(spending), f"{len(outgoing)} outgoing payments", "negative spending"),
        render_metric("Net position", money(net_position), f"Deposits {money(deposits)} · Debt {money(debt)}", "net position"),
    ])


def render_initial_accounts(data):
    positions, _deposits, _debt, _net_position = mobile_portfolio_summary(data)
    rows = []
    for row in positions:
        account = row["account"]
        position = row["position"]
        if not position:
            rows.append(
                f'<div class="account-row"><div><strong>{escape(account["label"])}</strong>'
                '<small>No balance snapshot</small></div><span>—</span></div>'
            )
            continue
        value_label = "estimated debt" if account["type"] == "credit_card" else "estimated balance"
        change_class = "positive" if position["change"] > 0 else "negative" if position["change"] < 0 else ""
        rows.append(
            f'<div class="account-row"><div><strong>{escape(account["label"])}</strong>'
            f'<small>{escape(value_label)} · snapshot {escape(position["snapshot"]["as_of"])}</small></div>'
            f'<span>{escape(money(position["current"]))}<small class="{change_class}">'
            f'{escape(signed_money(position["change"]))}</small></span></div>'
        )
    return "\n".join(rows) or '<p class="empty">No accounts recognised.</p>'


def mobile_grouped_cashflow(rows):
    groups = defaultdict(lambda: {"income": 0.0, "spending": 0.0, "net": 0.0})
    for item in mobile_external_rows(rows):
        key = item["date"][:7]
        if item["amount"] > 0:
            groups[key]["income"] += item["amount"]
        if item["amount"] < 0:
            groups[key]["spending"] += abs(item["amount"])
        groups[key]["net"] += item["amount"]
    return [(key, groups[key]) for key in sorted(groups.keys())]


def render_initial_cashflow_chart(data):
    months = mobile_grouped_cashflow(data["transactions"])
    if not months:
        return '<p class="empty">No external cash flow.</p>'
    maximum = max(1, *[value for _key, values in months for value in (values["income"], values["spending"])])
    return "\n".join(
        f"""
        <div class="month">
          <div class="bar-pair">
            <div class="bar-column"><span>{escape(money(values["income"]))}</span><i class="bar income-bar" style="height:{values["income"] / maximum * 100}%"></i></div>
            <div class="bar-column"><span>{escape(money(values["spending"]))}</span><i class="bar spending-bar" style="height:{values["spending"] / maximum * 100}%"></i></div>
          </div>
          <small>{escape(month_label(key))}</small>
        </div>
        """
        for key, values in months
    )


def render_initial_month_rows(data):
    months = mobile_grouped_cashflow(data["transactions"])
    return "\n".join(
        f"""
        <div class="month-row">
          <div><strong>{escape(month_label(key))}</strong><small>Net {escape(money(values["net"]))}</small></div>
          <div><span class="positive">{escape(money(values["income"]))}</span><span class="negative">{escape(money(values["spending"]))}</span></div>
        </div>
        """
        for key, values in reversed(months)
    )


def render_initial_categories(data):
    groups = defaultdict(float)
    for item in mobile_external_rows(data["transactions"]):
        if item["amount"] < 0:
            groups[item["category"]] += abs(item["amount"])
    sorted_groups = sorted(groups.items(), key=lambda row: row[1], reverse=True)[:8]
    if not sorted_groups:
        return '<p class="empty">No external spending categories.</p>'
    maximum = max(1, *[value for _name, value in sorted_groups])
    return "\n".join(
        f"""
        <div class="category-row">
          <div><strong>{escape(name)}</strong><span>{escape(money(value))}</span></div>
          <i style="width:{value / maximum * 100}%"></i>
        </div>
        """
        for name, value in sorted_groups
    )


def latest_mobile_date(data):
    latest = max((item["date"] for item in data["transactions"]), default="")
    return datetime.strptime(latest, "%Y-%m-%d") if latest else None


def period_transactions(data, period):
    if period == "all":
        return data["transactions"]
    latest = latest_mobile_date(data)
    if not latest:
        return []
    start = latest - timedelta(days=int(period))
    return [
        item for item in data["transactions"]
        if datetime.strptime(item["date"], "%Y-%m-%d") >= start
    ]


def data_for_rows(data, rows):
    copy = dict(data)
    copy["transactions"] = rows
    return copy


def mobile_account_balance_at(account, data, target_date):
    snapshot = data["balances"].get(account["key"])
    if not snapshot or account["type"] == "credit_card":
        return None
    account_items = [item for item in data["transactions"] if item["account_key"] == account["key"]]
    if target_date >= snapshot["as_of"]:
        movement = sum(
            item["amount"]
            for item in account_items
            if snapshot["as_of"] < item["date"] <= target_date
        )
        return snapshot["balance"] + movement
    movement_after_target = sum(
        item["amount"]
        for item in account_items
        if target_date < item["date"] <= snapshot["as_of"]
    )
    return snapshot["balance"] - movement_after_target


def mobile_month_end(data, key):
    latest = max((item["date"] for item in data["transactions"]), default="")
    if latest and latest[:7] == key:
        return latest
    year, month = [int(part) for part in key.split("-")]
    next_month = datetime(year + 1, 1, 1) if month == 12 else datetime(year, month + 1, 1)
    return (next_month - timedelta(days=1)).strftime("%Y-%m-%d")


def render_deposits_chart(data, rows):
    bank_accounts = [
        account for account in data["accounts"]
        if account["type"] != "credit_card" and account["key"] in data["balances"]
    ]
    month_keys = sorted({
        item["date"][:7]
        for item in rows
        if any(account["key"] == item["account_key"] for account in bank_accounts)
    })
    if not month_keys:
        month_keys = sorted({data["balances"][account["key"]]["as_of"][:7] for account in bank_accounts})
    series = []
    for key in month_keys:
        as_of = mobile_month_end(data, key)
        total = 0
        for account in bank_accounts:
            balance = mobile_account_balance_at(account, data, as_of)
            if balance is not None:
                total += balance
        series.append({"key": key, "total": total})
    if not series:
        return '<p class="empty">Set bank account balances to show deposits.</p>'
    maximum = max(1, *[abs(item["total"]) for item in series])
    return "\n".join(
        f"""
        <div class="month">
          <div class="bar-pair single">
            <div class="bar-column wide"><span>{escape(money(item["total"]))}</span><i class="bar deposit-bar" style="height:{abs(item["total"]) / maximum * 100}%"></i></div>
          </div>
          <small>{escape(month_label(item["key"]))}</small>
        </div>
        """
        for item in series
    )


def render_period_panel(data, period, title):
    rows = period_transactions(data, period)
    scoped = data_for_rows(data, rows)
    return f"""
    <section class="period-panel period-{escape(str(period))}-panel">
      <section class="metrics">
        {render_initial_metrics(scoped)}
      </section>

      <section class="card">
        <div class="chart-view cashflow-view">
          <div class="section-title"><span>MONTHLY</span><h2>External cash flow · {escape(title)}</h2></div>
          <div class="chart cashflow-chart">{render_initial_cashflow_chart(scoped)}</div>
          <div>{render_initial_month_rows(scoped)}</div>
        </div>
        <div class="chart-view deposits-view">
          <div class="section-title"><span>BALANCE TREND</span><h2>Monthly total deposits · {escape(title)}</h2></div>
          <div class="chart deposit-chart">{render_deposits_chart(data, rows)}</div>
        </div>
      </section>

      <section class="card">
        <div class="section-title"><span>SPENDING</span><h2>Top categories · {escape(title)}</h2></div>
        <div>{render_initial_categories(scoped)}</div>
      </section>
    </section>
    """


def render_report(payload, dataset, generated_at=None):
    generated_at = generated_at or datetime.now()
    data = mobile_report_data(payload)

    return f"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="Cache-Control" content="no-store">
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Expires" content="0">
  <title>Personal Financial Dashboard Mobile Report — {escape(dataset)}</title>
  <style>{CSS}</style>
</head>
<body>
  <main>
    <input class="state-radio" type="radio" id="period-all" name="period" checked>
    <input class="state-radio" type="radio" id="period-30" name="period">
    <input class="state-radio" type="radio" id="chart-cashflow" name="chart" checked>
    <input class="state-radio" type="radio" id="chart-deposits" name="chart">

    <header class="hero">
      <p>PERSONAL FINANCIAL DASHBOARD</p>
      <h1>{escape(dataset.title())} financial snapshot</h1>
      <small>Generated {escape(generated_at_label(generated_at))} · {payload["file_count"]} CSV files · {len(payload["transactions"])} transactions · {payload.get("duplicate_transactions_removed", 0)} duplicates removed</small>
    </header>

    <section class="controls" aria-label="Report controls">
      <div class="segmented">
        <label for="period-all">All time</label>
        <label for="period-30">Last 30 days</label>
      </div>
      <div class="segmented">
        <label for="chart-cashflow">Income / spending</label>
        <label for="chart-deposits">Deposits</label>
      </div>
    </section>

    <section class="card">
      <div class="section-title"><span>ACCOUNTS</span><h2>Estimated position</h2></div>
      <div>{render_initial_accounts(data)}</div>
    </section>

    {render_period_panel(data, "all", "All time")}
    {render_period_panel(data, "30", "Last 30 days")}

    <footer>
      Static read-only export. Source CSV, SQLite database, transaction memo details, and JavaScript are not embedded in this file.
    </footer>
  </main>
</body>
</html>
"""


def default_export_dir():
    configured = os.getenv("MOBILE_REPORT_EXPORT_DIR")
    if configured:
        return Path(configured).expanduser()
    icloud = Path.home() / "Library" / "Mobile Documents" / "com~apple~CloudDocs"
    if icloud.exists():
        return icloud / "Personal Financial Dashboard"
    return PROJECT_ROOT / "export"


def default_output_path(dataset, moment=None):
    return default_export_dir() / f"mobile-report-{dataset}-{snapshot_stamp(moment)}.html"


def generated_report_pattern(dataset):
    escaped = re.escape(dataset)
    return re.compile(
        rf"mobile-report-{escaped}(?:-(?:latest|\d{{8}}-\d{{6}}))?\.html$"
    )


def cleanup_generated_reports(dataset, directory, keep=None):
    pattern = generated_report_pattern(dataset)
    keep = Path(keep) if keep else None
    for path in Path(directory).glob(f"mobile-report-{dataset}*.html"):
        if keep and path == keep:
            continue
        if pattern.fullmatch(path.name):
            path.unlink(missing_ok=True)


def write_mobile_report(dataset, payload, output=None):
    moment = datetime.now()
    output = Path(output) if output else default_output_path(dataset, moment)
    output.parent.mkdir(parents=True, exist_ok=True)
    html = render_report(payload, dataset, moment)
    cleanup_generated_reports(dataset, output.parent, keep=output)
    temporary = output.with_name(f"{output.name}.tmp-{os.getpid()}")
    temporary.write_text(html, encoding="utf-8")
    os.replace(temporary, output)
    os.utime(output, None)
    return output


def generate_mobile_report(dataset, output=None):
    payload = build_transactions_payload(dataset, DATASETS[dataset])
    return write_mobile_report(dataset, payload, output)


def render_account_position(account, position):
    label = account_label(account["label"])
    if not position:
        return f"""
        <div class="account-row">
          <div><strong>{escape(label)}</strong><small>No balance snapshot</small></div>
          <span>—</span>
        </div>
        """
    value_label = "estimated debt" if account["type"] == "credit_card" else "estimated balance"
    change_class = "positive" if position["change"] > 0 else "negative" if position["change"] < 0 else ""
    return f"""
      <div class="account-row">
        <div><strong>{escape(label)}</strong><small>{escape(value_label)} · snapshot {escape(position["snapshot"]["as_of"])}</small></div>
        <span>{escape(money(position["current"]))}<small class="{change_class}">{escape(signed_money(position["change"]))}</small></span>
      </div>
    """


def render_transaction(item):
    tone = "internal" if item.get("excluded_from_totals") else "income" if item["amount"] > 0 else "expense"
    badge = "Internal" if item.get("excluded_from_totals") else "Income" if item["amount"] > 0 else "Expense"
    return f"""
      <div class="transaction {tone}">
        <div>
          <strong>{escape(compact_memo(item["memo"]))}</strong>
          <small>{escape(date_label(item["date"]))} · {escape(item["category"])} · {escape(account_label(item["account_label"]))}</small>
        </div>
        <span><em>{escape(badge)}</em>{escape(signed_money(item["amount"]))}</span>
      </div>
"""


REPORT_JS = r"""
const state = { period: 'all', chart: 'cashflow' };
const data = window.REPORT_DATA;
const currency = new Intl.NumberFormat('en-GB', { style: 'currency', currency: 'GBP' });
const monthFmt = new Intl.DateTimeFormat('en-GB', { month: 'short', year: 'numeric' });

function esc(value) {
  return String(value ?? '').replace(/[&<>"']/g, char => ({
    '&': '&amp;',
    '<': '&lt;',
    '>': '&gt;',
    '"': '&quot;',
    "'": '&#39;'
  }[char]));
}

function money(value) {
  return currency.format(value || 0);
}

function signedMoney(value) {
  return `${value >= 0 ? '+' : '-'}${money(Math.abs(value || 0))}`;
}

function monthLabel(key) {
  return monthFmt.format(new Date(`${key}-02T12:00:00`));
}

function latestDate(transactions = data.transactions) {
  return transactions.reduce((latest, item) => item.date > latest ? item.date : latest, '');
}

function filteredTransactions() {
  if (state.period === 'all') return data.transactions;
  const newest = latestDate();
  const days = Number(state.period);
  return data.transactions.filter(item => (new Date(`${newest}T12:00:00`) - new Date(`${item.date}T12:00:00`)) / 86400000 <= days);
}

function externalRows(rows) {
  return rows.filter(item => !item.excluded_from_totals);
}

function estimatedPosition(account) {
  const snapshot = data.balances[account.key];
  if (!snapshot) return null;
  const afterSnapshot = item => item.date > snapshot.as_of;
  if (account.type === 'credit_card') {
    const cardActivity = data.transactions
      .filter(item => item.account_key === account.key && afterSnapshot(item))
      .reduce((sum, item) => sum + item.amount, 0);
    const repayments = data.transactions
      .filter(item => item.linked_account_key === account.key && item.balance_affects_linked_account !== false && afterSnapshot(item))
      .reduce((sum, item) => sum + item.amount, 0);
    const current = snapshot.balance - cardActivity + repayments;
    return { current, change: current - snapshot.balance, snapshot };
  }
  const movement = data.transactions
    .filter(item => item.account_key === account.key && afterSnapshot(item))
    .reduce((sum, item) => sum + item.amount, 0);
  return { current: snapshot.balance + movement, change: movement, snapshot };
}

function accountBalanceAt(account, targetDate) {
  const snapshot = data.balances[account.key];
  if (!snapshot || account.type === 'credit_card') return null;
  const accountItems = data.transactions.filter(item => item.account_key === account.key);
  if (targetDate >= snapshot.as_of) {
    const movement = accountItems
      .filter(item => item.date > snapshot.as_of && item.date <= targetDate)
      .reduce((sum, item) => sum + item.amount, 0);
    return snapshot.balance + movement;
  }
  const movementAfterTarget = accountItems
    .filter(item => item.date > targetDate && item.date <= snapshot.as_of)
    .reduce((sum, item) => sum + item.amount, 0);
  return snapshot.balance - movementAfterTarget;
}

function monthEnd(key) {
  const newest = latestDate();
  if (newest && newest.slice(0, 7) === key) return newest;
  const [year, month] = key.split('-').map(Number);
  return new Date(Date.UTC(year, month, 0)).toISOString().slice(0, 10);
}

function groupedCashflow(rows) {
  const groups = new Map();
  externalRows(rows).forEach(item => {
    const key = item.date.slice(0, 7);
    if (!groups.has(key)) groups.set(key, { income: 0, spending: 0, net: 0 });
    const current = groups.get(key);
    if (item.amount > 0) current.income += item.amount;
    if (item.amount < 0) current.spending += Math.abs(item.amount);
    current.net += item.amount;
  });
  return [...groups.entries()].sort((a, b) => a[0].localeCompare(b[0]));
}

function renderMetrics(rows) {
  const external = externalRows(rows);
  const incoming = external.filter(item => item.amount > 0);
  const outgoing = external.filter(item => item.amount < 0);
  const income = incoming.reduce((sum, item) => sum + item.amount, 0);
  const spending = -outgoing.reduce((sum, item) => sum + item.amount, 0);
  const positions = data.accounts.map(account => ({ account, position: estimatedPosition(account) })).filter(row => row.position);
  const deposits = positions.filter(row => row.account.type !== 'credit_card').reduce((sum, row) => sum + row.position.current, 0);
  const debt = positions.filter(row => row.account.type === 'credit_card').reduce((sum, row) => sum + row.position.current, 0);

  document.querySelector('.metric.flow strong').textContent = money(income - spending);
  document.querySelector('.metric.income strong').textContent = money(income);
  document.querySelector('.metric.income small').textContent = `${incoming.length} incoming payments`;
  document.querySelector('.metric.spending strong').textContent = money(spending);
  document.querySelector('.metric.spending small').textContent = `${outgoing.length} outgoing payments`;
  document.querySelector('.metric.position strong').textContent = money(deposits - debt);
  document.querySelector('.metric.position small').textContent = `Deposits ${money(deposits)} · Debt ${money(debt)}`;
}

function renderAccounts() {
  document.querySelector('#accountRows').innerHTML = data.accounts.map(account => {
    const position = estimatedPosition(account);
    if (!position) {
      return `<div class="account-row"><div><strong>${esc(account.label)}</strong><small>No balance snapshot</small></div><span>—</span></div>`;
    }
    const valueLabel = account.type === 'credit_card' ? 'estimated debt' : 'estimated balance';
    const changeClass = position.change > 0 ? 'positive' : position.change < 0 ? 'negative' : '';
    return `<div class="account-row"><div><strong>${esc(account.label)}</strong><small>${esc(valueLabel)} · snapshot ${esc(position.snapshot.as_of)}</small></div><span>${money(position.current)}<small class="${changeClass}">${signedMoney(position.change)}</small></span></div>`;
  }).join('');
}

function renderCashflowChart(rows) {
  document.querySelector('#chartEyebrow').textContent = 'MONTHLY';
  document.querySelector('#chartTitle').textContent = 'External cash flow';
  const months = groupedCashflow(rows);
  const max = Math.max(1, ...months.flatMap(([, values]) => [values.income, values.spending]));
  document.querySelector('#chart').className = 'chart cashflow-chart';
  document.querySelector('#chart').innerHTML = months.length ? months.map(([key, values]) => `
    <div class="month">
      <div class="bar-pair">
        <div class="bar-column"><span>${money(values.income)}</span><i class="bar income-bar" style="height:${values.income / max * 100}%"></i></div>
        <div class="bar-column"><span>${money(values.spending)}</span><i class="bar spending-bar" style="height:${values.spending / max * 100}%"></i></div>
      </div>
      <small>${monthLabel(key)}</small>
    </div>`).join('') : '<p class="empty">No external cash flow.</p>';
  renderMonthRows(months);
}

function renderDepositsChart(rows) {
  document.querySelector('#chartEyebrow').textContent = 'BALANCE TREND';
  document.querySelector('#chartTitle').textContent = 'Monthly total deposits';
  const bankAccounts = data.accounts.filter(account => account.type !== 'credit_card' && data.balances[account.key]);
  const monthKeys = [...new Set(rows.filter(item => bankAccounts.some(account => account.key === item.account_key)).map(item => item.date.slice(0, 7)))].sort();
  const fallbackMonths = [...new Set(bankAccounts.map(account => data.balances[account.key].as_of.slice(0, 7)))].sort();
  const months = monthKeys.length ? monthKeys : fallbackMonths;
  const series = months.map(key => {
    const asOf = monthEnd(key);
    return {
      key,
      total: bankAccounts.reduce((sum, account) => {
        const balance = accountBalanceAt(account, asOf);
        return balance === null ? sum : sum + balance;
      }, 0)
    };
  });
  const max = Math.max(1, ...series.map(item => Math.abs(item.total)));
  document.querySelector('#chart').className = 'chart deposit-chart';
  document.querySelector('#chart').innerHTML = series.length ? series.map(item => `
    <div class="month">
      <div class="bar-pair single">
        <div class="bar-column wide"><span>${money(item.total)}</span><i class="bar deposit-bar" style="height:${Math.abs(item.total) / max * 100}%"></i></div>
      </div>
      <small>${monthLabel(item.key)}</small>
    </div>`).join('') : '<p class="empty">Set bank account balances to show deposits.</p>';
  document.querySelector('#monthRows').innerHTML = series.length ? series.slice().reverse().map(item => `
    <div class="month-row"><div><strong>${monthLabel(item.key)}</strong><small>Total deposits</small></div><div><span class="positive">${money(item.total)}</span></div></div>
  `).join('') : '';
}

function renderMonthRows(months) {
  document.querySelector('#monthRows').innerHTML = months.length ? months.slice().reverse().map(([key, values]) => `
    <div class="month-row">
      <div><strong>${monthLabel(key)}</strong><small>Net ${money(values.net)}</small></div>
      <div><span class="positive">${money(values.income)}</span><span class="negative">${money(values.spending)}</span></div>
    </div>
  `).join('') : '';
}

function renderCategories(rows) {
  const groups = new Map();
  externalRows(rows).filter(item => item.amount < 0).forEach(item => {
    groups.set(item.category, (groups.get(item.category) || 0) + Math.abs(item.amount));
  });
  const sorted = [...groups.entries()].sort((a, b) => b[1] - a[1]).slice(0, 8);
  const max = Math.max(1, ...sorted.map(([, value]) => value));
  document.querySelector('#categoryRows').innerHTML = sorted.length ? sorted.map(([name, value]) => `
    <div class="category-row"><div><strong>${esc(name)}</strong><span>${money(value)}</span></div><i style="width:${value / max * 100}%"></i></div>
  `).join('') : '<p class="empty">No external spending categories.</p>';
}

function setActive(container, attr, value) {
  document.querySelectorAll(`${container} button`).forEach(button => {
    button.classList.toggle('active', button.dataset[attr] === value);
  });
}

function render() {
  const rows = filteredTransactions();
  renderMetrics(rows);
  renderAccounts();
  renderCategories(rows);
  if (state.chart === 'deposits') renderDepositsChart(rows);
  else renderCashflowChart(rows);
}

document.querySelectorAll('#periodControl button').forEach(button => {
  button.addEventListener('click', () => {
    state.period = button.dataset.period;
    setActive('#periodControl', 'period', state.period);
    render();
  });
});
document.querySelectorAll('#chartControl button').forEach(button => {
  button.addEventListener('click', () => {
    state.chart = button.dataset.chart;
    setActive('#chartControl', 'chart', state.chart);
    render();
  });
});
render();
"""


CSS = """
:root {
  color-scheme: light;
  --paper: #f7f6f0;
  --card: #fffefa;
  --ink: #17261f;
  --muted: #778179;
  --line: #e2e4dc;
  --green: #2f7858;
  --red: #a95144;
  --soft-green: #e5f1ea;
  --soft-red: #f4e6e1;
}
* { box-sizing: border-box; }
body {
  margin: 0;
  background: var(--paper);
  color: var(--ink);
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
}
main {
  width: min(100%, 740px);
  margin: 0 auto;
  padding: 20px 14px 36px;
}
.hero {
  padding: 24px 4px 18px;
}
.hero p, .section-title span, .metric span {
  margin: 0 0 8px;
  color: #8f9890;
  font-size: 11px;
  font-weight: 800;
  letter-spacing: .16em;
}
h1 {
  margin: 0 0 8px;
  font-family: "Playfair Display", Georgia, serif;
  font-weight: 600;
  font-size: clamp(32px, 10vw, 56px);
  line-height: .96;
}
h2 {
  margin: 0;
  font-family: "Playfair Display", Georgia, serif;
  font-weight: 600;
  font-size: 26px;
}
small { color: var(--muted); line-height: 1.4; }
.metrics {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 10px;
}
.controls {
  display: grid;
  gap: 10px;
  margin: 0 0 14px;
}
.segmented {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 6px;
  padding: 5px;
  background: #eceee7;
  border: 1px solid var(--line);
  border-radius: 999px;
}
.state-radio {
  position: absolute;
  opacity: 0;
  pointer-events: none;
}
.segmented label {
  display: grid;
  place-items: center;
  min-height: 40px;
  border: 0;
  border-radius: 999px;
  background: transparent;
  color: var(--muted);
  font: inherit;
  font-size: 13px;
  font-weight: 800;
  text-align: center;
  cursor: pointer;
}
#period-all:checked ~ .controls label[for="period-all"],
#period-30:checked ~ .controls label[for="period-30"],
#chart-cashflow:checked ~ .controls label[for="chart-cashflow"],
#chart-deposits:checked ~ .controls label[for="chart-deposits"] {
  background: var(--card);
  color: var(--ink);
  box-shadow: 0 4px 14px rgba(23, 38, 31, .08);
}
.period-panel,
.chart-view {
  display: none;
}
#period-all:checked ~ .period-all-panel,
#period-30:checked ~ .period-30-panel {
  display: block;
}
#chart-cashflow:checked ~ .period-panel .cashflow-view,
#chart-deposits:checked ~ .period-panel .deposits-view {
  display: block;
}
.metric, .card {
  background: var(--card);
  border: 1px solid var(--line);
  border-radius: 24px;
  box-shadow: 0 8px 24px rgba(23, 38, 31, .05);
}
.metric {
  min-height: 132px;
  padding: 18px;
}
.metric strong {
  display: block;
  margin: 14px 0 8px;
  font-family: "Playfair Display", Georgia, serif;
  font-weight: 600;
  font-size: 30px;
  line-height: 1;
}
.metric.positive strong, .positive { color: var(--green); }
.metric.negative strong, .negative { color: var(--red); }
.card {
  margin-top: 14px;
  padding: 18px;
}
.section-title {
  margin-bottom: 14px;
}
.chart {
  display: flex;
  align-items: end;
  gap: 12px;
  min-height: 190px;
  margin: 10px -4px 12px;
  padding: 14px 2px 4px;
  overflow-x: auto;
}
.month {
  flex: 1 0 74px;
  min-width: 74px;
  text-align: center;
}
.bar-pair {
  display: flex;
  align-items: end;
  justify-content: center;
  gap: 7px;
  height: 142px;
  padding: 0 2px;
}
.bar-pair.single {
  gap: 0;
}
.bar-column {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: end;
  width: 30px;
  height: 100%;
  gap: 5px;
}
.bar-column.wide {
  width: 42px;
}
.bar-column span {
  writing-mode: vertical-rl;
  transform: rotate(180deg);
  max-height: 74px;
  color: var(--muted);
  font-size: 10px;
  font-weight: 800;
}
.bar {
  display: block;
  width: 100%;
  min-height: 6px;
  border-radius: 999px 999px 4px 4px;
}
.income-bar, .deposit-bar {
  background: var(--green);
}
.spending-bar {
  background: var(--red);
}
.month small {
  display: block;
  margin-top: 8px;
  white-space: nowrap;
  font-size: 11px;
}
.account-row, .month-row {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  padding: 14px 0;
  border-top: 1px solid var(--line);
}
.account-row:first-of-type, .month-row:first-of-type { border-top: 0; }
.account-row strong { display: block; font-size: 15px; }
.account-row span {
  text-align: right;
  font-weight: 800;
  white-space: nowrap;
}
.account-row span small {
  display: block;
  font-style: normal;
  font-size: 11px;
  font-weight: 700;
  color: var(--muted);
}
.month-row > div:last-child span {
  display: block;
  text-align: right;
  font-weight: 800;
}
.category-row {
  padding: 12px 0;
}
.category-row div {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  margin-bottom: 8px;
}
.category-row i {
  display: block;
  height: 8px;
  min-width: 8px;
  border-radius: 99px;
  background: var(--green);
}
footer {
  padding: 20px 4px 0;
  color: var(--muted);
  font-size: 12px;
  text-align: center;
}
.empty {
  color: var(--muted);
}
@media (max-width: 430px) {
  .metrics { grid-template-columns: 1fr; }
  .controls { position: sticky; top: 0; z-index: 2; padding-top: 4px; background: linear-gradient(var(--paper) 76%, transparent); }
  .metric { min-height: 112px; }
  .metric strong { font-size: 34px; }
}
"""


def parse_args():
    parser = argparse.ArgumentParser(description="Generate a static mobile HTML report.")
    parser.add_argument("--dataset", default="demo", choices=sorted(DATASETS.keys()))
    parser.add_argument("--output", type=Path)
    parser.add_argument(
        "--local",
        action="store_true",
        help="Write to the project export folder instead of the default iCloud Drive location.",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    output = args.output
    if args.local and not output:
        output = PROJECT_ROOT / "export" / f"mobile-report-{args.dataset}.html"
    output = generate_mobile_report(args.dataset, output)
    print(output)


if __name__ == "__main__":
    main()

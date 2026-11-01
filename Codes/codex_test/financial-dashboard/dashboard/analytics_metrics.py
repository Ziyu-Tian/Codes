"""Deterministic, privacy-safe metrics for the Analytics assistant.

This module never calls an LLM.  It converts the processed local transaction
stream into small aggregate DTOs whose money values are integer pennies.
"""

from calendar import monthrange
from dataclasses import dataclass, field
from datetime import date
from decimal import Decimal, ROUND_HALF_UP
import re

from .categorizer import CATEGORY_OPTIONS


MONEY_QUANTUM = Decimal("0.01")
MONTH_PATTERN = re.compile(r"^\d{4}-(0[1-9]|1[0-2])$")
ALLOWED_METRICS = {"income", "spending", "net_income", "deposits"}


class AnalyticsDataError(ValueError):
    """Raised when a requested aggregate cannot be calculated safely."""


def money_decimal(value):
    return Decimal(str(value)).quantize(MONEY_QUANTUM, rounding=ROUND_HALF_UP)


def money_minor(value):
    return int((money_decimal(value) * 100).to_integral_value(rounding=ROUND_HALF_UP))


def month_end(month_key):
    year, month = (int(part) for part in month_key.split("-"))
    return date(year, month, monthrange(year, month)[1])


def month_start(month_key):
    year, month = (int(part) for part in month_key.split("-"))
    return date(year, month, 1)


def shift_month(month_key, offset):
    year, month = (int(part) for part in month_key.split("-"))
    index = year * 12 + month - 1 + offset
    return f"{index // 12:04d}-{index % 12 + 1:02d}"


def iter_months(start_key, end_key):
    key = start_key
    while key <= end_key:
        yield key
        key = shift_month(key, 1)


@dataclass
class AnalyticsDataset:
    """A request-scoped view of already processed local financial data."""

    dataset: str
    transactions: list
    accounts: list
    balances: dict
    selected_account: str | None = None
    private_identifiers: tuple = field(default=(), repr=False)

    @classmethod
    def from_local(cls, dataset, csv_dir, selected_account=None):
        # Local imports avoid introducing a pipeline -> analytics cycle.
        from .pipeline import build_transactions_payload
        from .analytics_privacy import load_private_identifiers

        payload = build_transactions_payload(dataset, csv_dir)
        transactions = payload["transactions"]
        account_map = {a["key"]: a for a in payload["accounts"] if not a.get("archived")}
        accounts = sorted(account_map.values(), key=lambda item: item["key"])
        if selected_account and selected_account not in account_map:
            raise AnalyticsDataError("The selected account is not part of this dataset.")
        return cls(
            dataset=dataset,
            transactions=transactions,
            accounts=accounts,
            balances=payload["balances"],
            selected_account=selected_account,
            private_identifiers=load_private_identifiers(),
        )

    def _validate_scope(self, account_scope):
        if account_scope not in {"all", "selected"}:
            raise AnalyticsDataError("account_scope must be all or selected.")
        if account_scope == "selected" and not self.selected_account:
            raise AnalyticsDataError("No account is selected in the dashboard.")

    def scoped_transactions(self, account_scope):
        self._validate_scope(account_scope)
        if account_scope == "all":
            return list(self.transactions)
        return [
            item for item in self.transactions
            if item.get("account_key") == self.selected_account
        ]

    def scoped_accounts(self, account_scope):
        self._validate_scope(account_scope)
        if account_scope == "all":
            return list(self.accounts)
        return [
            account for account in self.accounts
            if account.get("key") == self.selected_account
        ]

    def available_dates(self, account_scope, include_snapshots=True):
        dates = [item.get("date", "") for item in self.scoped_transactions(account_scope)]
        if include_snapshots:
            for account in self.scoped_accounts(account_scope):
                snapshot = self.balances.get(account["key"])
                if snapshot:
                    dates.append(snapshot.get("as_of", ""))
        return sorted(value for value in dates if value)

    def latest_date(self, account_scope):
        dates = self.available_dates(account_scope)
        if not dates:
            raise AnalyticsDataError("No transaction or balance dates are available.")
        return dates[-1]

    def first_date(self, account_scope):
        dates = self.available_dates(account_scope, include_snapshots=False)
        if not dates:
            dates = self.available_dates(account_scope)
        if not dates:
            raise AnalyticsDataError("No transaction or balance dates are available.")
        return dates[0]

    def resolve_month(self, period, account_scope):
        if period in {"latest", "current_month"}:
            return self.latest_date(account_scope)[:7]
        if period == "previous_month":
            return shift_month(self.latest_date(account_scope)[:7], -1)
        if not isinstance(period, str) or not MONTH_PATTERN.fullmatch(period):
            raise AnalyticsDataError("Period must be latest, current_month, previous_month, or YYYY-MM.")
        return period

    def _period_end(self, month_key, account_scope, through_day=None):
        calendar_end = month_end(month_key)
        latest = date.fromisoformat(self.latest_date(account_scope))
        if month_key > latest.strftime("%Y-%m"):
            raise AnalyticsDataError("The requested month is later than the latest local data.")
        end = min(calendar_end, latest) if month_key == latest.strftime("%Y-%m") else calendar_end
        if through_day is not None:
            capped_day = min(int(through_day), calendar_end.day)
            end = min(end, date(calendar_end.year, calendar_end.month, capped_day))
        return end

    def get_period_summary(self, period, account_scope="all", through_day=None):
        month_key = self.resolve_month(period, account_scope)
        start = month_start(month_key)
        end = self._period_end(month_key, account_scope, through_day)
        external = []
        for item in self.scoped_transactions(account_scope):
            item_date = date.fromisoformat(item["date"])
            if start <= item_date <= end and not item.get("excluded_from_totals", False):
                external.append(item)

        income = sum(
            (money_decimal(item["amount"]) for item in external if item["amount"] > 0),
            Decimal("0"),
        )
        spending = sum(
            (-money_decimal(item["amount"]) for item in external if item["amount"] < 0),
            Decimal("0"),
        )
        excluded_count = sum(
            1
            for item in self.scoped_transactions(account_scope)
            if start <= date.fromisoformat(item["date"]) <= end
            and item.get("excluded_from_totals", False)
        )
        return {
            "currency": "GBP",
            "period": {
                "month": month_key,
                "start": start.isoformat(),
                "end": end.isoformat(),
                "partial": end < month_end(month_key),
            },
            "values": {
                "income_minor": money_minor(income),
                "spending_minor": money_minor(spending),
                "net_income_minor": money_minor(income - spending),
            },
            "counts": {
                "external": len(external),
                "excluded_internal": excluded_count,
            },
            "definition": "net_income = external_income - external_spending",
        }

    def deposits_at(self, target_date, account_scope="all"):
        if isinstance(target_date, str):
            target_date = date.fromisoformat(target_date)
        configured = []
        for account in self.scoped_accounts(account_scope):
            if account.get("type") == "credit_card":
                continue
            snapshot = self.balances.get(account["key"])
            if snapshot:
                configured.append((account, snapshot))
        if not configured:
            raise AnalyticsDataError("Bank balance snapshots are required for deposit analytics.")

        total = Decimal("0")
        recorded_accounts = 0
        all_items = self.scoped_transactions(account_scope)
        for account, snapshot in configured:
            if snapshot.get("source") == "manual":
                from .manual_accounts import balance_at
                value = balance_at(snapshot, target_date.isoformat())
                if value is not None:
                    total += money_decimal(value)
                    recorded_accounts += 1
                continue
            recorded_accounts += 1
            snapshot_date = date.fromisoformat(snapshot["as_of"])
            balance = money_decimal(snapshot["balance"])
            account_items = [
                item for item in all_items
                if item.get("account_key") == account["key"]
            ]
            if target_date >= snapshot_date:
                movement = sum(
                    (
                        money_decimal(item["amount"])
                        for item in account_items
                        if snapshot_date < date.fromisoformat(item["date"]) <= target_date
                    ),
                    Decimal("0"),
                )
                balance += movement
            else:
                movement = sum(
                    (
                        money_decimal(item["amount"])
                        for item in account_items
                        if target_date < date.fromisoformat(item["date"]) <= snapshot_date
                    ),
                    Decimal("0"),
                )
                balance -= movement
            total += balance
        if not recorded_accounts:
            raise AnalyticsDataError("No balance was recorded for the requested date.")
        return {
            "value_minor": money_minor(total),
            "configured_bank_accounts": recorded_accounts,
            "unrecorded_accounts": len(configured) - recorded_accounts,
            "as_of": target_date.isoformat(),
        }

    def compare_financial_metric(
        self,
        metric,
        current_period,
        baseline,
        alignment,
        account_scope="all",
    ):
        if metric not in ALLOWED_METRICS:
            raise AnalyticsDataError("Unsupported financial metric.")
        if baseline not in {"previous_month", "year_start", "previous_year"}:
            raise AnalyticsDataError("Unsupported comparison baseline.")
        if alignment not in {"matched_days", "full_period"}:
            raise AnalyticsDataError("Unsupported comparison alignment.")

        current_key = self.resolve_month(current_period, account_scope)
        if baseline == "previous_month":
            baseline_key = shift_month(current_key, -1)
        elif baseline == "previous_year":
            baseline_key = f"{int(current_key[:4]) - 1:04d}-{current_key[5:]}"
        else:
            baseline_key = f"{current_key[:4]}-01"

        current = self.get_period_summary(current_key, account_scope)
        through_day = date.fromisoformat(current["period"]["end"]).day if alignment == "matched_days" else None
        baseline_summary = self.get_period_summary(baseline_key, account_scope, through_day=through_day)

        if metric == "deposits":
            current_deposits = self.deposits_at(current["period"]["end"], account_scope)
            baseline_deposits = self.deposits_at(baseline_summary["period"]["end"], account_scope)
            current_value = current_deposits["value_minor"]
            baseline_value = baseline_deposits["value_minor"]
        else:
            value_key = f"{metric}_minor"
            current_value = current["values"][value_key]
            baseline_value = baseline_summary["values"][value_key]

        delta = current_value - baseline_value
        percent = None
        if baseline_value:
            percent = float(
                (Decimal(delta) / Decimal(abs(baseline_value)) * 100).quantize(Decimal("0.01"))
            )
        return {
            "metric": metric,
            "currency": "GBP",
            "alignment": alignment,
            "current": {
                "period": current_key,
                "start": current["period"]["start"],
                "end": current["period"]["end"],
                "partial": current["period"]["partial"],
                "value_minor": current_value,
            },
            "baseline": {
                "kind": baseline,
                "period": baseline_key,
                "start": baseline_summary["period"]["start"],
                "end": baseline_summary["period"]["end"],
                "value_minor": baseline_value,
            },
            "change": {
                "absolute_minor": delta,
                "percent": percent,
            },
            "definition": (
                "net_income = external_income - external_spending"
                if metric == "net_income"
                else "Internal movements and credit-card repayments are excluded from cash-flow metrics."
            ),
        }

    def get_monthly_metric_series(self, metric, range_name, account_scope="all"):
        if metric not in ALLOWED_METRICS:
            raise AnalyticsDataError("Unsupported financial metric.")
        if range_name not in {"last_6_months", "last_12_months", "year_to_date", "all_available"}:
            raise AnalyticsDataError("Unsupported trend range.")
        latest = self.latest_date(account_scope)[:7]
        if range_name == "last_6_months":
            first = shift_month(latest, -5)
        elif range_name == "last_12_months":
            first = shift_month(latest, -11)
        elif range_name == "year_to_date":
            first = f"{latest[:4]}-01"
        else:
            first = max(self.first_date(account_scope)[:7], shift_month(latest, -23))

        points = []
        for month_key in iter_months(first, latest):
            summary = self.get_period_summary(month_key, account_scope)
            if metric == "deposits":
                snapshots = [self.balances.get(a["key"]) for a in self.scoped_accounts(account_scope)
                             if a.get("type") != "credit_card" and self.balances.get(a["key"])]
                if snapshots and all(s.get("source") == "manual" for s in snapshots):
                    observed = [row["as_of"] for s in snapshots for row in s.get("history", [])]
                    if observed and summary["period"]["end"] < min(observed):
                        continue
                value = self.deposits_at(summary["period"]["end"], account_scope)["value_minor"]
            else:
                value = summary["values"][f"{metric}_minor"]
            points.append({
                "month": month_key,
                "value_minor": value,
                "through": summary["period"]["end"],
                "partial": summary["period"]["partial"],
            })
        return {
            "metric": metric,
            "currency": "GBP",
            "range": range_name,
            "points": points,
            "definition": (
                "net_income = external_income - external_spending"
                if metric == "net_income"
                else "Internal movements and credit-card repayments are excluded from cash-flow metrics."
            ),
        }

    def get_category_breakdown(self, period, direction, limit, account_scope="all"):
        if direction not in {"income", "spending", "all"}:
            raise AnalyticsDataError("Unsupported category direction.")
        if not isinstance(limit, int) or isinstance(limit, bool) or not 1 <= limit <= 10:
            raise AnalyticsDataError("Category limit must be an integer from 1 to 10.")
        summary = self.get_period_summary(period, account_scope)
        start = date.fromisoformat(summary["period"]["start"])
        end = date.fromisoformat(summary["period"]["end"])
        totals = {}
        counts = {}
        for item in self.scoped_transactions(account_scope):
            item_date = date.fromisoformat(item["date"])
            if not start <= item_date <= end or item.get("excluded_from_totals", False):
                continue
            if direction == "income" and item["amount"] <= 0:
                continue
            if direction == "spending" and item["amount"] >= 0:
                continue
            category = item.get("category") or "Other"
            totals[category] = totals.get(category, Decimal("0")) + abs(money_decimal(item["amount"]))
            counts[category] = counts.get(category, 0) + 1

        ordered = sorted(totals, key=lambda key: (-totals[key], key.lower()))
        custom_aliases = {
            category: f"Custom category {index + 1}"
            for index, category in enumerate(
                sorted((value for value in ordered if value not in CATEGORY_OPTIONS), key=str.lower)
            )
        }
        categories = [
            {
                "category": category if category in CATEGORY_OPTIONS else custom_aliases[category],
                "amount_minor": money_minor(totals[category]),
                "transaction_count": counts[category],
            }
            for category in ordered[:limit]
        ]
        return {
            "currency": "GBP",
            "period": summary["period"],
            "direction": direction,
            "categories": categories,
            "custom_categories_are_aliased": bool(custom_aliases),
        }

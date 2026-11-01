"""Synthetic integration coverage; never opens the user's database or CSVs."""
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from dashboard import database, manual_accounts
from dashboard.pipeline import build_transactions_payload, save_manual_account, link_manual_transfer, save_balances
from dashboard.analytics_metrics import AnalyticsDataset, AnalyticsDataError
from export_mobile_report import mobile_report_data, mobile_account_balance_at, mobile_estimated_position, render_report, mobile_deposit_series


class ManualAccountsTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        root = Path(self.temp.name)
        for attr, value in {"DATABASE_PATH": root / "test.sqlite3", "BALANCES_PATH": root / "none-balances.json",
                            "CATEGORY_RULES_PATH": root / "none-rules.json"}.items():
            patcher = patch.object(database, attr, value)
            patcher.start()
            self.addCleanup(patcher.stop)
        self.csv = root / "csv"
        self.csv.mkdir()
        # Fictitious identifiers, including an exact internal transfer and a same-bank purchase.
        (self.csv / "test.csv").write_text(
            "Date,Account,Amount,Subcategory,Memo\n"
            "01/09/2026,00-00-00 00000001,1000,Counter Credit,EXAMPLE PAYROLL\n"
            "02/10/2026,00-00-00 00000001,-100,Funds Transfer,000000 00000002 MOBILE-CHANNEL FT\n"
            "03/10/2026,00-00-00 00000001,-20,Purchase,EXAMPLE SAVER BANK SHOP\n"
            "04/10/2026,00-00-00 00000001,-30,Funds Transfer,MY SAVINGS REFERENCE\n"
            "05/10/2026,00-00-00 00000001,-40,Funds Transfer,MY SAVINGS REFERENCE\n"
            "06/10/2026,00-00-00 00000001,10,Funds Transfer,000000 00000002 MOBILE-CHANNEL FT\n",
            encoding="utf-8",
        )
        self.entry = dict(label="Example ISA", type="cash_isa", identifier="00-00-00 00000002",
                          balance="1000.00", as_of="2026-09-30", note="private observation")

    def create(self):
        payload = save_manual_account("demo", self.csv, self.entry)
        key = next(a["key"] for a in payload["accounts"] if a.get("source") == "manual")
        return key, payload

    def test_exact_transfer_both_directions_excluded_without_balance_increment(self):
        key, payload = self.create()
        linked = [t for t in payload["transactions"] if t.get("linked_account_key") == key]
        self.assertEqual(len(linked), 2)
        self.assertTrue(all(t["excluded_from_totals"] for t in linked))
        self.assertTrue(all(t["balance_affects_linked_account"] is False for t in linked))
        self.assertEqual(payload["balances"][key]["balance"], 1000)
        shop = next(t for t in payload["transactions"] if "SHOP" in t["memo"])
        self.assertFalse(shop["excluded_from_totals"])

    def test_private_identifier_not_returned_and_dataset_isolation(self):
        key, payload = self.create()
        account = next(a for a in payload["accounts"] if a["key"] == key)
        self.assertEqual(account["identifier_hint"], "•••• 0002")
        self.assertNotIn("identifier", account)
        self.assertEqual(manual_accounts.load_accounts("personal"), [])
        self.assertEqual(manual_accounts.balance_store("personal"), {})

    def test_history_corrections_forward_only_and_existing_balances_survive(self):
        key, _ = self.create()
        save_balances("demo", self.csv, [{"account_key": "00000000000001", "balance": 2000, "as_of": "2026-09-30"}])
        payload = save_manual_account("demo", self.csv, {**self.entry, "key": key, "identifier": "",
                                                        "balance": "1105", "as_of": "2026-10-31"})
        payload = save_manual_account("demo", self.csv, {**self.entry, "key": key, "identifier": "",
                                                        "balance": "1106", "as_of": "2026-10-31"})
        snapshot = payload["balances"][key]
        self.assertEqual(len(snapshot["history"]), 2)
        self.assertIsNone(manual_accounts.balance_at(snapshot, "2026-09-01"))
        self.assertEqual(manual_accounts.balance_at(snapshot, "2026-10-15"), 1000)
        self.assertEqual(manual_accounts.balance_at(snapshot, "2026-10-31"), 1106)
        self.assertEqual(payload["balances"]["00000000000001"]["balance"], 2000)
        self.assertEqual(manual_accounts.load_accounts("demo", private=True)[0]["identifier"], "00000000000002")

    def test_confirm_exact_rule_relearns_amounts_and_can_be_removed(self):
        key, payload = self.create()
        item = next(t for t in payload["transactions"] if t["amount"] == -30)
        payload = link_manual_transfer("demo", self.csv, {"fingerprint": item["transaction_fingerprint"],
                                                        "account_key": key, "remember": True})
        rows = [t for t in payload["transactions"] if "MY SAVINGS" in t["memo"]]
        self.assertEqual(len(rows), 2)
        self.assertTrue(all(t["excluded_from_totals"] for t in rows))
        payload = link_manual_transfer("demo", self.csv, {"fingerprint": item["transaction_fingerprint"], "account_key": None})
        self.assertFalse(any(t["excluded_from_totals"] for t in payload["transactions"] if "MY SAVINGS" in t["memo"]))

    def test_analytics_and_snapshot_agree_and_never_include_private_matching_data(self):
        key, payload = self.create()
        data = AnalyticsDataset.from_local("demo", self.csv, selected_account=key)
        self.assertEqual(data.deposits_at("2026-10-31", "selected")["value_minor"], 100000)
        with self.assertRaises(AnalyticsDataError):
            data.deposits_at("2026-09-01", "selected")
        trimmed = mobile_report_data(payload)
        account = next(a for a in trimmed["accounts"] if a.get("source") == "manual")
        self.assertIsNone(mobile_account_balance_at(account, trimmed, "2026-09-01"))
        self.assertEqual(mobile_account_balance_at(account, trimmed, "2026-10-31"), 1000)
        self.assertEqual(mobile_estimated_position(account, trimmed)["current"], 1000)
        serialized = json.dumps(trimmed)
        self.assertNotIn("00000000000002", serialized)
        self.assertNotIn("private observation", serialized)
        html = render_report(payload, "demo")
        self.assertNotIn("00000000000002", html)
        self.assertNotIn("private observation", html)
        self.assertIn("transfers to reconcile", html)
        series = data.get_monthly_metric_series("deposits", "last_6_months", "selected")
        self.assertEqual(series["points"][0]["month"], "2026-09")

    def test_latest_manual_month_appears_without_a_csv_in_that_month(self):
        key, _ = self.create()
        payload = save_manual_account("demo", self.csv, {**self.entry, "key": key, "identifier": "",
                                                        "as_of": "2026-11-01", "balance": "1105"})
        trimmed = mobile_report_data(payload)
        series = mobile_deposit_series(trimmed, trimmed["transactions"])
        self.assertEqual(series[-1], {"key": "2026-11", "total": 1105})

    def test_invalid_account_does_not_persist_partial_state(self):
        for change in ({"balance": "nan"}, {"balance": "-1"}, {"identifier": "123"},
                       {"type": "invalid"}, {"as_of": "not-a-date"}, {"identifier": "00-00-00 00000001"}):
            with self.subTest(change=change), self.assertRaises(ValueError):
                save_manual_account("demo", self.csv, {**self.entry, **change})
        self.assertEqual(manual_accounts.load_accounts("demo"), [])

    def test_archive_keeps_history_and_matching_but_hides_from_current_analytics(self):
        key, _ = self.create()
        payload = save_manual_account("demo", self.csv, {**self.entry, "key": key, "archived": True, "balance": ""})
        self.assertEqual(len(payload["balances"][key]["history"]), 1)
        self.assertTrue(any(t.get("linked_account_key") == key for t in payload["transactions"]))
        self.assertFalse(any(a["key"] == key for a in AnalyticsDataset.from_local("demo", self.csv).accounts))
        self.assertFalse(any(a.get("source") == "manual" for a in mobile_report_data(payload)["accounts"]))

import unittest

from fixtures import SENSITIVE_CANARY, demo_dataset


class AnalyticsMetricsTests(unittest.TestCase):
    def setUp(self):
        self.data = demo_dataset()

    def test_demo_monthly_gold_values_use_integer_pennies(self):
        may = self.data.get_period_summary("2026-05")
        june = self.data.get_period_summary("2026-06")

        self.assertEqual(
            may["values"],
            {"income_minor": 250000, "spending_minor": 22920, "net_income_minor": 227080},
        )
        self.assertEqual(
            june["values"],
            {"income_minor": 251245, "spending_minor": 76143, "net_income_minor": 175102},
        )
        self.assertEqual(may["counts"], {"external": 7, "excluded_internal": 2})
        self.assertEqual(june["counts"], {"external": 15, "excluded_internal": 4})
        for summary in (may, june):
            self.assertEqual(
                summary["values"]["net_income_minor"],
                summary["values"]["income_minor"] - summary["values"]["spending_minor"],
            )
            self.assertTrue(all(isinstance(value, int) for value in summary["values"].values()))

    def test_previous_month_full_period_comparison(self):
        result = self.data.compare_financial_metric(
            "net_income", "2026-06", "previous_month", "full_period"
        )
        self.assertEqual(result["current"]["value_minor"], 175102)
        self.assertEqual(result["baseline"]["value_minor"], 227080)
        self.assertEqual(result["change"]["absolute_minor"], -51978)
        self.assertEqual(result["change"]["percent"], -22.89)

    def test_matched_days_caps_baseline_to_latest_imported_day(self):
        result = self.data.compare_financial_metric(
            "net_income", "latest", "previous_month", "matched_days"
        )
        self.assertEqual(result["current"]["end"], "2026-06-28")
        self.assertEqual(result["baseline"]["end"], "2026-05-28")

    def test_custom_category_is_aliased_before_leaving_metrics_layer(self):
        data = demo_dataset(add_sensitive_canary=True)
        result = data.get_category_breakdown("2026-06", "spending", 10)
        serialised = str(result)
        self.assertNotIn(SENSITIVE_CANARY, serialised)
        self.assertTrue(result["custom_categories_are_aliased"])

    def test_deposit_series_uses_only_configured_bank_accounts(self):
        result = self.data.get_monthly_metric_series("deposits", "last_6_months")
        self.assertEqual(result["points"][-1]["value_minor"], 300000)
        self.assertTrue(all(isinstance(point["value_minor"], int) for point in result["points"]))


if __name__ == "__main__":
    unittest.main()

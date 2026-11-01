import json
import unittest

from dashboard.analytics_tools import AnalyticsToolError, AnalyticsToolRunner, parse_tool_arguments
from fixtures import demo_dataset


class AnalyticsToolTests(unittest.TestCase):
    def test_valid_exact_arguments_are_executed(self):
        runner = AnalyticsToolRunner(demo_dataset())
        result = runner.execute_json_call(
            "get_period_summary",
            json.dumps({"period": "2026-06", "account_scope": "all"}),
        )
        self.assertEqual(result["values"]["net_income_minor"], 175102)

    def test_unknown_tool_is_rejected(self):
        with self.assertRaises(AnalyticsToolError):
            parse_tool_arguments("query_sql", "{}")

    def test_extra_arguments_are_rejected(self):
        with self.assertRaises(AnalyticsToolError):
            parse_tool_arguments(
                "get_period_summary",
                json.dumps({"period": "latest", "account_scope": "all", "path": "/tmp/private.csv"}),
            )

    def test_malformed_json_is_rejected(self):
        with self.assertRaises(AnalyticsToolError):
            parse_tool_arguments("get_period_summary", "not-json")

    def test_invalid_month_is_rejected_locally(self):
        runner = AnalyticsToolRunner(demo_dataset())
        with self.assertRaises(AnalyticsToolError):
            runner.execute_json_call(
                "get_period_summary",
                json.dumps({"period": "2026-99", "account_scope": "all"}),
            )


if __name__ == "__main__":
    unittest.main()

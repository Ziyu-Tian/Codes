import json
import unittest
from copy import deepcopy
from datetime import date

from dashboard.analytics_context import AnalyticsConversationStore
from dashboard.analytics_metrics import shift_month
from dashboard.analytics_service import (
    AnalyticsInputError,
    AnalyticsService,
    contains_tool_protocol_markup,
    explicit_period_from_question,
    validate_question,
)
from dashboard.llm_client import DeepSeekResponsesClient
from fixtures import SENSITIVE_CANARY, demo_dataset


class FakeDeepSeekClient:
    configured = True
    model = "deepseek-flash"

    def __init__(self, responses):
        self.responses = list(responses)
        self.calls = []

    def public_status(self):
        return {"provider": "deepseek", "configured": True, "model": self.model}

    def create_response(self, **kwargs):
        self.calls.append(deepcopy(kwargs))
        response = self.responses.pop(0)
        if isinstance(response, Exception):
            raise response
        return response


def tool_selection(name="get_period_summary", arguments=None):
    arguments = arguments or {"period": "2026-06", "account_scope": "all"}
    return {
        "status": "completed",
        "model": "deepseek-flash",
        "output": [{
            "type": "function_call",
            "call_id": "call-safe-1",
            "name": name,
            "arguments": json.dumps(arguments),
        }],
        "usage": {"input_tokens": 100, "output_tokens": 20},
    }


def narration(text="The exact net income is shown in the verified card."):
    return {
        "status": "completed",
        "model": "deepseek-flash",
        "output": [{
            "type": "message",
            "content": [{"type": "output_text", "text": text}],
        }],
        "usage": {"input_tokens": 120, "output_tokens": 30},
    }


def july_august_dataset(selected_account=None, dataset="demo"):
    """Shift the two-month demo fixture forward for the documented follow-up example."""
    data = demo_dataset(selected_account=selected_account)
    data.dataset = dataset
    for item in data.transactions:
        old_date = date.fromisoformat(item["date"])
        shifted_month = shift_month(old_date.strftime("%Y-%m"), 2)
        item["date"] = f"{shifted_month}-{old_date.day:02d}"
    for snapshot in data.balances.values():
        old_date = date.fromisoformat(snapshot["as_of"])
        shifted_month = shift_month(old_date.strftime("%Y-%m"), 2)
        snapshot["as_of"] = f"{shifted_month}-{old_date.day:02d}"
    return data


class AnalyticsServiceTests(unittest.TestCase):
    def test_injected_empty_conversation_store_is_used(self):
        store = AnalyticsConversationStore()
        client = FakeDeepSeekClient([])
        client.configured = False

        result = AnalyticsService(client, conversation_store=store).answer(
            demo_dataset(),
            "五月的支出分类是什么？",
        )

        self.assertEqual(len(store), 1)
        self.assertIsNotNone(store.get(
            result["conversation_id"],
            store.context_key("demo", "all", None),
        ))

    def test_followup_previous_month_reuses_category_intent_and_recalculates(self):
        store = AnalyticsConversationStore()
        client = FakeDeepSeekClient([
            tool_selection(
                name="get_category_breakdown",
                arguments={
                    "period": "latest",
                    "direction": "spending",
                    "limit": 5,
                    "account_scope": "all",
                },
            ),
            narration("The verified August breakdown is ready."),
            # Deliberately wrong: the service must use the normalized prior intent.
            tool_selection(
                name="get_period_summary",
                arguments={"period": "latest", "account_scope": "all"},
            ),
            narration("The verified July breakdown is ready."),
        ])
        service = AnalyticsService(client, conversation_store=store)
        data = july_august_dataset()

        first = service.answer(data, "八月的支出分类是什么？")
        second = service.answer(
            data,
            "那上个月呢？",
            conversation_id=first["conversation_id"],
        )

        self.assertFalse(first["context_used"])
        self.assertTrue(second["context_used"])
        self.assertEqual(second["conversation_id"], first["conversation_id"])
        self.assertEqual(second["facts"][0]["name"], "get_category_breakdown")
        self.assertEqual(second["facts"][0]["result"]["period"]["month"], "2026-07")
        second_narration_call = client.calls[3]
        rewritten_call = next(
            item for item in second_narration_call["input_items"]
            if item.get("type") == "function_call"
        )
        self.assertEqual(rewritten_call["name"], "get_category_breakdown")
        self.assertEqual(json.loads(rewritten_call["arguments"])["period"], "2026-07")

    def test_explicit_followup_month_and_metric_override_previous_context(self):
        store = AnalyticsConversationStore()
        client = FakeDeepSeekClient([])
        client.configured = False
        service = AnalyticsService(client, conversation_store=store)
        data = july_august_dataset()

        first = service.answer(data, "八月的支出分类是什么？")
        second = service.answer(
            data,
            "那七月的收入分类呢？",
            conversation_id=first["conversation_id"],
        )

        self.assertTrue(second["context_used"])
        self.assertEqual(second["facts"][0]["name"], "get_category_breakdown")
        self.assertEqual(second["facts"][0]["result"]["period"]["month"], "2026-07")
        self.assertEqual(second["facts"][0]["result"]["direction"], "income")

    def test_followup_category_inherits_previous_income_direction(self):
        store = AnalyticsConversationStore()
        client = FakeDeepSeekClient([])
        client.configured = False
        service = AnalyticsService(client, conversation_store=store)
        data = july_august_dataset()

        first = service.answer(data, "八月的收入分类是什么？")
        second = service.answer(
            data,
            "那七月的分类呢？",
            conversation_id=first["conversation_id"],
        )

        self.assertEqual(second["facts"][0]["name"], "get_category_breakdown")
        self.assertEqual(second["facts"][0]["result"]["period"]["month"], "2026-07")
        self.assertEqual(second["facts"][0]["result"]["direction"], "income")

    def test_followup_trend_inherits_previous_spending_metric(self):
        store = AnalyticsConversationStore()
        client = FakeDeepSeekClient([])
        client.configured = False
        service = AnalyticsService(client, conversation_store=store)
        data = july_august_dataset()

        first = service.answer(data, "支出趋势如何？")
        second = service.answer(
            data,
            "那今年的趋势呢？",
            conversation_id=first["conversation_id"],
        )

        result = second["facts"][0]["result"]
        self.assertEqual(second["facts"][0]["name"], "get_monthly_metric_series")
        self.assertEqual(result["metric"], "spending")
        self.assertEqual(result["range"], "year_to_date")

    def test_unknown_expired_or_cross_scope_id_does_not_reuse_context(self):
        class Clock:
            now = 0

            def __call__(self):
                return self.now

        clock = Clock()
        store = AnalyticsConversationStore(ttl_seconds=1_800, clock=clock)
        client = FakeDeepSeekClient([])
        client.configured = False
        service = AnalyticsService(client, conversation_store=store)
        data = july_august_dataset()
        first = service.answer(data, "八月的支出分类是什么？")

        unknown = service.answer(data, "八月的支出是多少？", conversation_id="unknown")
        self.assertFalse(unknown["context_used"])
        self.assertNotEqual(unknown["conversation_id"], "unknown")

        selected_key = data.accounts[0]["key"]
        selected_data = july_august_dataset(selected_account=selected_key)
        cross_scope = service.answer(
            selected_data,
            "八月的支出是多少？",
            account_scope="selected",
            conversation_id=first["conversation_id"],
        )
        self.assertFalse(cross_scope["context_used"])
        self.assertNotEqual(cross_scope["conversation_id"], first["conversation_id"])

        clock.now = 1_800
        expired = service.answer(
            data,
            "八月的支出是多少？",
            conversation_id=first["conversation_id"],
        )
        self.assertFalse(expired["context_used"])
        self.assertNotEqual(expired["conversation_id"], first["conversation_id"])

    def test_cloud_flow_sends_only_allowlisted_aggregates(self):
        client = FakeDeepSeekClient([tool_selection(), narration("Model prose may be imperfect.")])
        result = AnalyticsService(client).answer(
            demo_dataset(add_sensitive_canary=True),
            "What is the exact net income this month?",
        )

        self.assertEqual(result["mode"], "deepseek")
        self.assertEqual(result["facts"][0]["result"]["values"]["net_income_minor"], 175102)
        outbound = json.dumps(client.calls, ensure_ascii=False)
        self.assertNotIn(SENSITIVE_CANARY, outbound)
        for forbidden_key in ("memo", "account_key", "account_id", "source_stem"):
            self.assertNotIn(f'"{forbidden_key}":', outbound)
        self.assertEqual(result["usage"], {
            "input_tokens": 220,
            "output_tokens": 50,
            "cached_input_tokens": 0,
        })

    def test_explicit_chinese_month_overrides_model_latest_and_dsml_is_hidden(self):
        client = FakeDeepSeekClient([
            tool_selection(
                name="get_category_breakdown",
                arguments={
                    "period": "latest",
                    "direction": "spending",
                    "limit": 10,
                    "account_scope": "all",
                },
            ),
            narration(
                '<｜｜DSML｜｜ calls><｜｜DSML｜｜ invoke name="get_category_breakdown">'
                '</｜｜DSML｜｜ invoke></｜｜DSML｜｜ calls>'
            ),
        ])
        result = AnalyticsService(client).answer(demo_dataset(), "五月的支出集中在哪些领域？")

        self.assertEqual(result["mode"], "local")
        self.assertNotIn("DSML", result["answer"])
        self.assertEqual(result["facts"][0]["result"]["period"]["month"], "2026-05")
        self.assertIn("2026-05", result["answer"])
        self.assertIn("internal tool markup", result["warning"])

    def test_explicit_month_overrides_wrong_concrete_model_month_and_rewrites_history(self):
        client = FakeDeepSeekClient([
            tool_selection(
                name="get_category_breakdown",
                arguments={
                    "period": "2026-06",
                    "direction": "spending",
                    "limit": 10,
                    "account_scope": "selected",
                },
            ),
            narration("The verified May categories are shown below."),
        ])
        result = AnalyticsService(client).answer(demo_dataset(), "五月的支出集中在哪些领域？")

        self.assertEqual(result["facts"][0]["result"]["period"]["month"], "2026-05")
        self.assertNotIn("tools", client.calls[1])
        self.assertNotIn("tool_choice", client.calls[1])
        rewritten_call = next(
            item for item in client.calls[1]["input_items"]
            if item.get("type") == "function_call"
        )
        self.assertEqual(json.loads(rewritten_call["arguments"]), {
            "period": "2026-05",
            "direction": "spending",
            "limit": 10,
            "account_scope": "all",
        })

    def test_explicit_income_direction_overrides_wrong_model_direction(self):
        client = FakeDeepSeekClient([
            tool_selection(
                name="get_category_breakdown",
                arguments={
                    "period": "2026-05",
                    "direction": "spending",
                    "limit": 5,
                    "account_scope": "all",
                },
            ),
            narration(),
        ])

        result = AnalyticsService(client).answer(
            demo_dataset(),
            "五月的收入分类是什么？",
        )

        self.assertEqual(result["facts"][0]["result"]["direction"], "income")
        rewritten_call = next(
            item for item in client.calls[1]["input_items"]
            if item.get("type") == "function_call"
        )
        self.assertEqual(json.loads(rewritten_call["arguments"])["direction"], "income")

    def test_explicit_spending_metric_overrides_wrong_model_trend_metric(self):
        client = FakeDeepSeekClient([
            tool_selection(
                name="get_monthly_metric_series",
                arguments={
                    "metric": "net_income",
                    "range_name": "last_6_months",
                    "account_scope": "all",
                },
            ),
            narration(),
        ])

        result = AnalyticsService(client).answer(demo_dataset(), "支出趋势如何？")

        self.assertEqual(result["facts"][0]["result"]["metric"], "spending")
        rewritten_call = next(
            item for item in client.calls[1]["input_items"]
            if item.get("type") == "function_call"
        )
        self.assertEqual(json.loads(rewritten_call["arguments"])["metric"], "spending")

    def test_model_protocol_without_structured_call_falls_back_to_local_category_tool(self):
        client = FakeDeepSeekClient([narration(
            '<｜｜DSML｜｜ calls><｜｜DSML｜｜ invoke name="get_category_breakdown">'
            '</｜｜DSML｜｜ invoke></｜｜DSML｜｜ calls>'
        )])
        result = AnalyticsService(client).answer(demo_dataset(), "五月的支出集中在哪些领域？")

        self.assertEqual(result["mode"], "local")
        self.assertEqual(result["facts"][0]["name"], "get_category_breakdown")
        self.assertEqual(result["facts"][0]["result"]["period"]["month"], "2026-05")
        self.assertNotIn("DSML", result["answer"])

    def test_server_scope_overrides_model_scope(self):
        unselected = demo_dataset()
        selected_key = unselected.accounts[0]["key"]
        client = FakeDeepSeekClient([
            tool_selection(arguments={"period": "2026-06", "account_scope": "all"}),
            narration(),
        ])
        result = AnalyticsService(client).answer(
            demo_dataset(selected_account=selected_key),
            "这个月的净收入是多少？",
            account_scope="selected",
        )

        rewritten_call = next(
            item for item in client.calls[1]["input_items"]
            if item.get("type") == "function_call"
        )
        self.assertEqual(json.loads(rewritten_call["arguments"])["account_scope"], "selected")
        self.assertEqual(result["facts"][0]["result"]["values"]["net_income_minor"], 229331)

    def test_incomplete_narration_falls_back_without_exposing_partial_text(self):
        responses = [
            tool_selection(
                name="get_category_breakdown",
                arguments={
                    "period": "2026-05",
                    "direction": "spending",
                    "limit": 5,
                    "account_scope": "all",
                },
            ),
            {
                "status": "incomplete",
                "output": [{
                    "type": "message",
                    "content": [{
                        "type": "output_text",
                        "text": "partial sentinel <tool_call",
                    }],
                }],
            },
        ]

        def transport(payload):
            return responses.pop(0)

        client = DeepSeekResponsesClient(api_key="test-only", transport=transport)
        result = AnalyticsService(client).answer(demo_dataset(), "五月的支出集中在哪些领域？")

        self.assertEqual(result["mode"], "local")
        self.assertNotIn("partial sentinel", result["answer"])
        self.assertEqual(result["facts"][0]["result"]["period"]["month"], "2026-05")
        self.assertIn("warning", result)

    def test_explicit_month_uses_latest_available_matching_year(self):
        self.assertEqual(
            explicit_period_from_question("请分析五月的支出", demo_dataset()),
            "2026-05",
        )

    def test_multiple_explicit_months_are_not_collapsed(self):
        self.assertIsNone(
            explicit_period_from_question("五月和六月分别有多少支出？", demo_dataset())
        )

        selection = {
            "status": "completed",
            "model": "deepseek-flash",
            "output": [
                {
                    "type": "function_call",
                    "call_id": "call-may",
                    "name": "get_period_summary",
                    "arguments": json.dumps({"period": "2026-05", "account_scope": "all"}),
                },
                {
                    "type": "function_call",
                    "call_id": "call-june",
                    "name": "get_period_summary",
                    "arguments": json.dumps({"period": "2026-06", "account_scope": "all"}),
                },
            ],
            "usage": {"input_tokens": 100, "output_tokens": 20},
        }
        client = FakeDeepSeekClient([selection, narration()])
        result = AnalyticsService(client).answer(
            demo_dataset(),
            "五月和六月分别有多少支出？",
        )

        self.assertEqual(
            [fact["result"]["period"]["month"] for fact in result["facts"]],
            ["2026-05", "2026-06"],
        )

    def test_model_missing_an_explicit_month_uses_bounded_local_multi_fallback(self):
        client = FakeDeepSeekClient([
            tool_selection(
                name="get_period_summary",
                arguments={"period": "2026-05", "account_scope": "all"},
            ),
        ])

        result = AnalyticsService(client).answer(
            demo_dataset(),
            "五月和六月分别有多少支出？",
        )

        self.assertEqual(result["mode"], "local")
        self.assertEqual(len(client.calls), 1)
        self.assertEqual(
            [fact["result"]["period"]["month"] for fact in result["facts"]],
            ["2026-05", "2026-06"],
        )

    def test_model_using_a_wrong_explicit_month_uses_local_multi_fallback(self):
        selection = {
            "status": "completed",
            "model": "deepseek-flash",
            "output": [
                {
                    "type": "function_call",
                    "call_id": "call-may",
                    "name": "get_period_summary",
                    "arguments": json.dumps({"period": "2026-05", "account_scope": "all"}),
                },
                {
                    "type": "function_call",
                    "call_id": "call-april",
                    "name": "get_period_summary",
                    "arguments": json.dumps({"period": "2026-04", "account_scope": "all"}),
                },
            ],
        }
        client = FakeDeepSeekClient([selection])

        result = AnalyticsService(client).answer(
            demo_dataset(),
            "五月和六月分别有多少支出？",
        )

        self.assertEqual(result["mode"], "local")
        self.assertEqual(
            [fact["result"]["period"]["month"] for fact in result["facts"]],
            ["2026-05", "2026-06"],
        )

    def test_more_than_four_explicit_months_is_safely_bounded(self):
        client = FakeDeepSeekClient([])
        client.configured = False

        result = AnalyticsService(client).answer(
            demo_dataset(),
            "一月、二月、三月、四月和五月的支出分别是多少？",
        )

        self.assertEqual(result["facts"], [])
        self.assertIn("maximum of 4", result["answer"])

    def test_two_explicit_comparison_months_keep_model_current_period(self):
        client = FakeDeepSeekClient([
            tool_selection(
                name="compare_financial_metric",
                arguments={
                    "metric": "spending",
                    "current_period": "2026-06",
                    "baseline": "previous_month",
                    "alignment": "full_period",
                    "account_scope": "all",
                },
            ),
            narration(),
        ])
        result = AnalyticsService(client).answer(
            demo_dataset(),
            "比较 2026-05 和 2026-06 的支出",
        )

        comparison = result["facts"][0]["result"]
        self.assertEqual(comparison["current"]["period"], "2026-06")
        self.assertEqual(comparison["baseline"]["period"], "2026-05")

    def test_explicit_year_next_to_chinese_text_is_detected(self):
        self.assertEqual(
            explicit_period_from_question("请看2025年8月的支出", demo_dataset()),
            "2025-08",
        )

    def test_protocol_detection_covers_ascii_function_markup(self):
        self.assertTrue(contains_tool_protocol_markup('<function_call name="x">'))

    def test_invalid_model_tool_call_falls_back_to_local_tool(self):
        client = FakeDeepSeekClient([
            tool_selection(
                arguments={"period": "latest", "account_scope": "all", "path": "/tmp/private.csv"}
            )
        ])
        result = AnalyticsService(client).answer(demo_dataset(), "这个月的净收入是多少？")
        self.assertEqual(result["mode"], "local")
        self.assertEqual(result["facts"][0]["result"]["values"]["net_income_minor"], 175102)
        self.assertIn("invalid tool call", result["warning"])

    def test_unconfigured_client_uses_local_comparison(self):
        client = FakeDeepSeekClient([])
        client.configured = False
        result = AnalyticsService(client).answer(demo_dataset(), "这个月净收入相比上个月如何？")
        self.assertEqual(result["mode"], "local")
        self.assertEqual(result["facts"][0]["name"], "compare_financial_metric")
        self.assertIn("£", result["answer"])

    def test_sensitive_question_is_blocked_before_api_call(self):
        client = FakeDeepSeekClient([])
        with self.assertRaises(AnalyticsInputError):
            AnalyticsService(client).answer(
                demo_dataset(),
                "Analyse Date,Account/Card No,Amount,Subcategory,Memo from private.csv",
            )
        self.assertEqual(client.calls, [])

    def test_long_card_number_is_blocked(self):
        with self.assertRaises(AnalyticsInputError):
            validate_question("Please analyse account 1234 5678 9012 3456")


if __name__ == "__main__":
    unittest.main()

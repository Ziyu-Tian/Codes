import unittest
from unittest.mock import Mock

from dashboard.analytics_context import AnalyticsConversationStore
from dashboard.analytics_service import AnalyticsInputError, AnalyticsService, validate_question
from fixtures import demo_dataset


class AnalyticsPrivacyTests(unittest.TestCase):
    def test_bare_split_and_unicode_eight_digit_accounts_are_blocked(self):
        for text in ("account 12345678", "账号 1234 5678", "账号１２３４５６７８",
                     "账号 1234\u200b5678", "account 1234-5678", "account ١٢٣٤٥٦٧٨"):
            with self.subTest(text=text), self.assertRaises(AnalyticsInputError):
                validate_question(text)

    def test_dates_months_and_ordinary_amounts_still_work(self):
        for text in ("2026-10-31 的收入", "Compare 2026-05 and 2026-06",
                     "2026/11/01 的支出", "八月和九月的存款", "支出超过 £1000 的分类"):
            self.assertTrue(validate_question(text))

    def test_known_manual_or_cross_dataset_identifier_is_blocked_before_provider_call(self):
        data = demo_dataset()
        data.private_identifiers = ("12345687654321",)
        client = Mock(configured=True)
        for question in ("看看账号87654321", "账户 8/7/6/5/4/3/2/1 的支出", "123456 87654321 的收入"):
            with self.subTest(question=question), self.assertRaises(AnalyticsInputError):
                AnalyticsService(client).answer(data, question)
        client.create_response.assert_not_called()

    def test_final_outbound_check_catches_identifiers_in_any_input_item(self):
        data = demo_dataset()
        data.private_identifiers = ("12345687654321",)
        client = Mock(configured=True)
        for output in ("private identifier 87654321", "private identifier 8765\n4321"):
            with self.subTest(output=output), self.assertRaises(AnalyticsInputError):
                AnalyticsService(client)._provider_response(data, input_items=[{
                    "type": "function_call_output", "output": output
                }])
        client.create_response.assert_not_called()

    def test_previous_turn_is_rechecked_against_current_private_accounts(self):
        data = demo_dataset()
        data.private_identifiers = ("12345687654321",)
        store = AnalyticsConversationStore()
        scope = store.context_key(data.dataset, "all", None)
        previous_id = store.save(scope, "账号 87654321 的收入", [{
            "name": "get_period_summary", "arguments": {"period": "2026-06", "account_scope": "all"}
        }])
        client = Mock(configured=False)
        result = AnalyticsService(client, store).answer(data, "那上个月呢", conversation_id=previous_id)
        self.assertFalse(result["context_used"])
        self.assertNotEqual(result["conversation_id"], previous_id)

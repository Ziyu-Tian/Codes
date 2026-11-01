import concurrent.futures
import unittest

from dashboard.analytics_context import AnalyticsConversationStore


class FakeClock:
    def __init__(self):
        self.now = 0.0

    def __call__(self):
        return self.now


def verified_call(period="2026-08"):
    return [{
        "name": "get_period_summary",
        "arguments": {"period": period, "account_scope": "all"},
        "result": {
            "period": {"month": period},
            "values": {"income_minor": 100, "spending_minor": 40, "net_income_minor": 60},
        },
    }]


class AnalyticsConversationStoreTests(unittest.TestCase):
    def test_matching_scope_reuses_id_and_replaces_the_previous_turn(self):
        store = AnalyticsConversationStore(secret=b"test-secret")
        key = store.context_key("demo", "all", None)
        conversation_id = store.save(key, "August?", verified_call())
        reused_id = store.save(
            key,
            "July?",
            verified_call("2026-07"),
            conversation_id=conversation_id,
        )

        self.assertEqual(reused_id, conversation_id)
        self.assertEqual(len(store), 1)
        self.assertEqual(store.get(conversation_id, key)["question"], "July?")

    def test_scope_fingerprint_hides_account_and_rejects_cross_scope_access(self):
        store = AnalyticsConversationStore(secret=b"test-secret")
        account = "private-account-key"
        selected_key = store.context_key("demo", "selected", account)
        other_key = store.context_key("demo", "selected", "other-account")
        conversation_id = store.save(selected_key, "Question", verified_call())

        self.assertNotIn(account, selected_key)
        self.assertIsNone(store.get(conversation_id, other_key))
        replacement = store.save(
            other_key,
            "Other scope",
            verified_call(),
            conversation_id=conversation_id,
        )
        self.assertNotEqual(replacement, conversation_id)

    def test_context_expires_after_thirty_minutes(self):
        clock = FakeClock()
        store = AnalyticsConversationStore(ttl_seconds=1_800, clock=clock)
        key = store.context_key("demo", "all", None)
        conversation_id = store.save(key, "Question", verified_call())

        clock.now = 1_799.9
        self.assertIsNotNone(store.get(conversation_id, key))
        clock.now = 1_800
        self.assertIsNone(store.get(conversation_id, key))
        self.assertEqual(len(store), 0)

    def test_store_is_bounded_and_returns_defensive_copies(self):
        store = AnalyticsConversationStore(max_sessions=2)
        key = store.context_key("demo", "all", None)
        first = store.save(key, "First", verified_call("2026-06"))
        second = store.save(key, "Second", verified_call("2026-07"))
        third = store.save(key, "Third", verified_call("2026-08"))

        self.assertEqual(len(store), 2)
        self.assertIsNone(store.get(first, key))
        loaded = store.get(second, key)
        loaded["tool_calls"][0]["arguments"]["period"] = "changed"
        self.assertEqual(
            store.get(second, key)["tool_calls"][0]["arguments"]["period"],
            "2026-07",
        )
        self.assertIsNotNone(store.get(third, key))

    def test_concurrent_saves_respect_capacity(self):
        store = AnalyticsConversationStore(max_sessions=8)
        key = store.context_key("demo", "all", None)

        with concurrent.futures.ThreadPoolExecutor(max_workers=8) as executor:
            ids = list(executor.map(
                lambda index: store.save(key, f"Question {index}", verified_call()),
                range(32),
            ))

        self.assertEqual(len(set(ids)), 32)
        self.assertEqual(len(store), 8)


if __name__ == "__main__":
    unittest.main()

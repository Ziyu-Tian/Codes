import unittest

from dashboard.llm_client import DeepSeekAPIError, DeepSeekResponsesClient


class DeepSeekResponsesClientTests(unittest.TestCase):
    def test_narration_request_omits_tool_fields_when_not_supplied(self):
        captured = []

        def transport(payload):
            captured.append(payload)
            return {"status": "completed", "output": []}

        client = DeepSeekResponsesClient(api_key="test-only", transport=transport)
        client.create_response(instructions="Narrate facts.", input_items=[])

        self.assertNotIn("tools", captured[0])
        self.assertNotIn("tool_choice", captured[0])

    def test_incomplete_response_fails_closed(self):
        client = DeepSeekResponsesClient(
            api_key="test-only",
            transport=lambda payload: {
                "status": "incomplete",
                "output": [{
                    "type": "message",
                    "content": [{"type": "output_text", "text": "<tool_call"}],
                }],
            },
        )

        with self.assertRaises(DeepSeekAPIError):
            client.create_response(instructions="Select a tool.", input_items=[])


if __name__ == "__main__":
    unittest.main()

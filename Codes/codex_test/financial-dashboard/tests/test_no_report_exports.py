import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import Mock, patch
from urllib.parse import urlsplit

import server


class NoReportExportTests(unittest.TestCase):
    def test_read_and_all_financial_writes_only_return_json(self):
        with tempfile.TemporaryDirectory() as directory, patch.dict(os.environ, {"MOBILE_REPORT_EXPORT_DIR": directory}):
            handler = object.__new__(server.Handler)
            handler.headers = {"Content-Type": "application/json"}
            handler.dataset_path = Mock(return_value=Path(directory))
            handler.read_json = Mock(return_value={"dataset": "demo", "category": "Other"})
            handler.send_json = Mock()
            payload = {"transactions": [], "accounts": [], "balances": {}, "file_count": 0}
            with patch.object(server, "build_transactions_payload", return_value=payload), \
                 patch.object(server, "save_balances", return_value={"balances": {}}), \
                 patch.object(server, "save_category_rule_for_transaction", return_value={"category": "Other"}), \
                 patch.object(server, "save_manual_account", return_value=payload), \
                 patch.object(server, "link_manual_transfer", return_value=payload):
                actions = [lambda: handler.handle_transactions(urlsplit("/api/transactions?dataset=demo")),
                           handler.handle_save_balances, handler.handle_save_category_rule,
                           lambda: handler.handle_manual_accounts("/api/manual-accounts"),
                           lambda: handler.handle_manual_accounts("/api/manual-transfers")]
                for action in actions:
                    handler.send_json.reset_mock()
                    action()
                    status, response = handler.send_json.call_args.args
                    self.assertEqual(status, 200)
                    self.assertNotIn("mobile_report_path", response)
                    self.assertNotIn("mobile_report_error", response)
                    self.assertEqual(list(Path(directory).iterdir()), [])

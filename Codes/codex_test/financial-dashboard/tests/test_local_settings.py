import json
import os
from pathlib import Path
import tempfile
import unittest

from dashboard.local_settings import (
    LocalSettingsError,
    delete_deepseek_api_key,
    load_deepseek_api_key,
    save_deepseek_api_key,
)


class LocalSettingsTests(unittest.TestCase):
    def test_key_is_saved_owner_only_and_can_be_deleted(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "llm_settings.json"
            key = "sk-test-not-a-real-secret-123456"
            save_deepseek_api_key(key, path)

            self.assertEqual(load_deepseek_api_key(path), key)
            self.assertEqual(os.stat(path).st_mode & 0o777, 0o600)
            stored = json.loads(path.read_text(encoding="utf-8"))
            self.assertEqual(set(stored), {"deepseek_api_key", "updated_at"})

            delete_deepseek_api_key(path)
            self.assertFalse(path.exists())
            self.assertEqual(load_deepseek_api_key(path), "")

    def test_short_or_whitespace_key_is_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "llm_settings.json"
            with self.assertRaises(LocalSettingsError):
                save_deepseek_api_key("short", path)
            with self.assertRaises(LocalSettingsError):
                save_deepseek_api_key("sk-test key with spaces", path)

    def test_symlink_is_not_followed(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            target = root / "target.json"
            target.write_text("{}", encoding="utf-8")
            link = root / "llm_settings.json"
            link.symlink_to(target)
            with self.assertRaises(LocalSettingsError):
                save_deepseek_api_key("sk-test-not-a-real-secret-123456", link)


if __name__ == "__main__":
    unittest.main()

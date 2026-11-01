import importlib.util
from pathlib import Path
import tempfile
import unittest


spec = importlib.util.spec_from_file_location(
    "remove_legacy_reports", Path(__file__).resolve().parents[1] / "scripts/remove_legacy_reports.py"
)
cleanup = importlib.util.module_from_spec(spec)
spec.loader.exec_module(cleanup)


class LegacyReportRemovalTests(unittest.TestCase):
    def test_only_recognized_regular_reports_are_candidates(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            report = root / "mobile-report-personal-20261101-120000.html"
            report.write_text("<html><title>Personal Financial Dashboard</title></html>")
            (root / "mobile-report-demo-latest.html").write_text("<html>Unrelated document</html>")
            (root / "other.html").write_text("<html>Personal Financial Dashboard</html>")
            (root / "account.csv").write_text("untouched")
            (root / "backup.sqlite3").write_bytes(b"untouched")
            link = root / "mobile-report-demo.html"
            link.symlink_to(report)
            self.assertEqual(cleanup.find_reports([root]), [report])
            self.assertTrue(report.exists())  # inventory is read-only

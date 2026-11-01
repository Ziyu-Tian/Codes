#!/usr/bin/env python3
"""One-time cleanup of recognized legacy reports; never generates a report.

Defaults to dry-run. --delete removes only regular HTML files with both the old
filename format and a dashboard report marker. CSVs, databases and backups are
never candidates. iCloud deletions are synced by macOS in the usual way.
"""
import argparse
import os
from pathlib import Path
import re


REPORT_NAME = re.compile(r"mobile-report-(?:personal|demo)(?:-(?:latest|\d{8}-\d{6}))?\.html\Z")


def find_reports(directories):
    found = []
    for directory in directories:
        directory = Path(directory)
        if not directory.is_dir():
            continue
        for path in directory.glob("mobile-report-*.html"):
            if not REPORT_NAME.fullmatch(path.name) or path.is_symlink() or not path.is_file():
                continue
            with path.open("rb") as stream:
                header = stream.read(131072).decode("utf-8", errors="replace").lower()
            if "<html" in header and ("personal financial dashboard" in header or "financial snapshot" in header):
                if path not in found:
                    found.append(path)
    return found


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--delete", action="store_true")
    args = parser.parse_args()
    directories = [Path(__file__).resolve().parents[1] / "export",
                   Path.home() / "Library/Mobile Documents/com~apple~CloudDocs/Personal Financial Dashboard",
                   Path.home() / "Downloads"]
    if os.getenv("MOBILE_REPORT_EXPORT_DIR"):
        directories.append(Path(os.environ["MOBILE_REPORT_EXPORT_DIR"]).expanduser())
    reports = find_reports(directories)
    if args.delete:
        for path in reports:
            path.unlink()
    print(f"{'Deleted' if args.delete else 'Recognized'} {len(reports)} legacy dashboard reports.")


if __name__ == "__main__":
    main()

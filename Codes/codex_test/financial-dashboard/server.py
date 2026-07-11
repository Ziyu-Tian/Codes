#!/usr/bin/env python3
"""Local HTTP entrypoint for the financial dashboard.

Maintenance note: read agent.md before changing backend module boundaries.
"""

import json
import os
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlsplit

from dashboard.config import DATASETS, PUBLIC
from dashboard.pipeline import (
    build_transactions_payload,
    save_balances,
    save_category_rule_for_transaction,
)
from export_mobile_report import write_mobile_report


def attach_mobile_report(dataset, payload):
    """Best-effort static snapshot export for phone viewing."""
    try:
        output = write_mobile_report(dataset, payload)
        payload["mobile_report_path"] = str(output)
    except OSError as error:
        payload["mobile_report_error"] = str(error)
    return payload


class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(PUBLIC), **kwargs)

    def end_headers(self):
        self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def send_json(self, status, value):
        payload = json.dumps(value).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(payload)))
        self.end_headers()
        self.wfile.write(payload)

    def read_json(self):
        length = min(int(self.headers.get("Content-Length", 0)), 1_000_000)
        return json.loads(self.rfile.read(length) or b"{}")

    def do_GET(self):
        url = urlsplit(self.path)
        if url.path == "/api/transactions":
            return self.handle_transactions(url)
        if url.path.startswith("/api/"):
            return self.send_json(404, {"error": "Not found."})
        return super().do_GET()

    def do_POST(self):
        url = urlsplit(self.path)
        if url.path == "/api/balances":
            return self.handle_save_balances()
        if url.path == "/api/category-rule":
            return self.handle_save_category_rule()
        return self.send_json(404, {"error": "Not found."})

    def dataset_path(self, dataset):
        csv_dir = DATASETS.get(dataset)
        if csv_dir is None:
            raise KeyError("Unknown dataset.")
        return csv_dir

    def handle_transactions(self, url):
        dataset = parse_qs(url.query).get("dataset", ["personal"])[0]
        try:
            csv_dir = self.dataset_path(dataset)
            payload = build_transactions_payload(dataset, csv_dir)
            return self.send_json(200, attach_mobile_report(dataset, payload))
        except KeyError as error:
            return self.send_json(400, {"error": str(error).strip("'")})
        except FileNotFoundError as error:
            return self.send_json(404, {"error": str(error)})
        except (TypeError, ValueError) as error:
            return self.send_json(422, {"error": f"Invalid CSV data: {error}"})

    def handle_save_balances(self):
        try:
            data = self.read_json()
            dataset = data.get("dataset", "personal")
            csv_dir = self.dataset_path(dataset)
            payload = save_balances(dataset, csv_dir, data.get("balances", []))
            snapshot_payload = build_transactions_payload(dataset, csv_dir)
            snapshot_result = attach_mobile_report(dataset, snapshot_payload)
            if "mobile_report_path" in snapshot_result:
                payload["mobile_report_path"] = snapshot_result["mobile_report_path"]
            if "mobile_report_error" in snapshot_result:
                payload["mobile_report_error"] = snapshot_result["mobile_report_error"]
            return self.send_json(200, payload)
        except KeyError as error:
            return self.send_json(400, {"error": str(error).strip("'")})
        except (TypeError, ValueError, json.JSONDecodeError) as error:
            return self.send_json(400, {"error": f"Invalid balance data: {error}"})

    def handle_save_category_rule(self):
        try:
            data = self.read_json()
            dataset = data.get("dataset", "personal")
            csv_dir = self.dataset_path(dataset)
            category = str(data.get("category", "")).strip()
            fingerprint = str(data.get("transaction_fingerprint", "")).strip()
            payload = save_category_rule_for_transaction(
                dataset,
                csv_dir,
                fingerprint,
                category,
                data.get("learn_amount_rule", True),
            )
            snapshot_payload = build_transactions_payload(dataset, csv_dir)
            snapshot_result = attach_mobile_report(dataset, snapshot_payload)
            if "mobile_report_path" in snapshot_result:
                payload["mobile_report_path"] = snapshot_result["mobile_report_path"]
            if "mobile_report_error" in snapshot_result:
                payload["mobile_report_error"] = snapshot_result["mobile_report_error"]
            return self.send_json(200, payload)
        except LookupError as error:
            return self.send_json(404, {"error": str(error)})
        except KeyError as error:
            return self.send_json(400, {"error": str(error).strip("'")})
        except (TypeError, ValueError, json.JSONDecodeError) as error:
            return self.send_json(400, {"error": f"Invalid category data: {error}"})


if __name__ == "__main__":
    port = int(os.getenv("PORT", "3000"))
    print(f"Financial dashboard: http://localhost:{port}")
    ThreadingHTTPServer(("127.0.0.1", port), Handler).serve_forever()

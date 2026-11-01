#!/usr/bin/env python3
"""Local HTTP entrypoint for the financial dashboard.

Maintenance note: read agent.md before changing backend module boundaries.
"""

import json
import os
import stat
import zlib
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlsplit

from dashboard.analytics_metrics import AnalyticsDataError
from dashboard.analytics_service import (
    AnalyticsInputError,
    analytics_status,
    answer_analytics_question,
)
from dashboard.config import (
    DATASETS,
    PET_SPRITESHEET_MAX_BYTES,
    PET_SPRITESHEET_PATH,
    PUBLIC,
)
from dashboard.local_settings import (
    LocalSettingsError,
    delete_deepseek_api_key,
    save_deepseek_api_key,
)
from dashboard.pipeline import (
    build_transactions_payload,
    save_balances,
    save_category_rule_for_transaction,
)
from export_mobile_report import write_mobile_report


PET_SPRITESHEET_DIMENSIONS = (1536, 1872)


def _png_dimensions(content):
    """Return dimensions from a structurally valid PNG IHDR."""
    if len(content) < 33 or content[:8] != b"\x89PNG\r\n\x1a\n":
        return None
    if content[8:12] != b"\x00\x00\x00\r" or content[12:16] != b"IHDR":
        return None
    ihdr = content[16:29]
    expected_crc = int.from_bytes(content[29:33], "big")
    if zlib.crc32(b"IHDR" + ihdr) & 0xFFFFFFFF != expected_crc:
        return None
    width = int.from_bytes(ihdr[:4], "big")
    height = int.from_bytes(ihdr[4:8], "big")
    if width <= 0 or height <= 0:
        return None
    return width, height


def _webp_dimensions(content):
    """Return canvas dimensions from a structurally bounded WebP container."""
    if (
        len(content) < 20
        or content[:4] != b"RIFF"
        or content[8:12] != b"WEBP"
        or int.from_bytes(content[4:8], "little") != len(content) - 8
    ):
        return None

    dimensions = None
    offset = 12
    while offset < len(content):
        if offset + 8 > len(content):
            return None
        chunk_type = content[offset:offset + 4]
        chunk_size = int.from_bytes(content[offset + 4:offset + 8], "little")
        payload_start = offset + 8
        payload_end = payload_start + chunk_size
        padded_end = payload_end + (chunk_size & 1)
        if payload_end > len(content) or padded_end > len(content):
            return None
        payload = content[payload_start:payload_end]

        if dimensions is None and chunk_type == b"VP8X" and len(payload) >= 10:
            width = 1 + int.from_bytes(payload[4:7], "little")
            height = 1 + int.from_bytes(payload[7:10], "little")
            dimensions = width, height
        elif dimensions is None and chunk_type == b"VP8L" and len(payload) >= 5:
            if payload[0] != 0x2F:
                return None
            packed = int.from_bytes(payload[1:5], "little")
            if packed >> 29:
                return None
            dimensions = 1 + (packed & 0x3FFF), 1 + ((packed >> 14) & 0x3FFF)
        elif dimensions is None and chunk_type == b"VP8 " and len(payload) >= 10:
            if payload[3:6] != b"\x9d\x01\x2a":
                return None
            width = int.from_bytes(payload[6:8], "little") & 0x3FFF
            height = int.from_bytes(payload[8:10], "little") & 0x3FFF
            if width <= 0 or height <= 0:
                return None
            dimensions = width, height

        offset = padded_end

    return dimensions if offset == len(content) else None


def load_pet_spritesheet(path=None, max_bytes=PET_SPRITESHEET_MAX_BYTES):
    """Read the one configured pet asset without following symlinks.

    Returning ``None`` for every unavailable/invalid case lets the HTTP layer
    avoid exposing filesystem details in its response.
    """
    asset_path = os.fspath(PET_SPRITESHEET_PATH if path is None else path)
    flags = os.O_RDONLY | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0)
    try:
        descriptor = os.open(asset_path, flags)
    except (OSError, TypeError, ValueError):
        return None

    try:
        file_status = os.fstat(descriptor)
        if (
            not stat.S_ISREG(file_status.st_mode)
            or file_status.st_size <= 0
            or file_status.st_size > max_bytes
        ):
            return None

        chunks = []
        remaining = max_bytes + 1
        while remaining:
            chunk = os.read(descriptor, min(65_536, remaining))
            if not chunk:
                break
            chunks.append(chunk)
            remaining -= len(chunk)
        content = b"".join(chunks)
    except OSError:
        return None
    finally:
        os.close(descriptor)

    if not content or len(content) > max_bytes:
        return None
    if _png_dimensions(content) == PET_SPRITESHEET_DIMENSIONS:
        content_type = "image/png"
    elif _webp_dimensions(content) == PET_SPRITESHEET_DIMENSIONS:
        content_type = "image/webp"
    else:
        return None
    return content, content_type


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
        self.send_header("X-Content-Type-Options", "nosniff")
        self.send_header("X-Frame-Options", "DENY")
        self.send_header("Referrer-Policy", "no-referrer")
        super().end_headers()

    def send_json(self, status, value):
        payload = json.dumps(value).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(payload)))
        self.end_headers()
        self.wfile.write(payload)

    def read_json(self, max_length=1_000_000):
        length = int(self.headers.get("Content-Length", 0))
        if length < 0:
            raise ValueError("Content-Length must not be negative.")
        if length > max_length:
            raise ValueError("Request body is too large.")
        return json.loads(self.rfile.read(length) or b"{}")

    def do_GET(self):
        url = urlsplit(self.path)
        if url.path == "/assets/pet/spritesheet":
            return self.handle_pet_spritesheet()
        if url.path == "/assets/pet" or url.path.startswith("/assets/pet/"):
            return self.send_json(404, {"error": "Pet asset not found."})
        if url.path == "/api/transactions":
            return self.handle_transactions(url)
        if url.path == "/api/analytics/status":
            return self.send_json(200, analytics_status())
        if url.path.startswith("/api/"):
            return self.send_json(404, {"error": "Not found."})
        return super().do_GET()

    def handle_pet_spritesheet(self):
        asset = load_pet_spritesheet()
        if asset is None:
            return self.send_json(404, {"error": "Pet spritesheet is unavailable."})
        content, content_type = asset
        self.send_response(200)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(content)))
        self.send_header("Cross-Origin-Resource-Policy", "same-origin")
        self.end_headers()
        self.wfile.write(content)

    def do_POST(self):
        url = urlsplit(self.path)
        if url.path == "/api/balances":
            return self.handle_save_balances()
        if url.path == "/api/category-rule":
            return self.handle_save_category_rule()
        if url.path == "/api/analytics/chat":
            return self.handle_analytics_chat()
        if url.path == "/api/analytics/settings":
            return self.handle_save_analytics_settings()
        return self.send_json(404, {"error": "Not found."})

    def do_DELETE(self):
        url = urlsplit(self.path)
        if url.path == "/api/analytics/settings":
            return self.handle_delete_analytics_settings()
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

    def handle_analytics_chat(self):
        try:
            content_type = self.headers.get("Content-Type", "").split(";", 1)[0].strip().lower()
            if content_type != "application/json":
                return self.send_json(415, {"error": "Analytics requests must use application/json."})
            data = self.read_json(max_length=16_384)
            dataset = str(data.get("dataset", "personal"))
            csv_dir = self.dataset_path(dataset)
            account_scope = str(data.get("account_scope", "all"))
            selected_account = data.get("selected_account")
            if account_scope == "all":
                selected_account = None
            elif not isinstance(selected_account, str) or not selected_account or len(selected_account) > 128:
                raise AnalyticsInputError("Select a valid dashboard account first.")
            payload = answer_analytics_question(
                dataset=dataset,
                csv_dir=csv_dir,
                question=data.get("question"),
                account_scope=account_scope,
                selected_account=selected_account,
                conversation_id=data.get("conversation_id"),
            )
            return self.send_json(200, payload)
        except KeyError as error:
            return self.send_json(400, {"error": str(error).strip("'")})
        except AnalyticsInputError as error:
            return self.send_json(400, {"error": str(error)})
        except FileNotFoundError as error:
            return self.send_json(404, {"error": str(error)})
        except AnalyticsDataError as error:
            return self.send_json(422, {"error": str(error)})
        except (TypeError, ValueError, json.JSONDecodeError) as error:
            return self.send_json(400, {"error": f"Invalid analytics request: {error}"})

    def handle_save_analytics_settings(self):
        try:
            content_type = self.headers.get("Content-Type", "").split(";", 1)[0].strip().lower()
            if content_type != "application/json":
                return self.send_json(415, {"error": "Analytics settings must use application/json."})
            data = self.read_json(max_length=4_096)
            save_deepseek_api_key(data.get("api_key"))
            return self.send_json(200, analytics_status())
        except (LocalSettingsError, TypeError, ValueError, json.JSONDecodeError) as error:
            return self.send_json(400, {"error": str(error)})

    def handle_delete_analytics_settings(self):
        try:
            delete_deepseek_api_key()
            return self.send_json(200, analytics_status())
        except LocalSettingsError as error:
            return self.send_json(400, {"error": str(error)})


if __name__ == "__main__":
    port = int(os.getenv("PORT", "3000"))
    print(f"Financial dashboard: http://localhost:{port}")
    ThreadingHTTPServer(("127.0.0.1", port), Handler).serve_forever()

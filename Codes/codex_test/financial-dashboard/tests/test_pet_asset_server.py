import io
import os
import struct
import tempfile
import unittest
import zlib
from pathlib import Path
from unittest.mock import Mock, patch

import server


def png_header(width=1536, height=1872):
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    checksum = zlib.crc32(b"IHDR" + ihdr) & 0xFFFFFFFF
    return b"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR" + ihdr + checksum.to_bytes(4, "big")


def webp_lossless_header(width=1536, height=1872):
    dimensions = (width - 1) | ((height - 1) << 14)
    payload = b"\x2f" + dimensions.to_bytes(4, "little")
    chunk = b"VP8L" + len(payload).to_bytes(4, "little") + payload + b"\x00"
    riff_payload = b"WEBP" + chunk
    return b"RIFF" + len(riff_payload).to_bytes(4, "little") + riff_payload


class PetSpritesheetLoadingTests(unittest.TestCase):
    def test_loads_png_and_uses_detected_content_type(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            path = Path(temporary_directory) / "pet.bin"
            payload = png_header()
            path.write_bytes(payload)

            self.assertEqual(server.load_pet_spritesheet(path), (payload, "image/png"))

    def test_loads_webp_and_uses_detected_content_type(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            path = Path(temporary_directory) / "pet.data"
            payload = webp_lossless_header()
            path.write_bytes(payload)

            self.assertEqual(server.load_pet_spritesheet(path), (payload, "image/webp"))

    def test_rejects_missing_directory_unsupported_and_oversized_files(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            unsupported = root / "pet.webp"
            unsupported.write_bytes(b"not-an-image")
            oversized = root / "large.webp"
            oversized.write_bytes(webp_lossless_header())

            self.assertIsNone(server.load_pet_spritesheet(root / "missing.webp"))
            self.assertIsNone(server.load_pet_spritesheet(root))
            self.assertIsNone(server.load_pet_spritesheet(unsupported))
            self.assertIsNone(server.load_pet_spritesheet(oversized, max_bytes=12))

    def test_rejects_truncated_png_and_webp_headers(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            truncated_png = root / "truncated.png"
            truncated_png.write_bytes(png_header()[:28])
            truncated_webp = root / "truncated.webp"
            truncated_webp.write_bytes(webp_lossless_header()[:-3])

            self.assertIsNone(server.load_pet_spritesheet(truncated_png))
            self.assertIsNone(server.load_pet_spritesheet(truncated_webp))

    def test_rejects_images_with_unexpected_dimensions(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            wrong_png = root / "wrong.png"
            wrong_png.write_bytes(png_header(width=768, height=936))
            wrong_webp = root / "wrong.webp"
            wrong_webp.write_bytes(webp_lossless_header(width=768, height=936))

            self.assertIsNone(server.load_pet_spritesheet(wrong_png))
            self.assertIsNone(server.load_pet_spritesheet(wrong_webp))

    @unittest.skipUnless(hasattr(os, "O_NOFOLLOW"), "platform lacks O_NOFOLLOW")
    def test_rejects_symlinks(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            target = root / "target.webp"
            target.write_bytes(webp_lossless_header())
            link = root / "link.webp"
            link.symlink_to(target)

            self.assertIsNone(server.load_pet_spritesheet(link))


class PetSpritesheetRouteTests(unittest.TestCase):
    @staticmethod
    def handler(path):
        handler = object.__new__(server.Handler)
        handler.path = path
        handler.send_json = Mock()
        handler.handle_pet_spritesheet = Mock()
        return handler

    def test_fixed_route_ignores_query_path_parameters(self):
        handler = self.handler("/assets/pet/spritesheet?path=/etc/passwd")

        handler.do_GET()

        handler.handle_pet_spritesheet.assert_called_once_with()
        handler.send_json.assert_not_called()

    def test_pet_subpaths_and_traversal_are_not_served(self):
        for path in (
            "/assets/pet",
            "/assets/pet/spritesheet/private",
            "/assets/pet/../private-file",
        ):
            with self.subTest(path=path):
                handler = self.handler(path)
                handler.do_GET()
                handler.handle_pet_spritesheet.assert_not_called()
                handler.send_json.assert_called_once_with(404, {"error": "Pet asset not found."})

    def test_unavailable_response_does_not_include_configured_path(self):
        handler = object.__new__(server.Handler)
        handler.send_json = Mock()

        with patch.object(server, "load_pet_spritesheet", return_value=None):
            handler.handle_pet_spritesheet()

        handler.send_json.assert_called_once_with(
            404,
            {"error": "Pet spritesheet is unavailable."},
        )
        self.assertNotIn(str(server.PET_SPRITESHEET_PATH), repr(handler.send_json.call_args))

    def test_success_response_sets_image_headers_and_writes_bytes(self):
        handler = object.__new__(server.Handler)
        handler.send_response = Mock()
        handler.send_header = Mock()
        handler.end_headers = Mock()
        handler.wfile = io.BytesIO()

        with patch.object(
            server,
            "load_pet_spritesheet",
            return_value=(b"safe-image", "image/webp"),
        ):
            handler.handle_pet_spritesheet()

        handler.send_response.assert_called_once_with(200)
        self.assertIn(
            unittest.mock.call("Content-Type", "image/webp"),
            handler.send_header.call_args_list,
        )
        self.assertIn(
            unittest.mock.call("Content-Length", "10"),
            handler.send_header.call_args_list,
        )
        self.assertEqual(handler.wfile.getvalue(), b"safe-image")


class RequestBodyParsingTests(unittest.TestCase):
    def test_negative_content_length_is_rejected_without_reading(self):
        handler = object.__new__(server.Handler)
        handler.headers = {"Content-Length": "-1"}
        handler.rfile = Mock()

        with self.assertRaisesRegex(ValueError, "must not be negative"):
            handler.read_json()

        handler.rfile.read.assert_not_called()


class AnalyticsChatRouteTests(unittest.TestCase):
    def test_optional_conversation_id_is_forwarded_to_service(self):
        handler = object.__new__(server.Handler)
        handler.headers = {"Content-Type": "application/json"}
        handler.read_json = Mock(return_value={
            "dataset": "demo",
            "question": "What changed?",
            "account_scope": "all",
            "conversation_id": "conversation-safe-1",
        })
        handler.dataset_path = Mock(return_value=Path("/configured/demo"))
        handler.send_json = Mock()

        with patch.object(server, "answer_analytics_question", return_value={"answer": "ok"}) as answer:
            handler.handle_analytics_chat()

        answer.assert_called_once_with(
            dataset="demo",
            csv_dir=Path("/configured/demo"),
            question="What changed?",
            account_scope="all",
            selected_account=None,
            conversation_id="conversation-safe-1",
        )
        handler.send_json.assert_called_once_with(200, {"answer": "ok"})


if __name__ == "__main__":
    unittest.main()

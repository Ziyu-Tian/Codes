"""Private machine-local settings that must never enter Git or API payloads."""

from datetime import datetime, timezone
import json
import os
from pathlib import Path
import tempfile

from .config import LLM_SETTINGS_PATH


class LocalSettingsError(ValueError):
    """Raised when the local settings file or supplied key is invalid."""


def load_local_settings(path=LLM_SETTINGS_PATH):
    path = Path(path)
    if path.is_symlink():
        raise LocalSettingsError("Refusing to read a symlinked settings file.")
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        return {}
    except (OSError, json.JSONDecodeError) as error:
        raise LocalSettingsError("The local Analytics settings file is unreadable.") from error
    return value if isinstance(value, dict) else {}


def load_deepseek_api_key(path=LLM_SETTINGS_PATH):
    try:
        key = load_local_settings(path).get("deepseek_api_key", "")
    except LocalSettingsError:
        return ""
    return key.strip() if isinstance(key, str) else ""


def validate_api_key(api_key):
    if not isinstance(api_key, str):
        raise LocalSettingsError("API key must be text.")
    api_key = api_key.strip()
    if not 16 <= len(api_key) <= 512:
        raise LocalSettingsError("Enter a valid DeepSeek API key.")
    if any(character.isspace() for character in api_key):
        raise LocalSettingsError("The API key cannot contain whitespace.")
    return api_key


def save_deepseek_api_key(api_key, path=LLM_SETTINGS_PATH):
    """Atomically store a validated key with owner-only permissions."""
    api_key = validate_api_key(api_key)
    path = Path(path)
    if path.is_symlink():
        raise LocalSettingsError("Refusing to replace a symlinked settings file.")
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "deepseek_api_key": api_key,
        "updated_at": datetime.now(timezone.utc).isoformat(timespec="seconds").replace("+00:00", "Z"),
    }
    temporary_path = None
    try:
        descriptor, temporary_name = tempfile.mkstemp(
            prefix=".llm-settings-",
            suffix=".json",
            dir=path.parent,
            text=True,
        )
        temporary_path = Path(temporary_name)
        os.chmod(temporary_path, 0o600)
        with os.fdopen(descriptor, "w", encoding="utf-8") as stream:
            json.dump(payload, stream, ensure_ascii=False, indent=2)
            stream.write("\n")
            stream.flush()
            os.fsync(stream.fileno())
        os.replace(temporary_path, path)
        os.chmod(path, 0o600)
    except OSError as error:
        if temporary_path:
            try:
                temporary_path.unlink(missing_ok=True)
            except OSError:
                pass
        raise LocalSettingsError("Could not save the API key locally.") from error


def delete_deepseek_api_key(path=LLM_SETTINGS_PATH):
    path = Path(path)
    if path.is_symlink():
        raise LocalSettingsError("Refusing to delete a symlinked settings file.")
    try:
        path.unlink()
    except FileNotFoundError:
        pass
    except OSError as error:
        raise LocalSettingsError("Could not remove the local API key.") from error

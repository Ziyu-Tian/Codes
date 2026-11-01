"""Minimal DeepSeek Responses API client using only the Python standard library."""

import json
import os
import time
from urllib.error import HTTPError, URLError
from urllib.request import Request, urlopen

from .local_settings import load_deepseek_api_key


DEFAULT_BASE_URL = "https://api.deepseek.com"
DEFAULT_MODEL = "deepseek-flash"


class DeepSeekAPIError(RuntimeError):
    """A deliberately sanitised provider error safe to show in the local UI."""

    def __init__(self, message, status=None):
        super().__init__(message)
        self.status = status


def _bounded_int(environment_name, default, minimum, maximum):
    try:
        value = int(os.getenv(environment_name, str(default)))
    except ValueError:
        value = default
    return max(minimum, min(value, maximum))


class DeepSeekResponsesClient:
    """Small injectable client for DeepSeek's OpenAI-compatible /responses endpoint."""

    provider = "deepseek"

    def __init__(self, api_key=None, base_url=None, model=None, transport=None):
        environment_key = os.getenv("DEEPSEEK_API_KEY", "").strip()
        if api_key is not None:
            self.api_key = api_key
            self.api_key_source = "injected"
        elif environment_key:
            self.api_key = environment_key
            self.api_key_source = "environment"
        else:
            self.api_key = load_deepseek_api_key()
            self.api_key_source = "local_file" if self.api_key else None
        self.base_url = (base_url or os.getenv("DEEPSEEK_BASE_URL", DEFAULT_BASE_URL)).rstrip("/")
        self.model = model or os.getenv("DEEPSEEK_MODEL", DEFAULT_MODEL)
        self.reasoning_effort = os.getenv("DEEPSEEK_REASONING_EFFORT", "none")
        self.timeout = _bounded_int("DEEPSEEK_TIMEOUT_SECONDS", 20, 5, 60)
        self.max_output_tokens = _bounded_int("DEEPSEEK_MAX_OUTPUT_TOKENS", 800, 200, 2_000)
        self._transport = transport or self._post_json

    @property
    def configured(self):
        return bool(self.api_key)

    def public_status(self):
        return {
            "provider": self.provider,
            "configured": self.configured,
            "model": self.model,
            "api_style": "responses",
            "reasoning_effort": self.reasoning_effort,
            "key_source": self.api_key_source,
            "privacy": "Questions and allowlisted aggregate results only; no raw transactions.",
        }

    def create_response(self, instructions, input_items, tools=None, tool_choice=None):
        if not self.configured:
            raise DeepSeekAPIError("DeepSeek is not configured.")
        payload = {
            "model": self.model,
            "instructions": instructions,
            "input": input_items,
            "reasoning": {"effort": self.reasoning_effort},
            "temperature": 0,
            "max_output_tokens": self.max_output_tokens,
        }
        if tools is not None:
            payload["tools"] = tools
        if tool_choice is not None:
            payload["tool_choice"] = tool_choice
        response = self._transport(payload)
        if not isinstance(response, dict) or not isinstance(response.get("output"), list):
            raise DeepSeekAPIError("DeepSeek returned an invalid response.")
        if response.get("status") != "completed" or response.get("error"):
            raise DeepSeekAPIError("DeepSeek could not complete the request.")
        return response

    def _post_json(self, payload):
        encoded = json.dumps(payload, ensure_ascii=False, separators=(",", ":")).encode("utf-8")
        request = Request(
            f"{self.base_url}/responses",
            data=encoded,
            method="POST",
            headers={
                "Authorization": f"Bearer {self.api_key}",
                "Content-Type": "application/json",
                "Accept": "application/json",
                "User-Agent": "PersonalFinancialDashboard/1.0",
            },
        )
        for attempt in range(2):
            try:
                with urlopen(request, timeout=self.timeout) as response:
                    return json.loads(response.read().decode("utf-8"))
            except HTTPError as error:
                if error.code in {429, 500, 502, 503, 504} and attempt == 0:
                    time.sleep(0.5)
                    continue
                if error.code in {401, 403}:
                    raise DeepSeekAPIError("The DeepSeek API key was rejected.", error.code) from error
                if error.code == 429:
                    raise DeepSeekAPIError("The DeepSeek rate limit or budget was reached.", error.code) from error
                raise DeepSeekAPIError("DeepSeek returned an API error.", error.code) from error
            except (URLError, TimeoutError) as error:
                if attempt == 0:
                    time.sleep(0.5)
                    continue
                raise DeepSeekAPIError("DeepSeek is temporarily unreachable.") from error
            except (UnicodeDecodeError, json.JSONDecodeError) as error:
                raise DeepSeekAPIError("DeepSeek returned unreadable data.") from error
        raise DeepSeekAPIError("DeepSeek is temporarily unreachable.")

"""Short-lived, in-memory context for the Analytics assistant.

The store deliberately keeps only validated questions and already executed
aggregate tool calls.  It never persists data and never stores a raw account
identifier: dataset/account scope is represented by a process-local HMAC.
"""

from collections import OrderedDict
from copy import deepcopy
import hashlib
import hmac
import secrets
import threading
import time


DEFAULT_TTL_SECONDS = 30 * 60
DEFAULT_MAX_SESSIONS = 64
MAX_CONVERSATION_ID_LENGTH = 128


class AnalyticsConversationStore:
    """Thread-safe bounded storage for one verified turn per conversation."""

    def __init__(
        self,
        ttl_seconds=DEFAULT_TTL_SECONDS,
        max_sessions=DEFAULT_MAX_SESSIONS,
        clock=None,
        secret=None,
    ):
        if ttl_seconds <= 0:
            raise ValueError("Conversation TTL must be positive.")
        if max_sessions <= 0:
            raise ValueError("Conversation capacity must be positive.")
        self.ttl_seconds = float(ttl_seconds)
        self.max_sessions = int(max_sessions)
        self._clock = clock or time.monotonic
        self._secret = secret or secrets.token_bytes(32)
        self._entries = OrderedDict()
        self._lock = threading.RLock()

    def context_key(self, dataset, account_scope, selected_account):
        """Return a non-reversible, process-local key for a dashboard scope."""
        parts = (
            str(dataset),
            str(account_scope),
            "" if selected_account is None else str(selected_account),
        )
        encoded = "\x00".join(parts).encode("utf-8")
        return hmac.new(self._secret, encoded, hashlib.sha256).hexdigest()

    def get(self, conversation_id, context_key):
        """Return a defensive copy only when the ID is live and scope-matched."""
        if not self._valid_id(conversation_id):
            return None
        with self._lock:
            self._purge_expired()
            entry = self._entries.get(conversation_id)
            if not entry or not hmac.compare_digest(entry["context_key"], context_key):
                return None
            return deepcopy({
                "question": entry["question"],
                "tool_calls": entry["tool_calls"],
            })

    def save(self, context_key, question, tool_calls, conversation_id=None):
        """Save one verified turn, reusing only an existing scope-matched ID."""
        safe_calls = deepcopy(tool_calls)
        now = self._clock()
        with self._lock:
            self._purge_expired(now)
            existing = self._entries.get(conversation_id) if self._valid_id(conversation_id) else None
            if not existing or not hmac.compare_digest(existing["context_key"], context_key):
                conversation_id = self._new_id()
            else:
                self._entries.pop(conversation_id, None)
            self._entries[conversation_id] = {
                "context_key": context_key,
                "question": question,
                "tool_calls": safe_calls,
                "expires_at": now + self.ttl_seconds,
            }
            while len(self._entries) > self.max_sessions:
                self._entries.popitem(last=False)
            return conversation_id

    def __len__(self):
        with self._lock:
            self._purge_expired()
            return len(self._entries)

    def _purge_expired(self, now=None):
        now = self._clock() if now is None else now
        expired = [
            conversation_id
            for conversation_id, entry in self._entries.items()
            if entry["expires_at"] <= now
        ]
        for conversation_id in expired:
            self._entries.pop(conversation_id, None)

    def _new_id(self):
        conversation_id = secrets.token_urlsafe(24)
        while conversation_id in self._entries:
            conversation_id = secrets.token_urlsafe(24)
        return conversation_id

    @staticmethod
    def _valid_id(conversation_id):
        return (
            isinstance(conversation_id, str)
            and 1 <= len(conversation_id) <= MAX_CONVERSATION_ID_LENGTH
        )


DEFAULT_CONVERSATION_STORE = AnalyticsConversationStore()

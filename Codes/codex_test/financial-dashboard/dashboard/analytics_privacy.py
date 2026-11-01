"""Local-only account checks before any Analytics text reaches a provider."""
import re
import unicodedata


def normalise_text(text):
    text = unicodedata.normalize("NFKC", text)
    return "".join(str(unicodedata.decimal(char)) if char.isdecimal() else char
                   for char in text if unicodedata.category(char) != "Cf")


def account_tokens(values):
    tokens = set()
    for value in values:
        text = normalise_text(str(value or ""))
        if "*" in text or text.startswith(("card:", "manual:", "file:")):
            continue
        if not re.fullmatch(r"[\d\s-]+", text):
            continue
        digits = re.sub(r"\D", "", text)
        if 8 <= len(digits) <= 19:
            tokens.add(digits)
            if len(digits) == 14:
                tokens.add(digits[-8:])
    return tokens


def known_identifiers(data):
    values = list(data.private_identifiers)
    for item in data.transactions:
        values.extend(item.get(key) for key in ("account", "account_id", "counterparty_account_id"))
    values.extend(account.get("key") for account in data.accounts)
    return account_tokens(values)


def contains_known_identifier(text, identifiers):
    text = normalise_text(text)
    for token in identifiers:
        # Boundaries prevent a short identifier matching part of a larger number.
        pattern = r"(?<!\d)" + r"[\W_]*".join(re.escape(d) for d in token) + r"(?!\d)"
        if re.search(pattern, text):
            return True
    return False


def contains_account_number(text, identifiers=()):
    text = normalise_text(text)
    if contains_known_identifier(text, identifiers):
        return True
    # Keep ordinary ISO dates usable, but reject standalone or split 8–19 digit
    # numbers. Known account matches above always take precedence over dates.
    without_dates = re.sub(r"(?<!\d)20\d{2}[-/]\d{2}[-/]\d{2}(?!\d)", "[date]", text)
    return bool(re.search(r"(?<!\d)\d(?:[\s.\-–—_]*\d){7,18}(?!\d)", without_dates))


def load_private_identifiers():
    """Read cached accounts across datasets, including archived manual accounts.

    The pipeline has already initialized these tables. Never return this set in
    dashboard JSON, tool output, context notes, or logs.
    """
    from .database import connect
    with connect() as db:
        values = [row[0] for row in db.execute("SELECT DISTINCT account FROM raw_transactions")]
        values.extend(row[0] for row in db.execute("SELECT identifier FROM manual_accounts"))
    return tuple(account_tokens(values))


def safe_provider_payload(input_items, identifiers):
    if isinstance(input_items, dict):
        return all(safe_provider_payload(value, identifiers)
                   for pair in input_items.items() for value in pair)
    if isinstance(input_items, (list, tuple)):
        return all(safe_provider_payload(value, identifiers) for value in input_items)
    return not contains_known_identifier(str(input_items), identifiers)

import re
from datetime import datetime

TRANSFER_WORDS = ("transfer", "counter credit", "mobile-channel ft", "internal")


def clean_text(value):
    return re.sub(r"\s+", " ", (value or "").strip().lower())


def normalise_account(value):
    digits = re.sub(r"\D", "", value or "")
    return digits if len(digits) >= 8 else ""


def memo_account(memo):
    """Extract a UK-style sort code + account number such as 000000 00000002."""
    match = re.search(r"(?<!\d)(\d{6})\s+(\d{8})(?!\d)", memo or "")
    return "".join(match.groups()) if match else ""


def account_identity(source_stem, bank_account):
    """Identify an account from row data; filenames are only fallback labels."""
    account_id = normalise_account(bank_account)
    visible_digits = re.sub(r"\D", "", bank_account or "")
    is_masked_card = "*" in (bank_account or "") and bool(visible_digits)
    if is_masked_card:
        last_four = visible_digits[-4:]
        return f"card:{last_four}", f"Credit card · {last_four}", "", "credit_card"
    if len(account_id) == 14:
        sort_code = "-".join(account_id[index:index + 2] for index in range(0, 6, 2))
        label = f"Bank account {sort_code} · {account_id[-4:]}"
        account_type = "bank_account"
    elif len(account_id) >= 15:
        label = f"Credit card · {account_id[-4:]}"
        account_type = "credit_card"
    elif account_id:
        label = f"Account · {account_id[-4:]}"
        account_type = "other"
    else:
        label = f"Unknown account · {source_stem.replace('_', ' ').title()}"
        account_type = "unknown"
    key = account_id or f"file:{source_stem.lower()}"
    return key, label, account_id, account_type


def looks_like_card_payment_received(category, memo):
    return clean_text(category) == "payment received" and "payment by direct debit" in clean_text(memo)


def normalise_amount(account_type, category, memo, amount):
    if account_type == "credit_card" and category.lower() == "purchase" and amount > 0:
        return -amount
    if account_type == "credit_card" and looks_like_card_payment_received(category, memo) and amount < 0:
        return abs(amount)
    return amount


def looks_like_transfer(item):
    text = f"{item['category']} {item['memo']}".lower()
    return any(word in text for word in TRANSFER_WORDS)


def looks_like_card_repayment(item):
    return clean_text(item["category"]) == "direct debit" and "b/card rewards" in clean_text(item["memo"])


def card_last_four(item):
    digits = re.sub(r"\D", "", item.get("account", ""))
    return digits[-4:] if digits else ""


def enrich_accounts_and_amounts(raw_items):
    enriched = []
    for raw in raw_items:
        account_key, account_label, account_id, account_type = account_identity(raw["source_stem"], raw["account"])
        enriched.append({
            "id": raw["id"],
            "date": raw["date"],
            "account": raw["account"],
            "account_key": account_key,
            "account_label": account_label,
            "account_id": account_id,
            "account_type": account_type,
            "amount": normalise_amount(account_type, raw["category"], raw["memo"], raw["raw_amount"]),
            "category": raw["category"],
            "memo": raw["memo"],
            "source": raw["source"],
        })
    return enriched


def mark_internal_transfers(items):
    """Recognise owned-account transfers and credit-card repayment pairs."""
    owned_accounts = {item["account_id"]: item["account_label"] for item in items if item["account_id"]}
    owned_cards = {}
    owned_card_labels = {}
    for item in items:
        if item["account_type"] == "credit_card":
            last_four = card_last_four(item)
            if last_four:
                owned_cards[last_four] = item["account_key"]
                owned_card_labels[item["account_key"]] = item["account_label"]

    internal_rows = []
    card_repayment_rows = []
    card_payment_rows = []
    for item in items:
        counterparty_id = memo_account(item["memo"])
        item["counterparty_account_id"] = counterparty_id or None
        if looks_like_card_repayment(item):
            card_match = re.search(r"(?<!\d)(\d{12,19})(?!\d)", item["memo"])
            last_four = card_match.group(1)[-4:] if card_match else ""
            linked_card_key = owned_cards.get(last_four)
            item["transfer_type"] = "internal"
            item["internal_reason"] = "credit_card_repayment"
            item["transfer_pair_id"] = f"card-repayment-{item['id']}"
            item["counterparty_account"] = owned_card_labels.get(linked_card_key, "Credit card repayment")
            item["linked_account_key"] = linked_card_key
            item["balance_affects_linked_account"] = True
            item["excluded_from_totals"] = True
            card_repayment_rows.append(item)
            continue
        if item["account_type"] == "credit_card" and looks_like_card_payment_received(item["category"], item["memo"]):
            item["transfer_type"] = "internal"
            item["internal_reason"] = "credit_card_payment_received"
            item["transfer_pair_id"] = f"card-payment-{item['id']}"
            item["counterparty_account"] = "Bank account repayment"
            item["linked_account_key"] = None
            item["balance_affects_linked_account"] = False
            item["excluded_from_totals"] = True
            card_payment_rows.append(item)
            continue
        is_owned_counterparty = counterparty_id in owned_accounts and counterparty_id != item["account_id"]
        if looks_like_transfer(item) and is_owned_counterparty:
            item["transfer_type"] = "internal"
            item["internal_reason"] = "owned_account_transfer"
            item["counterparty_account"] = owned_accounts[counterparty_id]
            item["linked_account_key"] = None
            item["balance_affects_linked_account"] = False
            item["excluded_from_totals"] = True
            internal_rows.append(item)

    pair_card_repayments(card_repayment_rows, card_payment_rows)
    pair_owned_account_transfers(internal_rows)
    fill_transfer_defaults(items)
    return items


def pair_card_repayments(card_repayment_rows, card_payment_rows):
    used_card_payments = set()
    pair_number = 0
    for bank_item in card_repayment_rows:
        candidates = []
        for card_item in card_payment_rows:
            if card_item["id"] in used_card_payments:
                continue
            same_card = not bank_item.get("linked_account_key") or bank_item["linked_account_key"] == card_item["account_key"]
            if not same_card:
                continue
            if round(abs(bank_item["amount"]) - abs(card_item["amount"]), 2) != 0:
                continue
            days = abs((datetime.fromisoformat(card_item["date"]) - datetime.fromisoformat(bank_item["date"])).days)
            if days <= 3:
                candidates.append((days, card_item["date"], card_item))
        if not candidates:
            continue
        card_item = min(candidates, key=lambda value: (value[0], value[1]))[2]
        used_card_payments.add(card_item["id"])
        pair_number += 1
        pair_id = f"card-repayment-pair-{pair_number}"
        bank_item["transfer_pair_id"] = pair_id
        card_item["transfer_pair_id"] = pair_id
        bank_item["balance_affects_linked_account"] = False
        bank_item["counterparty_account"] = card_item["account_label"]
        card_item["counterparty_account"] = bank_item["account_label"]


def pair_owned_account_transfers(internal_rows):
    used = set()
    pair_number = 0
    for item in internal_rows:
        if item["id"] in used:
            continue
        candidates = []
        for candidate in internal_rows:
            if candidate["id"] == item["id"] or candidate["id"] in used:
                continue
            reciprocal = (
                candidate["account_id"] == item["counterparty_account_id"] and
                candidate["counterparty_account_id"] == item["account_id"]
            )
            if not reciprocal or round(candidate["amount"] + item["amount"], 2) != 0:
                continue
            days = abs((datetime.fromisoformat(candidate["date"]) - datetime.fromisoformat(item["date"])).days)
            if days <= 3:
                candidates.append((days, candidate["date"], candidate))
        if not candidates:
            continue
        candidate = min(candidates, key=lambda value: (value[0], value[1]))[2]
        used.update((item["id"], candidate["id"]))
        pair_number += 1
        pair_id = f"internal-{pair_number}"
        item["transfer_pair_id"] = pair_id
        candidate["transfer_pair_id"] = pair_id


def fill_transfer_defaults(items):
    for item in items:
        if "transfer_type" not in item:
            item["transfer_type"] = "external" if looks_like_transfer(item) else "regular"
            item["internal_reason"] = None
            item["transfer_pair_id"] = None
            item["counterparty_account"] = None
            item["linked_account_key"] = None
            item["balance_affects_linked_account"] = False
            item["excluded_from_totals"] = False
        elif "transfer_pair_id" not in item:
            item["transfer_pair_id"] = f"internal-row-{item['id']}"
            item.setdefault("linked_account_key", None)
            item.setdefault("balance_affects_linked_account", False)
        else:
            item.setdefault("linked_account_key", None)
            item.setdefault("balance_affects_linked_account", False)


def process_transactions(raw_items):
    items = enrich_accounts_and_amounts(raw_items)
    items.sort(key=lambda item: (item["date"], item["id"]), reverse=True)
    return mark_internal_transfers(items)

import hashlib
import re
from datetime import datetime, timezone

from .storage import load_category_store, save_category_store

CATEGORY_OPTIONS = [
    "Income",
    "Groceries",
    "Dining",
    "Transport",
    "Shopping",
    "Bills & utilities",
    "Subscriptions",
    "Housing",
    "Travel",
    "Health",
    "Education",
    "Cash",
    "Internal transfer",
    "Credit card repayment",
    "External transfer",
    "Other",
]

AUTO_CATEGORY_RULES = [
    ("Income", ("salary", "payroll", "wages", "hmrc", "counter credit")),
    ("Groceries", ("tesco", "sainsbury", "sainsburys", "morrisons", "aldi", "lidl", "iceland", "waitrose", "asda", "market")),
    ("Dining", ("restaurant", "cafe", "coffee", "starbucks", "mcdonald", "burger king", "subway", "tortilla", "haidilao", "gdk", "deliveroo", "uber eats", "just eat")),
    ("Transport", ("tfl", "train", "rail", "tube", "bus", "uber", "bolt", "parking", "fuel", "shell", "bp ")),
    ("Shopping", ("amazon", "aliexpress", "taobao", "apple.com/bill", "apple", "argos", "ikea", "boots", "dark sphere")),
    ("Bills & utilities", ("ovo energy", "energy", "utility", "water", "gas", "electric", "broadband", "phone", "mobile", "council tax")),
    ("Subscriptions", ("netflix", "spotify", "prime", "subscription", "patreon", "github", "icloud")),
    ("Housing", ("rent", "letting", "landlord", "mortgage")),
    ("Travel", ("hotel", "airbnb", "booking.com", "easyjet", "ryanair", "ba.com", "airline", "flight")),
    ("Health", ("pharmacy", "chemist", "nhs", "doctor", "dentist", "optician")),
    ("Education", ("university", "tuition", "course", "book", "school")),
    ("Cash", ("atm", "cash withdrawal")),
]


def clean_text(value):
    return re.sub(r"\s+", " ", (value or "").strip().lower())


def merchant_signature(memo):
    text = clean_text(memo)
    text = re.sub(r"\b\d{2,}\b", " ", text)
    text = re.sub(r"[^a-z0-9]+", " ", text)
    words = [word for word in text.split() if len(word) > 1]
    return " ".join(words[:5]) or "unknown"


def transaction_fingerprint(item):
    raw = "|".join([
        item.get("date", ""),
        item.get("account_key", ""),
        f"{item.get('amount', 0):.2f}",
        clean_text(item.get("original_category", item.get("category", ""))),
        clean_text(item.get("memo", "")),
    ])
    return hashlib.sha256(raw.encode("utf-8")).hexdigest()[:20]


def amount_rule_key(item):
    return "|".join([
        item.get("account_type", ""),
        merchant_signature(item.get("memo", "")),
        f"{abs(item.get('amount', 0)):.2f}",
        "in" if item.get("amount", 0) > 0 else "out",
    ])


def builtin_category(item):
    if item.get("transfer_type") == "internal":
        if item.get("internal_reason") in ("credit_card_repayment", "credit_card_payment_received"):
            return "Credit card repayment"
        return "Internal transfer"
    if item.get("transfer_type") == "external":
        return "External transfer"
    text = clean_text(f"{item.get('original_category', item.get('category', ''))} {item.get('memo', '')}")
    for category, keywords in AUTO_CATEGORY_RULES:
        if any(keyword in text for keyword in keywords):
            return category
    if item.get("amount", 0) > 0:
        return "Income"
    return "Other"


def apply_category_rules(items, dataset):
    store = load_category_store().get(dataset, {})
    overrides = store.get("transaction_overrides", {})
    amount_rules = store.get("amount_rules", {})
    for item in items:
        item["original_category"] = item["category"]
        item["transaction_fingerprint"] = transaction_fingerprint(item)
        item["category_signature"] = merchant_signature(item["memo"])
        item["amount_rule_key"] = amount_rule_key(item)
        if item["transaction_fingerprint"] in overrides:
            item["category"] = overrides[item["transaction_fingerprint"]]["category"]
            item["category_source"] = "manual"
        elif item["amount_rule_key"] in amount_rules:
            item["category"] = amount_rules[item["amount_rule_key"]]["category"]
            item["category_source"] = "learned_amount"
        else:
            item["category"] = builtin_category(item)
            item["category_source"] = "auto"
    return items


def category_options_for(dataset):
    store = load_category_store().get(dataset, {})
    custom = store.get("custom_categories", [])
    return sorted(set(CATEGORY_OPTIONS + custom), key=str.lower)


def category_rule_count(dataset):
    store = load_category_store().get(dataset, {})
    return sum(len(store.get(key, {})) for key in ("transaction_overrides", "amount_rules"))


def now_iso():
    return datetime.now(timezone.utc).isoformat(timespec="seconds").replace("+00:00", "Z")


def learn_category_rule(dataset, transaction, category, learn_amount_rule=True):
    store = load_category_store()
    dataset_store = store.setdefault(dataset, {})
    overrides = dataset_store.setdefault("transaction_overrides", {})
    amount_rules = dataset_store.setdefault("amount_rules", {})
    custom_categories = set(dataset_store.setdefault("custom_categories", []))
    custom_categories.add(category)
    dataset_store["custom_categories"] = sorted(custom_categories, key=str.lower)

    timestamp = now_iso()
    overrides[transaction["transaction_fingerprint"]] = {
        "category": category,
        "memo": transaction["memo"],
        "amount": transaction["amount"],
        "account_key": transaction["account_key"],
        "updated_at": timestamp,
    }
    if learn_amount_rule:
        key = transaction["amount_rule_key"]
        existing = amount_rules.get(key, {})
        amount_rules[key] = {
            "category": category,
            "signature": transaction["category_signature"],
            "amount": abs(transaction["amount"]),
            "direction": "in" if transaction["amount"] > 0 else "out",
            "last_memo": transaction["memo"],
            "usage_count": int(existing.get("usage_count", 0)) + 1,
            "updated_at": timestamp,
        }

    save_category_store(store)
    return {
        "category": category,
        "category_options": category_options_for(dataset),
        "learned_amount_rule": learn_amount_rule,
    }

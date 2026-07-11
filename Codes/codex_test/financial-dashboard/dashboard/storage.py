from .database import (
    load_balance_store_from_db,
    load_category_store_from_db,
    save_balance_store_to_db,
    save_category_store_to_db,
)


def load_balance_store():
    return load_balance_store_from_db()


def save_balance_store(store):
    save_balance_store_to_db(store)


def load_category_store():
    return load_category_store_from_db()


def save_category_store(store):
    save_category_store_to_db(store)

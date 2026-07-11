import csv
from datetime import datetime
from pathlib import Path


def import_csv_file(csv_file):
    """Read one CSV file into raw, source-preserving transaction records."""
    csv_file = Path(csv_file)
    rows = []
    with csv_file.open(encoding="utf-8-sig", newline="") as stream:
        for index, row in enumerate(csv.DictReader(stream)):
            if not row.get("Date") or not row.get("Amount"):
                continue
            date = datetime.strptime(row["Date"].strip(), "%d/%m/%Y")
            amount = float(row["Amount"])
            rows.append({
                "id": f"{csv_file.name}-{index}-{row['Date']}-{row['Amount']}",
                "date": date.strftime("%Y-%m-%d"),
                "account": (row.get("Account") or row.get("Account/Card No") or "").strip(),
                "raw_amount": amount,
                "category": row.get("Subcategory", "").strip() or "Uncategorised",
                "memo": " ".join(row.get("Memo", "").split()),
                "source": csv_file.name,
                "source_stem": csv_file.stem,
                "row_index": index,
            })
    return rows


def import_csv_transactions(csv_dir):
    """Read every CSV row into raw, source-preserving transaction records."""
    csv_dir = Path(csv_dir)
    csv_files = sorted(csv_dir.glob("*.csv"))
    if not csv_files:
        raise FileNotFoundError(f"No CSV files found in {csv_dir}")

    rows = []
    for csv_file in csv_files:
        rows.extend(import_csv_file(csv_file))
    return rows

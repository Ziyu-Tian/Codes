"""Allowlisted function tools exposed to the Analytics language model."""

import json

from .analytics_metrics import AnalyticsDataError


ACCOUNT_SCOPE_SCHEMA = {"type": "string", "enum": ["all", "selected"]}
METRIC_SCHEMA = {
    "type": "string",
    "enum": ["income", "spending", "net_income", "deposits"],
}


TOOL_DEFINITIONS = [
    {
        "type": "function",
        "name": "get_period_summary",
        "description": (
            "Return exact locally calculated external income, external spending, and net income "
            "for one calendar month. Use 'latest' for this month/latest imported month."
        ),
        "parameters": {
            "type": "object",
            "properties": {
                "period": {
                    "type": "string",
                    "description": "latest, current_month, previous_month, or YYYY-MM",
                },
                "account_scope": ACCOUNT_SCOPE_SCHEMA,
            },
            "required": ["period", "account_scope"],
            "additionalProperties": False,
        },
    },
    {
        "type": "function",
        "name": "compare_financial_metric",
        "description": (
            "Compare a locally calculated financial metric with the previous month, January, "
            "or the same month last year. For an incomplete latest month prefer matched_days."
        ),
        "parameters": {
            "type": "object",
            "properties": {
                "metric": METRIC_SCHEMA,
                "current_period": {
                    "type": "string",
                    "description": "latest, current_month, previous_month, or YYYY-MM",
                },
                "baseline": {
                    "type": "string",
                    "enum": ["previous_month", "year_start", "previous_year"],
                },
                "alignment": {
                    "type": "string",
                    "enum": ["matched_days", "full_period"],
                },
                "account_scope": ACCOUNT_SCOPE_SCHEMA,
            },
            "required": [
                "metric",
                "current_period",
                "baseline",
                "alignment",
                "account_scope",
            ],
            "additionalProperties": False,
        },
    },
    {
        "type": "function",
        "name": "get_monthly_metric_series",
        "description": (
            "Return a compact monthly series calculated from local data. Use this for trend questions."
        ),
        "parameters": {
            "type": "object",
            "properties": {
                "metric": METRIC_SCHEMA,
                "range_name": {
                    "type": "string",
                    "enum": ["last_6_months", "last_12_months", "year_to_date", "all_available"],
                },
                "account_scope": ACCOUNT_SCOPE_SCHEMA,
            },
            "required": ["metric", "range_name", "account_scope"],
            "additionalProperties": False,
        },
    },
    {
        "type": "function",
        "name": "get_category_breakdown",
        "description": (
            "Return the largest locally aggregated categories for a month. No transaction memo, "
            "merchant, account identifier, or source filename is returned."
        ),
        "parameters": {
            "type": "object",
            "properties": {
                "period": {
                    "type": "string",
                    "description": "latest, current_month, previous_month, or YYYY-MM",
                },
                "direction": {
                    "type": "string",
                    "enum": ["income", "spending", "all"],
                },
                "limit": {"type": "integer", "minimum": 1, "maximum": 10},
                "account_scope": ACCOUNT_SCOPE_SCHEMA,
            },
            "required": ["period", "direction", "limit", "account_scope"],
            "additionalProperties": False,
        },
    },
]


EXPECTED_ARGUMENTS = {
    "get_period_summary": {"period", "account_scope"},
    "compare_financial_metric": {
        "metric",
        "current_period",
        "baseline",
        "alignment",
        "account_scope",
    },
    "get_monthly_metric_series": {"metric", "range_name", "account_scope"},
    "get_category_breakdown": {"period", "direction", "limit", "account_scope"},
}


class AnalyticsToolError(ValueError):
    """Raised when a model requests an unknown or malformed local tool call."""


def parse_tool_arguments(tool_name, raw_arguments):
    if tool_name not in EXPECTED_ARGUMENTS:
        raise AnalyticsToolError("The requested analytics tool is not allowed.")
    if not isinstance(raw_arguments, str) or len(raw_arguments) > 4_000:
        raise AnalyticsToolError("Tool arguments must be a small JSON object.")
    try:
        arguments = json.loads(raw_arguments)
    except json.JSONDecodeError as error:
        raise AnalyticsToolError("The model returned invalid JSON tool arguments.") from error
    if not isinstance(arguments, dict):
        raise AnalyticsToolError("Tool arguments must be a JSON object.")
    expected = EXPECTED_ARGUMENTS[tool_name]
    actual = set(arguments)
    if actual != expected:
        missing = sorted(expected - actual)
        extra = sorted(actual - expected)
        detail = []
        if missing:
            detail.append(f"missing {', '.join(missing)}")
        if extra:
            detail.append(f"unexpected {', '.join(extra)}")
        raise AnalyticsToolError("Invalid tool fields: " + "; ".join(detail))
    return arguments


class AnalyticsToolRunner:
    def __init__(self, analytics_dataset):
        self.data = analytics_dataset

    def execute(self, name, arguments):
        if name == "get_period_summary":
            return self.data.get_period_summary(**arguments)
        if name == "compare_financial_metric":
            return self.data.compare_financial_metric(**arguments)
        if name == "get_monthly_metric_series":
            return self.data.get_monthly_metric_series(**arguments)
        if name == "get_category_breakdown":
            return self.data.get_category_breakdown(**arguments)
        raise AnalyticsToolError("The requested analytics tool is not allowed.")

    def execute_json_call(self, name, raw_arguments):
        try:
            return self.execute(name, parse_tool_arguments(name, raw_arguments))
        except AnalyticsDataError as error:
            raise AnalyticsToolError(str(error)) from error

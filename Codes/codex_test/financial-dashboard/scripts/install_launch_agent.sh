#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LABEL="${PFD_LAUNCH_LABEL:-com.personalfinancialdashboard.app}"
PLIST="$HOME/Library/LaunchAgents/${LABEL}.plist"
PYTHON_BIN="${PYTHON_BIN:-$(command -v python3)}"
PORT="${PORT:-3000}"
LOG_DIR="$ROOT_DIR/logs"
LAUNCHER="$ROOT_DIR/scripts/PersonalFinancialDashboard"
SERVICE_NAME="${PFD_SERVICE_NAME:-PersonalFinancialDashboard}"

mkdir -p "$HOME/Library/LaunchAgents" "$LOG_DIR"

export ROOT_DIR LABEL PLIST PYTHON_BIN PORT LOG_DIR LAUNCHER SERVICE_NAME
"$PYTHON_BIN" - <<'PY'
import os
import plistlib
import shlex
from pathlib import Path

root = os.environ["ROOT_DIR"]
label = os.environ["LABEL"]
plist_path = Path(os.environ["PLIST"])
python_bin = os.environ["PYTHON_BIN"]
port = os.environ["PORT"]
log_dir = os.environ["LOG_DIR"]
launcher = os.environ["LAUNCHER"]
service_name = os.environ["SERVICE_NAME"]

launcher_command = " ".join([
    "exec",
    "-a",
    shlex.quote(service_name),
    "/bin/bash",
    shlex.quote(launcher),
    "--supervise",
])

plist = {
    "Label": label,
    "ProgramArguments": ["/bin/bash", "-lc", launcher_command],
    "WorkingDirectory": root,
    "EnvironmentVariables": {
        "PORT": port,
        "PYTHON_BIN": python_bin,
        "PFD_SERVICE_NAME": service_name,
    },
    "RunAtLoad": True,
    "KeepAlive": {
        "SuccessfulExit": False,
    },
    "StandardOutPath": f"{log_dir}/launchd.out.log",
    "StandardErrorPath": f"{log_dir}/launchd.err.log",
}

plist_path.write_bytes(plistlib.dumps(plist, sort_keys=False))
PY

launchctl bootout "gui/$(id -u)" "$PLIST" >/dev/null 2>&1 || true
launchctl bootstrap "gui/$(id -u)" "$PLIST"
launchctl enable "gui/$(id -u)/$LABEL" >/dev/null 2>&1 || true
launchctl kickstart -k "gui/$(id -u)/$LABEL" >/dev/null 2>&1 || true

echo "Installed $LABEL"
echo "Service name: $SERVICE_NAME"
echo "Plist: $PLIST"
echo "Dashboard: http://localhost:${PORT}"
echo "Logs:"
echo "  $LOG_DIR/launchd.out.log"
echo "  $LOG_DIR/launchd.err.log"

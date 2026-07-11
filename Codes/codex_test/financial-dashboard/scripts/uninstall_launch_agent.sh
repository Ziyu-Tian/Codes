#!/usr/bin/env bash
set -euo pipefail

LABEL="${PFD_LAUNCH_LABEL:-com.personalfinancialdashboard.app}"
LABELS=("$LABEL")

for label in "${LABELS[@]}"; do
  plist="$HOME/Library/LaunchAgents/${label}.plist"
  launchctl bootout "gui/$(id -u)" "$plist" >/dev/null 2>&1 || true
  rm -f "$plist"
done

echo "Uninstalled PersonalFinancialDashboard LaunchAgent"

#!/usr/bin/env bash
# set -euo pipefail

# 色定義とヘルパー関数
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m'

info() { echo -e "[${BLUE}INFO${NC}] $*"; }
success() { echo -e "[${GREEN}SUCCESS${NC}] $*"; }
warn() { echo -e "[${YELLOW}WARN${NC}] $*"; }
failure() { echo -e "[${RED}FAILURE${NC}] $*"; }
error() { echo -e "[${RED}ERROR${NC}] $*"; }

VENV_PYTHON="$HOME/AtCoder/.venv/bin/python"

file_name="$1"
IFS=_ read -r contest part <<<"$file_name"
problem_name="${contest//-/_}_${part}"
problem_url="https://atcoder.jp/contests/${contest}/tasks/${problem_name}"
scraping_script_path="../script/AtCoder_Statement_Scraping.py"

info "Scraping from $problem_url..."
$VENV_PYTHON "$scraping_script_path" "$problem_url" -d "../md/"
scraping_status=$?
if [ "$scraping_status" -ne "0" ]; then
    error "Scraping failed."
    exit 1
fi
success "Scraping completed. md/${problem_name}.md generated."

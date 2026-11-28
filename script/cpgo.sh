#!/usr/bin/env bash
# filepath: c:\Users\ruthe\AtCoder\Python\script\cpgo.sh

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

PYPY_EXEC="$HOME/.pyenv/versions/pypy3.11-7.3.20/bin/pypy3"
VENV_PYTHON="$HOME/AtCoder/.venv/bin/python"
OJ_EXEC="$HOME/AtCoder/.venv/bin/oj"
OJ_TEMPLATE_EXEC="$HOME/AtCoder/.venv/bin/oj-template"

# 引数: 0: file_name
file_name="$1"

naive_solving_script_path="../naive/${file_name}_naive.py"

# 問題名関連
IFS='_' read -r contest_name problem_name_list_1 rest <<<"$file_name"

problem_name="${contest_name//-/_}_$problem_name_list_1"

test_dir="../test/$problem_name"

created_naive_file=false
if [ ! -f "$naive_solving_script_path" ]; then
	touch "$naive_solving_script_path"
	created_naive_file=true
fi

echo "Naive solving script path: $naive_solving_script_path"

if [ "$created_naive_file" = true ]; then
	read -p "Created naive solving File. Do you continue the process? (Y/n) " response
else
	read -p "The naive solving File already exists. Do you continue the process? (Y/n) " response
fi

if [[ ! "$response" == "y" && ! "$response" == "n" ]]; then
	while true; do
		warn "Please enter 'Y' or 'n'."
		read -p "Do you continue the process? (Y/n) " response
		if [[ "$response" == "y" || "$response" == "n" ]]; then
			break
		fi
	done
fi

if [[ "$response" == "n" ]]; then
	info "Existing..."
	exit 0
fi

$OJ_EXEC g/o -c "$PYPY_EXEC $naive_solving_script_path" -d "$test_dir"
echo "$OJ_EXEC g/o -c "$PYPY_EXEC $naive_solving_script_path" -d "$test_dir""

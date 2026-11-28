#!/usr/bin/env bash
# filepath: c:\Users\ruthe\AtCoder\Python\script\cpgi.sh

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

generate_script_path="../script/generate.py"
convert_encoding_script_path="../script/convert_encoding.py"

# 問題名関連
IFS='_' read -r contest_name rest <<<"$file_name"
problem_name_list_1="$rest"

problem_name="${contest_name//-/_}_$problem_name_list_1"
test_dir="../test/$problem_name"
contest_url="https://atcoder.jp/contests/$contest_name"
problem_url="https://atcoder.jp/contests/$contest_name/tasks/$problem_name"

while true; do
	read -p "Do you execute oj-template? (Y/n): " response
	response=$(echo "$response" | tr '[:upper:]' '[:lower:]')
	if [[ "$response" == "y" || "$response" == "n" ]]; then
		break
	fi
	warn "Please enter 'Y' or 'n'."
done

if [[ "$response" == "y" ]]; then
	$OJ_TEMPLATE_EXEC -t generate.py "$problem_url" >$generate_script_path
	oj_template_status=$?
	if [ $oj_template_status -ne 0 ]; then
		error "Failed to execute oj-template: $problem_url"
		warn "Please enter the correct problem URL (or 'q' to quit):"
		read -r correct_url
		if [ "$correct_url" = "q" ]; then
			info "Exiting..."
			exit 1
		fi
		problem_url="$correct_url"
		$OJ_TEMPLATE_EXEC -t generate.py "$problem_url" >$generate_script_path
		if [ $? -ne 0 ]; then
			error "Failed to execute oj-template again. Exiting."
			exit 1
		fi
	fi
	success "Generated $generate_script_path using oj-template."
	# $VENV_PYTHON "$convert_encoding_script_path" "$generate_script_path"
fi

while true; do
	read -p "How many input do you want to generate? " count_input
	if [[ "$count_input" =~ ^[0-9]+$ ]] && [[ "$count_input" -lt 1001 ]]; then
		break
	fi
	warn "Please enter a valid positive integer not greater than 1000."
done

$OJ_EXEC g/i -d $test_dir "$VENV_PYTHON $generate_script_path" $count_input
oj_generate_status=$?
if [ $oj_generate_status -ne 0 ]; then
	error "oj-generate failed. Exiting."
	exit 1
fi
success "Generated $count_input input files in $test_dir."
exit 0

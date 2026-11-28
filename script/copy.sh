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
test_dir="../test/$problem_name"

# テストケースのリストを収集
test_cases=()
for out_file in "$test_dir"/*.out; do
	[[ -e "$out_file" ]] || break
	base="${out_file##*/}"
	base="${base%.out}"
	if [[ -f "$test_dir/$base.in" ]] && [[ -f "$test_dir/$base.out" ]]; then
		test_cases+=("$base")
	fi
done

info "Found ${#test_cases[@]} test cases in $test_dir:"
for case in "${test_cases[@]}"; do
	echo " - $case"
done

# テストケースが存在しない場合
if [[ ${#test_cases[@]} -eq 0 ]]; then
	warn "No test cases found in $test_dir"
	exit 1
fi

# ユーザーに選択を促す
echo
read -p "Which test case do you want to copy? " selected

# selectedが空文字列かつ"sample-1"がtest_casesにあるならselected="sample-1"
if [[ -z "$selected" ]] && [[ " ${test_cases[*]} " =~ " sample-1 " ]]; then
	selected="sample-1"
	info "Selected test case set to sample-1"
elif [[ "$selected" =~ ^[0-9]$ ]]; then
    selected="sample-$selected"
    info "Selected test case set to sample-$selected"
fi
# 選択されたテストケースが有効か確認
if [[ ! " ${test_cases[*]} " =~ " ${selected} " ]]; then
	error "Invalid selection: $selected"
	exit 1
fi

# コピー先ディレクトリを作成
mkdir -p std

# ファイルをコピー
cp "$test_dir/$selected.in" std/input.txt
cp "$test_dir/$selected.out" std/expected.txt

success "Copied $selected to std/input.txt and std/expected.txt"

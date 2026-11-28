#!/usr/bin/env bash
# filepath: c:\Users\ruthe\AtCoder\Python\script\cptest.sh

# set -euo pipefail
ulimit -s 1048576

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
highlight_success() {
    local msg="$*"
    local border="======================================================="
    if command -v lolcrab >/dev/null 2>&1; then
        {
            echo "$border"
            echo "  ${msg}  "
            echo "$border"
        } | lolcrab -g cool -S 0
    else
        # lolcrab が無い場合は従来の色・太字で強調
        echo -e "${GREEN}${border}${NC}"
        echo -e "${BOLD}${GREEN}  ${msg}  ${NC}"
        echo -e "${GREEN}${border}${NC}"
    fi
}

print_runtime_diagnosis() {
    local log_file="$1"
    local shown=false
    local location=""

    location="$(grep -m 1 -F "$file_path:" "$log_file" || true)"

    if grep -q "AddressSanitizer: heap-buffer-overflow" "$log_file"; then
        error "要約: heap-buffer-overflow = vector/配列など、ヒープ上の領域を範囲外アクセスしています。"
        warn "よくある原因: i <= n、a[i+1]、空の vector の back()、end() になった iterator の参照。"
        shown=true
    elif grep -q "AddressSanitizer: stack-buffer-overflow" "$log_file"; then
        error "要約: stack-buffer-overflow = ローカル配列など、スタック上の領域を範囲外アクセスしています。"
        warn "よくある原因: 固定長配列の添字ミス、再帰内の小さい配列への範囲外アクセス。"
        shown=true
    elif grep -q "AddressSanitizer: global-buffer-overflow" "$log_file"; then
        error "要約: global-buffer-overflow = グローバル配列などを範囲外アクセスしています。"
        warn "配列サイズが入力の最大値に足りているか、添字が 0-indexed / 1-indexed でずれていないか確認してください。"
        shown=true
    elif grep -q "AddressSanitizer: stack-overflow" "$log_file"; then
        error "要約: stack-overflow = スタックを使い切っています。"
        warn "よくある原因: 再帰が深すぎる、再帰の終了条件ミス、巨大なローカル配列。"
        shown=true
    elif grep -q "AddressSanitizer: heap-use-after-free" "$log_file"; then
        error "要約: heap-use-after-free = 解放済みのメモリを使っています。"
        warn "よくある原因: vector の再確保後に古い参照/iterator/pointer を使っている。"
        shown=true
    elif grep -q "AddressSanitizer: SEGV" "$log_file"; then
        error "要約: SEGV = 不正なアドレスを読んだ/書いたためクラッシュしています。"
        warn "よくある原因: null pointer、範囲外アクセス、壊れた iterator。"
        shown=true
    elif grep -q "runtime error: signed integer overflow" "$log_file"; then
        error "要約: signed integer overflow = int/long long などの符号付き整数が範囲を超えています。"
        warn "掛け算・足し算の前に long long へ変換する、上限値を見直す、などを確認してください。"
        shown=true
    elif grep -q "runtime error: division by zero" "$log_file"; then
        error "要約: division by zero = 0 で割っています。"
        warn "割る直前の値が 0 にならない条件か確認してください。"
        shown=true
    elif grep -q "runtime error:.*index .* out of bounds" "$log_file"; then
        error "要約: index out of bounds = 配列の添字が有効範囲外です。"
        warn "0-indexed / 1-indexed のずれ、ループ条件、番兵の有無を確認してください。"
        shown=true
    elif grep -q "runtime error:.*reference binding to null pointer" "$log_file"; then
        error "要約: null pointer への参照 = 存在しない要素を参照しています。"
        warn "空の vector/deque/string に front()/back() していないか確認してください。"
        shown=true
    fi

    if grep -q "READ of size 4" "$log_file"; then
        warn "補足: READ of size 4 = int 1個分を不正に読もうとしています。"
        shown=true
    elif grep -q "WRITE of size 4" "$log_file"; then
        warn "補足: WRITE of size 4 = int 1個分を不正に書き込もうとしています。"
        shown=true
    fi

    if [ -n "$location" ]; then
        warn "関係ありそうな場所: $location"
        shown=true
    fi

    if [ "$shown" = true ]; then
        info "上の英語ログは詳細情報です。まずはこの日本語ヒントと該当行の添字・iterator・再帰条件を見てください。"
    fi
}

run_oj_with_diagnosis() {
    local log_file
    local status
    local cmd

    log_file="$(mktemp)"

    if command -v script >/dev/null 2>&1; then
        printf -v cmd "%q " "$@"
        script -q -e -c "$cmd" /dev/null | tee "$log_file"
        status=${PIPESTATUS[0]}
    else
        "$@" 2>&1 | tee "$log_file"
        status=${PIPESTATUS[0]}
    fi

    if [ $status -ne 0 ]; then
        print_runtime_diagnosis "$log_file"
    fi

    rm -f "$log_file"
    return $status
}

PYPY_EXEC="$HOME/.pyenv/versions/pypy3.11-7.3.20/bin/pypy3"
VENV_PYTHON="$HOME/AtCoder/.venv/bin/python"
OJ_EXEC="$HOME/AtCoder/.venv/bin/oj"
AUTOFLAKE_EXEC="$HOME/AtCoder/.venv/bin/autoflake"
BUNDLE_SH="$HOME/AtCoder/Algorithm/script/cpbundle.sh"
CLIP_EXE="/mnt/c/Windows/System32/clip.exe"

# 引数: 0: file_name, 1: file_path, 2: judge_type
file_name="$1"
file_path="$2"
judge_type=$3

if [[ ! "$judge_type" =~ ^[012]$ ]]; then
	error "judge_type must be 0, 1, or 2"
	exit 1
fi

# file_path からディレクトリ部分とファイル名部分を取得
if [[ "$file_path" == *.cpp ]]; then
	file_dir="$(dirname "$file_path")"
	src_base="$(basename "$file_path")"
	src_stem="${src_base%.*}" # 拡張子なし
	out_dir="$file_dir/out"
	exe_path="$out_dir/$src_stem"
	lang_id=6017
elif [[ "$file_path" == *.py ]]; then
	exe_path="$PYPY_EXEC $file_path"
	lang_id=6083
else
	error "Unsupported file extension. Only .cpp and .py are supported."
	exit 1
fi

# 問題名関連
IFS='_' read -r contest_name problem_name_list_1 rest <<<"$file_name"

problem_name="${contest_name//-/_}_$problem_name_list_1"
test_dir="../test/$problem_name"

# AtCoder の場合(デフォルト)
contest_url="https://atcoder.jp/contests/$contest_name"
problem_url="https://atcoder.jp/contests/$contest_name/tasks/$problem_name"

# yukicoder の場合
if [ "$contest_name" = "yukicoder" ]; then
	problem_name="$problem_name_list_1"
	contest_url="https://yukicoder.me/problems/no/$problem_name"
	problem_url="https://yukicoder.me/problems/no/$problem_name"
# AOJ の場合
elif [[ "$contest_name" == "aoj"* ]]; then
	problem_name="$problem_name_list_1"
	contest_url="https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=$problem_name"
	problem_url="https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=$problem_name"
fi

# ジャッジスクリプトのパス設定
if [ "$judge_type" -eq 0 ]; then
	judge_script=""
	template_judge_script=""
elif [ "$judge_type" -eq 1 ]; then
	judge_script="../judge/${problem_name}_judge.py"
	template_judge_script="../judge/template_judge.py"
elif [ "$judge_type" -eq 2 ]; then
	judge_script="../judge/${problem_name}_interactive.py"
	template_judge_script="../judge/template_interactive.py"
fi

# テストケースディレクトリが無ければダウンロード
if [ "$judge_type" -lt 2 ] && [ ! -d "$test_dir" ]; then
	info "Test directory not found. Downloading to: $test_dir"
	$OJ_EXEC dl -d "$test_dir" "$problem_url"
	oj_dl_status=$?
	if [ $oj_dl_status -ne 0 ]; then
		error "Failed to download test cases from: $problem_url"
		warn "Please enter the correct problem URL (or 'q' to quit):"
		read -r correct_url
		if [ "$correct_url" = "q" ]; then
			info "Exiting..."
			exit 1
		fi
		problem_url="$correct_url"
		$OJ_EXEC dl -d "$test_dir" "$problem_url"
		if [ $? -ne 0 ]; then
			error "Failed to download test cases again. Exiting."
			exit 1
		fi
	fi
fi

# ジャッジスクリプトの準備
if [ "$judge_type" -eq 0 ]; then
	:
elif [ -f "$judge_script" ]; then
	info "$judge_script exists."
else
	info "Creating $judge_script from $template_judge_script..."
	cp "$template_judge_script" "$judge_script"
	while true; do
		read -p "Do you continue the processing? (Y/n): " response
		response=$(echo "$response" | tr '[:upper:]' '[:lower:]')
		if [[ "$response" == "y" || "$response" == "n" ]]; then
			break
		fi
	done
	if [[ "$response" == "n" ]]; then
		info "Exiting..."
		exit 1
	fi
fi

# Pythonスクリプトで制限時間取得
time_limit="$($VENV_PYTHON ../script/AtCoder_Timelimit.py "$problem_url")"
info "Time Limit: $time_limit sec"
info "Problem URL: $problem_url"
info "Contest URL: $contest_url"
info "Test Directory: $test_dir"

export LOCAL=true

# online-judge-tools でテスト実行
info "Running tests with oj..."

RETEST_ERROR="1e-9"
tolerance_pass=false

if [ "$judge_type" -eq 0 ]; then
	# まずは厳密一致
	run_oj_with_diagnosis "$OJ_EXEC" t -c "$exe_path" -d "$test_dir" -t "$time_limit" -m exact-match
	oj_status=$?

	# 失敗したら許容誤差つきで再テスト
	if [ $oj_status -ne 0 ]; then
		warn "Exact-match failed. Retesting with tolerance: -e $RETEST_ERROR"
		run_oj_with_diagnosis "$OJ_EXEC" t -c "$exe_path" -d "$test_dir" -t "$time_limit" -e "$RETEST_ERROR"
		oj_status=$?
		if [ $oj_status -eq 0 ]; then
			tolerance_pass=true
		fi
	fi
elif [ "$judge_type" -eq 1 ]; then
	run_oj_with_diagnosis "$OJ_EXEC" t -c "$exe_path" -d "$test_dir" -t "$time_limit" --judge-command "$PYPY_EXEC $judge_script"
	oj_status=$?
else
	run_oj_with_diagnosis "$OJ_EXEC" t/r -c "$exe_path" "$PYPY_EXEC $judge_script"
	oj_status=$?
fi

# テスト結果判定
if [ $oj_status -eq 0 ]; then
	if [ "$tolerance_pass" = true ]; then
		success "All test cases passed with tolerance (e=$RETEST_ERROR)."
		warn "Float tolerance pass: For your reference, please also confirm the output format, including the number of digits and trailing spaces."
	else
		highlight_success "All test cases passed! You can submit the solution."
	fi

	if [[ "$file_path" == *.cpp ]]; then
		set -o pipefail
		$BUNDLE_SH "$file_path"
	elif [[ "$file_path" == *.py ]]; then
		$AUTOFLAKE_EXEC --in-place --remove-all-unused-imports "$file_path"
		result=$?
		if [ $result -eq 0 ]; then
			success "Python code formatted with autoflake."
		else
			error "Failed to format Python code with autoflake."
		fi
		if [ -x "$CLIP_EXE" ]; then
			if command -v iconv >/dev/null 2>&1; then
				iconv -f UTF-8 -t CP932//TRANSLIT "$file_path" | "$CLIP_EXE"
				result=$?
			else
				cat "$file_path" | "$CLIP_EXE"
				result=$?
			fi

			if [ $result -eq 0 ]; then
				success "Python code copied to clipboard."
			else
				error "Failed to copy Python code to clipboard."
			fi
		else
			warn "clip.exe not found. Skipping clipboard copy."
		fi
	fi

	# 自動提出部分はコメントアウト中
	# URL 存在確認
	# if curl -s -o /dev/null -w "%{http_code}" "$problem_url" | grep -q "^200$"; then
	# 	info "Checking contest period..."

	# 	# コンテスト期間チェック
	# 	contest_check_result="$($VENV_PYTHON ../script/AtCoder_Contest_Period.py "$contest_url")"

	# 	if [ "$contest_check_result" = "True" ]; then
	# 		info "Contest is currently active. Submitting the solution..."

	# 		# 自動提出
	# 		$OJ_EXEC submit -l "$lang_id" "$problem_url" "$file_path"
	# 		submit_status=$?

	# 		if [ $submit_status -eq 0 ]; then
	# 			success "Solution submitted successfully!"
	# 		else
	# 			error "Submission failed. Please check your login status or submit manually."
	# 		fi
	# 	else
	# 		warn "Contest is not currently active (ended or not started yet)."
	# 		info "Automatic submission is disabled outside of contest period."
	# 		info "If you want to submit anyway, please use the following command manually:"
	# 		echo -e "${YELLOW}$OJ_EXEC submit -l "$lang_id" \"$problem_url\" \"$file_path\"${NC}"
	# 	fi
	# else
	# 	failure "Problem URL not found. Not submitting."
	# fi

else
	failure "Some tests failed. Fix your sourcecode."
fi

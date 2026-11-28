#!/usr/bin/env bash
# filepath: c:\Users\ruthe\AtCoder\Python\script\cpbundle.sh

set -o pipefail

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

OJ_BUNDLE="$HOME/AtCoder/.venv/bin/oj-bundle"
CLIP_EXE="/mnt/c/Windows/System32/clip.exe"

# バンドル済みのファイルを保存してクリップボードにコピーする。
publish_bundle() (
    local bundle="$1"
    local destination="$2"
    local staged

    if [[ ! -s "$bundle" ]]; then
        error "Bundled output is empty."
        return 1
    fi
    staged="$(mktemp -- "${destination}.XXXXXX")" || return 1
    trap 'rm -f -- "$staged"' EXIT

    if ! cp -- "$bundle" "$staged" || ! mv -fT -- "$staged" "$destination"; then
        error "Failed to save bundled code to $destination."
        return 1
    fi
    success "Bundled C++ code saved to $destination."

    if [[ ! -x "$CLIP_EXE" ]]; then
        warn "clip.exe not found. Skipping clipboard copy."
        return 0
    fi
    if ! iconv -f UTF-8 -t CP932//TRANSLIT "$bundle" | "$CLIP_EXE"; then
        error "Failed to copy bundled C++ code to clipboard."
        return 1
    fi
    success "Bundled C++ code copied to clipboard."
)

if [[ "$1" == --publish ]]; then
    publish_bundle "$2" "$3"
    exit $?
fi

output_file=""
if [[ "$1" == --output ]]; then
    output_file="$2"
    shift 2
fi
file_path="$1"
work_dir="$(mktemp -d)" || exit 1
tmp="$work_dir/submission.cpp"
bundle_src="$work_dir/source.cpp"
trap 'rm -f -- "$tmp" "$bundle_src"; rmdir -- "$work_dir"' EXIT

# 1. 行を削除(d)せず、空行に置換(s/.*//)して行番号をズレないようにする
#    #ifdef ONLINE_JUDGE が無い場合はこの処理を飛ばす（catで素通し）
if grep -qE '^[[:space:]]*#ifdef[[:space:]]+ONLINE_JUDGE[[:space:]]*$' "$file_path"; then
  filter_cmd=(sed
    -e '0,/^#ifdef ONLINE_JUDGE/s/.*//'
    -e '0,/^#endif/{/^#endif/s/.*//}'
  )
else
  filter_cmd=(cat)
fi

"${filter_cmd[@]}" "$file_path" > "$bundle_src" || exit 1

# 2. oj-bundle の出力に含まれる「一時ファイルのパス」を「元のファイルのパス」に書き戻す
#    (sed の区切り文字に | を使うことでパス中の / をエスケープ不要にする)
if ! "$OJ_BUNDLE" -I /home/ruthe/AtCoder/Algorithm/C++/lib -I /usr/local/include "$bundle_src" \
    | sed "s|$bundle_src|$file_path|g" > "$tmp"; then
    error "oj-bundle failed."
    exit 1
fi
if [[ ! -s "$tmp" ]]; then
    error "Bundled output is empty."
    exit 1
fi

if [[ -n "$output_file" ]]; then
    cp -- "$tmp" "$output_file"
else
    publish_bundle "$tmp" "$(dirname -- "$file_path")/submission.cpp"
fi

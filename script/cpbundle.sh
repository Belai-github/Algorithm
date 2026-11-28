#!/usr/bin/env bash
# filepath: c:\Users\ruthe\AtCoder\Python\script\cpbundle.sh

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

OJ_BUNDLE="$HOME/AtCoder/.venv/bin/oj-bundle"
CLIP_EXE="/mnt/c/Windows/System32/clip.exe"

# 引数: 0: file_path
file_path="$1"

# file_path からディレクトリ部分とファイル名部分を取得
file_dir="$(dirname "$file_path")"
src_base="$(basename "$file_path")"

# 一時ファイル作成
tmp="$(mktemp)"
# バンドル用の一時ファイルを作成
bundle_src="$(mktemp).cpp"

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

"${filter_cmd[@]}" "$file_path" > "$bundle_src"

# 2. oj-bundle の出力に含まれる「一時ファイルのパス」を「元のファイルのパス」に書き戻す
#    (sed の区切り文字に | を使うことでパス中の / をエスケープ不要にする)
if "$OJ_BUNDLE" -I /home/ruthe/AtCoder/Algorithm/C++/lib -I /usr/local/include "$bundle_src" \
    | sed "s|$bundle_src|$file_path|g" > "$tmp"; then

    # 保存
    cp "$tmp" "${file_dir}/submission.cpp"
    success "Bundled C++ code saved to ${file_dir}/submission.cpp."

    # 後始末（一時ファイルを削除）
    rm -f "$bundle_src"

    # クリップボードコピー
    if [ ! -s "$tmp" ]; then
        error "Bundled output is empty. Clipboard was not modified."
        rm -f "$tmp"
        exit 1
    fi

    if [ -x "$CLIP_EXE" ]; then
        iconv -f UTF-8 -t CP932//TRANSLIT "$tmp" | "$CLIP_EXE"
        success "Bundled C++ code copied to clipboard."
    else
        warn "clip.exe not found. Skipping clipboard copy."
    fi
else
    error "oj-bundle failed. Clipboard was not modified."
    rm -f "$bundle_src"
    rm -f "$tmp"
    exit 1
fi
rm -f "$tmp"

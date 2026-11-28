#!/bin/bash

# 色定義
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[1m'
NC='\033[0m'

info() { echo -e "[${BLUE}INFO${NC}] $*"; }
success() { echo -e "[${GREEN}SUCCESS${NC}] $*"; }
error() { echo -e "[${RED}ERROR${NC}] $*"; }

# 共通のソースヘッダ
SOURCE_HEADER="/home/ruthe/AtCoder/Algorithm/C++/pch/pch.hpp"
# 自作ライブラリのパス
LIB_PATH="/home/ruthe/AtCoder/Algorithm/C++/lib"

# 1. g++ 用
GPP_DIR="/home/ruthe/AtCoder/Algorithm/C++/pch/g++"
rm -rf "$GPP_DIR"
mkdir -p "$GPP_DIR"

info "Updating header for g++..."
cp "$SOURCE_HEADER" "$GPP_DIR/pch.hpp"

info "Generating PCH for g++..."
g++ -DLOCAL -std=c++20 -g -O2 -Wall -fsanitize=address,undefined \
    -I "$LIB_PATH" \
    -x c++-header "$GPP_DIR/pch.hpp" -o "$GPP_DIR/pch.hpp.gch"

if [ $? -ne 0 ]; then error "Failed g++"; exit 1; fi


# 2. clang++ 用
CLANG_DIR="/home/ruthe/AtCoder/Algorithm/C++/pch/clang++"
rm -rf "$CLANG_DIR"
mkdir -p "$CLANG_DIR"

info "Updating header for clang++..."
cp "$SOURCE_HEADER" "$CLANG_DIR/pch.hpp"

info "Generating PCH for clang++ (via bash -lc)..."

bash -lc "clang++ -DLOCAL -std=c++20 -g -O2 -w -fsanitize=address,undefined \
    -I \"$LIB_PATH\" \
    -x c++-header \"$CLANG_DIR/pch.hpp\" -o \"$CLANG_DIR/pch.hpp.pch\""

if [ $? -ne 0 ]; then error "Failed clang++"; exit 1; fi

success "PCH generation completed!"

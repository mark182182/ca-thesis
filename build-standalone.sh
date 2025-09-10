#!/bin/bash
set -e

source "$(dirname "$0")/common.sh"

cd "tests/standalone"

rm -rf "$output_dir"
mkdir -p "$output_dir"

export CC=clang
cmake -S . -B "$output_dir" \
    -G "MinGW Makefiles" \
    -DCMAKE_BUILD_TYPE="DEBUG" \
    -DCMAKE_C_COMPILER="C:/msys64/mingw64/bin/clang.exe" \
    -DCMAKE_MAKE_PROGRAM="make" \
    -DCOMMON_FLAGS="$COMMON_FLAGS"

cmake --build "$output_dir"
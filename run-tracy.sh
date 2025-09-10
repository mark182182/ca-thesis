#!/bin/bash

set -e

source "$(dirname "$0")/common.sh"

tracy_bin="./profiler/build/Release/tracy-profiler.exe"

if [ ! -f "$tracy_bin" ]; then
    echo "Tracy profiler not found. Build Tracy first."
    exit 1
fi

echo "Running Tracy profiler..."
"$tracy_bin" &
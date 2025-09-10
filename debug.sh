#!/bin/bash

set -e

source "$(dirname "$0")/common.sh"

current_dir="$(pwd)"
/c/msys64/usr/bin/mintty.exe /mingw64/bin/gdb -ex run -ex backtrace "${current_dir}/$output_dir/ca_renderer.exe"
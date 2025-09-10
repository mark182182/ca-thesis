#!/bin/bash

set -e

source "$(dirname "$0")/common.sh"

echo "Creating build directory for Tracy profiler..."
cmake -B profiler/build -S external/tracy-0.12.2/profilerprofiler -DCMAKE_BUILD_TYPE=Release

echo "Building Tracy profiler..."
cmake --build profiler/build --config Release --parallel

echo "Build completed."
#!/bin/bash
set -e

source "$(dirname "$0")/common.sh"

if ! command -v clang &> /dev/null; then
    echo "Error: clang compiler not found"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "Error: make not found. Please install using: choco install make"
    exit 1
fi

verbose_mode=""
if [[ "$*" == *"-v"* ]]; then
    echo "Verbose mode enabled."
    verbose_mode="--verbose"
fi

dump_mode=""
if [[ "$*" == *"--dump"* ]]; then
    echo "AST dump mode enabled."
    dump_mode="--dump"
fi

mkdir -p build

# Need to patch raylib's CMakeLists.txt, otherwise it would try to use the CXX compiler as well, but it does not actually need it.

raylib_cmake_path="external/raylib-${raylib_version}/CMakeLists.txt"
if [ -f "$raylib_cmake_path" ]; then
    echo "Patching raylib CMakeLists.txt..."
    sed -i 's/project(raylib)/project(raylib LANGUAGES C)/' "$raylib_cmake_path"
else
    echo "Warning: raylib CMakeLists.txt not found at $raylib_cmake_path"
    exit 1
fi

# Needs MSYS2 and `pacman -S mingw-w64-x86_64-clang`, otherwise the MSVC linker would be used.

# Target: x86_64-w64-windows-gnu

if [[ "$*" != *"--no-tidy"* ]]; then
    echo "Running clang-tidy..."
    clang-tidy -p build ./src/**/*.c -checks=-*,clang-analyzer-*,concurrency-*,modernize-*,performance-*,readability-*
fi

compat_mode=""
if [[ "$*" == *"--compat"* ]]; then
    echo "Compatibility mode enabled."
    compat_mode="ON"
fi

include_tests=""
if [[ "$*" == *"--tests"* ]]; then
    echo "Including tests in the build."
    include_tests="ON"
fi

tracy_enable=""
if [[ "$*" == *"--tracy"* ]]; then
    if [[ "$build_type" != "DEBUG" ]]; then
        echo "Error: Tracy profiler can only be enabled in DEBUG build type."
        exit 1
    fi
    echo "Tracy profiler enabled."
    tracy_enable="ON"

    # need to copy the standard C++ runtime libraries for Tracy to work
    cp "C:/msys64/mingw64/bin/libgcc_s_seh-1.dll" "$output_dir"
    cp "C:/msys64/mingw64/bin/libwinpthread-1.dll" "$output_dir"
    cp "C:/msys64/mingw64/bin/libstdc++-6.dll" "$output_dir"
fi

export CC="C:/msys64/mingw64/bin/clang.exe"
export CXX="C:/msys64/mingw64/bin/clang++.exe"

echo "Building CMake configuration..."
start_time=$(date +%s%3N)
cmake -S . -B "$output_dir" \
    -G "MinGW Makefiles" \
    -DCMAKE_BUILD_TYPE="$build_type" \
    -DCMAKE_C_COMPILER="$CC" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DCMAKE_MAKE_PROGRAM="make" \
    -DCLANG_VERBOSE="$verbose_mode" \
    -DCLANG_DUMP_AST="$dump_mode" \
    -DCOMPAT_MODE="$compat_mode" \
    -DCOMMON_FLAGS="$COMMON_FLAGS" \
    -DCOMMON_FLAGS_COMPAT="$COMMON_FLAGS_COMPAT" \
    -DRAYLIB_VERSION="$raylib_version" \
    -DTRACY_VERSION="$tracy_version" \
    -DTRACY_ENABLE="$tracy_enable" \
    -DGRAPHICS_COMPAT_LEVEL="$graphics_compat_level" \
    -DGRAPHICS_LEVEL="$graphics_level" \
    -DINCLUDE_TESTS="$include_tests"
echo "CMake configuration completed in $(($(date +%s%3N) - start_time)) ms."


start_time=$(date +%s%3N)
echo "Building the project..."
# build the project
cmake --build "$output_dir" $verbose_mode --parallel

if [ $? -eq 0 ]; then
    echo "Build completed successfully in $(($(date +%s%3N) - start_time)) ms."
else
    echo "Build failed"
    exit 1
fi

if [[ "$include_tests" == "ON" ]]; then
    echo "Running tests..."
    cd "$output_dir"
    ctest --output-on-failure
fi

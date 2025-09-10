#!/bin/bash

if [[ "$*" =~ "--build_type" ]]; then
    build_type=$(echo "$*" | grep -oP '(?<=--build_type\s)\w+')
    echo "Build type specified: $build_type"
else
    echo "No build type specified. Defaulting to 'DEBUG'."
    build_type="DEBUG"
fi

if [[ "$*" == *"--rebuild"* ]]; then
    echo "Rebuilding the project..."
    rm -rf "$output_dir"
else
    echo "Building the project..."
fi

output_dir="build/$build_type"
if [ ! -d "$output_dir" ]; then
    echo "Creating output directory: $output_dir"
    mkdir -p "$output_dir"
else
    echo "Output directory already exists: $output_dir"
fi

tracy_version="0.12.2"
echo "Using Tracy version: $tracy_version"

raylib_version="5.5"
echo "Using raylib version: $raylib_version"

graphics_compat_level="GRAPHICS_API_OPENGL_33"
graphics_level="GRAPHICS_API_OPENGL_43"

COMMON_FLAGS="-Weverything -Wno-missing-prototypes -Wno-strict-prototypes -Wno-declaration-after-statement -mavx2"

COMMON_FLAGS_COMPAT="-Weverything -Wno-missing-prototypes -Wno-strict-prototypes -Wno-declaration-after-statement -march=core-avx-i"
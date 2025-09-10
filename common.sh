#!/bin/bash

tracy_version="0.12.2"
echo "Using Tracy version: $tracy_version"

raylib_version="5.5"
echo "Using raylib version: $raylib_version"

graphics_compat_level="GRAPHICS_API_OPENGL_33"
graphics_level="GRAPHICS_API_OPENGL_43"

COMMON_FLAGS="-Weverything -Wno-missing-prototypes -Wno-strict-prototypes -Wno-declaration-after-statement -mavx2"

COMMON_FLAGS_COMPAT="-Weverything -Wno-missing-prototypes -Wno-strict-prototypes -Wno-declaration-after-statement -march=core-avx-i"
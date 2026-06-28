#!/bin/bash
set -e

BUILD_DIR="cmake-build-debug"

if [ ! -d "$BUILD_DIR" ]; then
    cmake -B "$BUILD_DIR" -S . -DCMAKE_BUILD_TYPE=Debug
fi

cmake --build "$BUILD_DIR"

echo "Build complete. Run with: ./$BUILD_DIR/scriptura"
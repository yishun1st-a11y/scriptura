#!/bin/bash
set -e

BUILD_DIR="build/Desktop-Debug"

if [ ! -d "$BUILD_DIR" ]; then
    cmake -B "$BUILD_DIR" -S .
fi

cmake --build "$BUILD_DIR"

echo "Build complete. Run with: ./build/Desktop-Debug/scriptura"
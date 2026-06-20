#!/bin/bash

BUILD_DIR="build/Desktop-Debug"

if [ ! -f "$BUILD_DIR/scriptura" ]; then
    echo "Building first..."
    ./build.sh
fi

"$BUILD_DIR/scriptura"
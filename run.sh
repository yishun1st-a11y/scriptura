#!/bin/bash

BUILD_DIR="cmake-build-debug"

if [ ! -f "$BUILD_DIR/scriptura" ]; then
    echo "Building first..."
    ./build.sh
fi

"$BUILD_DIR/scriptura"
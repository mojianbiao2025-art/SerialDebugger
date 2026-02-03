#!/bin/bash

# WebAssembly Clean Rebuild Script
# This script completely cleans the build directory and rebuilds from scratch

echo "=========================================="
echo "WebAssembly Clean Rebuild"
echo "=========================================="

# Setup Emscripten environment
source $HOME/emsdk/emsdk_env.sh

# Find Qt installation
QT_ROOT=$(find $PWD -type d -name "wasm_32" 2>/dev/null | head -1)
echo "Found Qt at: $QT_ROOT"

if [ -z "$QT_ROOT" ]; then
    echo "ERROR: Qt installation not found!"
    exit 1
fi

export Qt5_DIR=$QT_ROOT/lib/cmake/Qt5
export PATH=$QT_ROOT/bin:$PATH

# CRITICAL: Remove old build directory completely
echo ""
echo "Cleaning old build directory..."
rm -rf build-wasm
rm -rf CMakeCache.txt
rm -rf CMakeFiles

echo "Creating fresh build directory..."
mkdir build-wasm
cd build-wasm

# Export Qt root for CMake
export QT_ROOT=$QT_ROOT

echo ""
echo "Running CMake configuration..."
emcmake cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$QT_ROOT;$QT_ROOT/lib/cmake" \
    -DQt5_DIR="$QT_ROOT/lib/cmake/Qt5" \
    -DQt5Core_DIR="$QT_ROOT/lib/cmake/Qt5Core" \
    -DQt5Gui_DIR="$QT_ROOT/lib/cmake/Qt5Gui" \
    -DQt5Widgets_DIR="$QT_ROOT/lib/cmake/Qt5Widgets"

if [ $? -ne 0 ]; then
    echo ""
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

echo ""
echo "Building project..."
ninja -v

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "Build successful!"
    echo "=========================================="
    echo ""
    echo "Output files:"
    ls -lh bin/SerialDebugger.*
    echo ""
    echo "To test, run:"
    echo "  cd build-wasm/bin"
    echo "  python3 -m http.server 8000"
    echo "Then open http://localhost:8000/SerialDebugger.html"
else
    echo ""
    echo "=========================================="
    echo "Build failed!"
    echo "=========================================="
    exit 1
fi

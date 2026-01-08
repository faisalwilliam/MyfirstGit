#!/bin/bash

# Script to build EmployeeManagementSystem for WebAssembly (Wasm)

# Ensure we are in the project directory
cd "$(dirname "$0")"

echo "--- Building for WebAssembly ---"

# Create a build directory for Wasm
rm -rf build_wasm
mkdir -p build_wasm
cd build_wasm

# Source Emscripten environment if found locally
if [ -f "../emsdk/emsdk_env.sh" ]; then
    echo "Sourcing local Emscripten environment..."
    source "../emsdk/emsdk_env.sh"
fi

# Run qmake using the Qt for WebAssembly kit
# NOTE: You must have the Emscripten environment sourced before running this.
# Example: source ~/emsdk/emsdk_env.sh
# And ensure 'qmake' points to your Qt Wasm installation, e.g., ~/Qt/5.15.2/wasm_32/bin/qmake

# Attempt to find qmake for Qt 5 WebAssembly specifically in common locations
SEARCH_PATHS="$HOME/Qt /opt/Qt"
QMAKE_WASM=""

# Check specific known paths first (aqtinstall default)
if [ -x "$HOME/Qt/5.15.2/wasm_32/bin/qmake" ]; then
    QMAKE_WASM="$HOME/Qt/5.15.2/wasm_32/bin/qmake"
fi

if [ -z "$QMAKE_WASM" ]; then
for search_path in $SEARCH_PATHS; do
    if [ -d "$search_path" ]; then
        # Try to find specific version first, then any wasm version
        QMAKE_WASM=$(find "$search_path" -name qmake -path "*5.15*wasm*" 2>/dev/null | head -n 1)
        if [ -z "$QMAKE_WASM" ]; then
            QMAKE_WASM=$(find "$search_path" -name qmake -path "*wasm*" 2>/dev/null | head -n 1)
        fi
        if [ -n "$QMAKE_WASM" ]; then break; fi
    fi
done
fi

if [ -z "$QMAKE_WASM" ]; then
    echo "Error: Could not find Qt for WebAssembly qmake. Please edit this script to set QMAKE_WASM path."
    echo "Example: QMAKE_WASM=~/Qt/5.15.13/wasm_32/bin/qmake"
    exit 1
fi

echo "Using qmake: $QMAKE_WASM"
$QMAKE_WASM .. CONFIG+=release
make -j$(nproc)

echo "--- Build Complete ---"
echo "To test, run the following commands:"
echo "  cd build_wasm"
echo "  python3 -m http.server"
echo "Then open http://localhost:8000/EmployeeManagementSystemQt.html in your browser."
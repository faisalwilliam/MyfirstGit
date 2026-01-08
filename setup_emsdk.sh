#!/bin/bash

# Script to install Emscripten SDK compatible with Qt 5.15.2

echo "--- Cloning Emscripten SDK ---"
if [ ! -d "emsdk" ]; then
    git clone https://github.com/emscripten-core/emsdk.git
else
    echo "emsdk directory already exists. Skipping clone."
fi

cd emsdk

echo "--- Installing Emscripten 1.39.8 (Required for Qt 5.15) ---"
# Qt 5.15.2 is known to work best with 1.39.8
./emsdk install 1.39.8

echo "--- Activating Emscripten ---"
./emsdk activate 1.39.8

echo "--- Done ---"
echo "Emscripten installed in ./emsdk"
echo "You can now run ./build_wasm.sh"
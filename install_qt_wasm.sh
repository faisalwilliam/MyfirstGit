#!/bin/bash

# Script to install Qt 5.15.2 WebAssembly via command line (No Login Required)

echo "--- Installing aqtinstall ---"
# Ensure pip is installed: sudo apt install python3-pip
pip3 install aqtinstall

echo "--- Installing Qt 5.15.2 WebAssembly ---"
mkdir -p ~/Qt
cd ~/Qt

# Install Qt 5.15.2 with WebAssembly (wasm_32) architecture
python3 -m aqt install-qt linux desktop 5.15.2 wasm_32

echo "--- Done ---"
echo "Qt installed in ~/Qt/5.15.2/wasm_32"
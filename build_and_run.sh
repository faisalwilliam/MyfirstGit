#!/bin/bash

# Set the project directory
PROJECT_DIR="/home/amir/MyfirstGit"

# Navigate to the directory
if [ -d "$PROJECT_DIR" ]; then
    cd "$PROJECT_DIR"
else
    echo "Error: Project directory $PROJECT_DIR not found."
    exit 1
fi

echo "--- 1. Checking Assets ---"
# Ensure icon.png exists
if [ ! -f "icon.png" ]; then
    echo "Downloading icon.png..."
    wget -q -O icon.png https://upload.wikimedia.org/wikipedia/commons/thumb/8/81/Qt_logo_2016.svg/512px-Qt_logo_2016.svg.png || echo "Warning: Failed to download icon.png"
fi

# Ensure splash.png exists
if [ ! -f "splash.png" ]; then
    echo "Downloading splash.png..."
    wget -q -O splash.png https://upload.wikimedia.org/wikipedia/commons/thumb/6/66/Computer_programming_-_Unsplash.jpg/640px-Computer_programming_-_Unsplash.jpg
fi

echo "--- 2. Cleaning and Building ---"
# Clean previous build artifacts
if [ -f "Makefile" ]; then
    make clean > /dev/null
fi

# Run qmake
/usr/lib/qt5/bin/qmake CONFIG+=release

# Run make (using all available cores)
make -j$(nproc)

# Check if the executable was created
if [ -f "EmployeeManagementSystemQt" ]; then
    echo "--- 3. Running Application ---"
    ./EmployeeManagementSystemQt
else
    echo "Error: Build failed! Executable not found."
    exit 1
fi
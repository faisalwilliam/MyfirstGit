#!/bin/bash

# Script to download and launch the Qt Online Installer

echo "--- Finding latest Qt Installer ---"
# Scrape the download page to find the current filename
INSTALLER_FILE=$(wget -qO- https://download.qt.io/official_releases/online_installers/ | grep -o 'qt-unified-linux-x64-[0-9.]*-online.run' | head -n 1)

if [ -z "$INSTALLER_FILE" ]; then
    echo "Error: Could not determine installer filename."
    exit 1
fi

echo "Downloading $INSTALLER_FILE..."
wget "https://download.qt.io/official_releases/online_installers/$INSTALLER_FILE"

echo "Making executable..."
chmod +x "$INSTALLER_FILE"

echo "Launching Installer..."
./"$INSTALLER_FILE"
#!/bin/bash

# Ensure we are in the project directory
cd "$(dirname "$0")"

# Define Version
# Extract version from .pro file
VERSION=$(grep "VERSION" EmployeeManagementSystem.pro | cut -d '=' -f 2 | tr -d ' ')

echo "--- 1. Building Release Version ---"
# Clean up previous builds to ensure a fresh release build
if [ -f "Makefile" ]; then
    make clean > /dev/null
fi

# Run qmake and make
/usr/lib/qt5/bin/qmake CONFIG+=release
make -j$(nproc)

if [ ! -f "EmployeeManagementSystemQt" ]; then
    echo "Error: Build failed. Executable not found."
    exit 1
fi

echo "--- 2. Preparing Deployment Folder ---"
rm -rf deploy
mkdir -p deploy

# Copy executable and assets
cp EmployeeManagementSystemQt deploy/

# Create a fresh desktop file to ensure correct formatting
cat > deploy/EmployeeManagementSystem.desktop <<EOF
[Desktop Entry]
Type=Application
Name=Employee Management System
Comment=Calculate weekly wages and tax
Exec=EmployeeManagementSystemQt
Icon=icon
Categories=Office;
Terminal=false
EOF

cp icon.png deploy/
cp splash.png deploy/

# Resize icon to standard size if convert is available to avoid issues with linuxdeployqt
if command -v convert >/dev/null 2>&1; then
    convert deploy/icon.png -resize 256x256 deploy/icon.png
fi

# Note: We do NOT copy config.ini so the customer gets a fresh configuration.

echo "--- Verification Step ---"
echo "Contents of deploy folder:"
ls -l deploy/
read -p "Press Enter to continue packaging, or Ctrl+C to abort..."

echo "--- Creating Zip Archive for Distribution ---"
# Create a zip file of the deploy folder for easy distribution
zip -r EmployeeManagementSystem_Deploy.zip deploy/
echo "Created archive: EmployeeManagementSystem_Deploy.zip"

echo "--- 3. Setting up Packaging Tool ---"
# Expected SHA256 checksum for linuxdeployqt-continuous-x86_64.AppImage (Update this if the tool updates)
# Note: Since it's a continuous release, the checksum changes frequently. 
# Ideally, you should pin to a specific release version instead of continuous.
# For this example, we will just check if the file is a valid executable after download.

if [ ! -f "linuxdeployqt-continuous-x86_64.AppImage" ]; then
    echo "Downloading linuxdeployqt..."
    wget -q https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
    chmod +x linuxdeployqt-continuous-x86_64.AppImage
fi

# Basic validation: Check if it's a valid ELF executable
if ! file linuxdeployqt-continuous-x86_64.AppImage | grep -q "ELF"; then
    echo "Error: Downloaded linuxdeployqt is not a valid executable. Deleting..."
    rm linuxdeployqt-continuous-x86_64.AppImage
    exit 1
fi

echo "--- 4. Generating AppImage ---"
# Ensure correct qmake is found
export PATH=/usr/lib/qt5/bin:$PATH
export VERSION

# Unset QT variables that might interfere with linuxdeployqt
unset QT_PLUGIN_PATH
unset LD_LIBRARY_PATH

# Debug: Check binary dependencies
echo "--- Checking Binary Dependencies ---"
ldd deploy/EmployeeManagementSystemQt

# Debug: Validate Desktop File
if command -v desktop-file-validate >/dev/null 2>&1; then
    echo "--- Validating Desktop File ---"
    desktop-file-validate deploy/EmployeeManagementSystem.desktop
fi

# Run linuxdeployqt
./linuxdeployqt-continuous-x86_64.AppImage deploy/EmployeeManagementSystem.desktop -appimage -unsupported-allow-new-glibc -verbose=3 -no-translations

if [ $? -ne 0 ]; then
    echo "Error: linuxdeployqt failed to generate the AppImage."
    echo "Trying to run without -appimage to debug AppDir creation..."
    ./linuxdeployqt-continuous-x86_64.AppImage deploy/EmployeeManagementSystem.desktop -unsupported-allow-new-glibc -verbose=3 -no-translations
    exit 1 # Still exit with error, but after trying to show more info
fi

echo "--- Packaging Complete ---"
echo "You can send this file to your customers:"
ls -lh Employee_Management_System*.AppImage

# Identify the generated file
APPIMAGE_FILE=$(find . -maxdepth 1 -name "Employee_Management_System*.AppImage" -print -quit)

if [ -f "$APPIMAGE_FILE" ]; then
    echo "--- Generating Checksum ---"
    sha256sum "$APPIMAGE_FILE" > "$APPIMAGE_FILE.sha256"
    echo "Checksum saved to: $APPIMAGE_FILE.sha256"
fi

echo
echo "--- 5. Uploading to Server ---"
read -p "Do you want to upload the AppImage to the server? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    # --- Upload Settings (CHANGE THESE) ---
    UPLOAD_USER="user"
    UPLOAD_HOST="your-server.com"
    UPLOAD_PATH="/var/www/html/downloads/"

    if [ -f "$APPIMAGE_FILE" ]; then
        echo "Uploading $APPIMAGE_FILE to $UPLOAD_HOST..."
        rsync -avz --progress "$APPIMAGE_FILE" "$UPLOAD_USER@$UPLOAD_HOST:$UPLOAD_PATH"
        
        if [ -f "$APPIMAGE_FILE.sha256" ]; then
            echo "Uploading checksum..."
            rsync -avz --progress "$APPIMAGE_FILE.sha256" "$UPLOAD_USER@$UPLOAD_HOST:$UPLOAD_PATH"
        fi
    else
        echo "Error: AppImage file not found for upload."
    fi

    if [ -f "EmployeeManagementSystem_Deploy.zip" ]; then
        echo "Uploading EmployeeManagementSystem_Deploy.zip..."
        rsync -avz --progress "EmployeeManagementSystem_Deploy.zip" "$UPLOAD_USER@$UPLOAD_HOST:$UPLOAD_PATH"
    fi

    echo "Upload complete."
fi
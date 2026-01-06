#!/bin/bash

# Script to export the current project folder to a custom server

# --- Configuration (CHANGE THESE) ---


UPLOAD_USER="" # Leave empty for local copy
UPLOAD_HOST="" # Leave empty for local copy
UPLOAD_PATH="/home/postgres/backups/MyfirstGit/"
# ------------------------------------

echo "Exporting project folder to $UPLOAD_HOST..."

# Use rsync to upload the current directory
# We exclude build artifacts and git history to keep the upload clean and fast

DESTINATION="$UPLOAD_PATH"
if [ -n "$UPLOAD_HOST" ]; then
    DESTINATION="$UPLOAD_USER@$UPLOAD_HOST:$UPLOAD_PATH"
fi

mkdir -p "$UPLOAD_PATH" 2>/dev/null # Attempt to create local dir if it doesn't exist

rsync -avz --progress \
    --exclude '.git' \
    --exclude '*.o' \
    --exclude 'moc_*' \
    --exclude 'EmployeeManagementSystemQt' \
    . "$DESTINATION"

echo "Export complete."
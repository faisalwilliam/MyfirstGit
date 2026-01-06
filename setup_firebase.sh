#!/bin/bash

echo "--- Firebase Configuration Setup ---"

# 1. Prompt for credentials
read -p "Enter your Firebase Web API Key: " API_KEY
read -p "Enter your Firebase Project ID: " PROJECT_ID

# Remove potential whitespace
API_KEY=$(echo "$API_KEY" | tr -d ' ')
PROJECT_ID=$(echo "$PROJECT_ID" | tr -d ' ')

if [ -z "$API_KEY" ] || [ -z "$PROJECT_ID" ]; then
    echo "Error: Values cannot be empty."
    exit 1
fi

echo "Updating source files..."

# 2. Replace placeholders using sed
# We use s|old|new|g delimiter to safely handle special characters

sed -i "s|YOUR_WEB_API_KEY|$API_KEY|g" LoginDialog.cpp RegisterDialog.cpp ResetPasswordDialog.cpp
sed -i "s|YOUR_PROJECT_ID|$PROJECT_ID|g" MainWindow.cpp MyReportsDialog.cpp

echo "Success! Credentials updated."
echo "--------------------------------"
echo "Now run: ./build_and_run.sh"
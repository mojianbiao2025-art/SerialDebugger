#!/bin/bash
# Clone specific commit from main branch
# Commit: 40f92b824712fe353961bcd499ca1190072beaa2

echo "========================================"
echo "Clone Specific Commit"
echo "========================================"
echo ""
echo "Commit: 40f92b824712fe353961bcd499ca1190072beaa2"
echo "Branch: main"
echo ""

# Set repository URL (modify if needed)
REPO_URL="https://github.com/YOUR_USERNAME/SerialDebugger.git"
COMMIT_HASH="40f92b824712fe353961bcd499ca1190072beaa2"
TARGET_DIR="SerialDebugger-40f92b8"

# Ask for repository URL
echo "Please enter the repository URL:"
echo "(Press Enter to use: $REPO_URL)"
read -p "Repository URL: " USER_REPO_URL

if [ ! -z "$USER_REPO_URL" ]; then
    REPO_URL="$USER_REPO_URL"
fi

echo ""
echo "Repository: $REPO_URL"
echo "Target directory: $TARGET_DIR"
echo ""

# Check if directory already exists
if [ -d "$TARGET_DIR" ]; then
    echo "ERROR: Directory $TARGET_DIR already exists!"
    echo "Please delete it or choose a different name."
    exit 1
fi

# Clone repository
echo "Step 1: Cloning repository..."
git clone "$REPO_URL" "$TARGET_DIR"
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to clone repository!"
    exit 1
fi

# Checkout specific commit
echo ""
echo "Step 2: Checking out specific commit..."
cd "$TARGET_DIR"
git checkout "$COMMIT_HASH"
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to checkout commit!"
    cd ..
    exit 1
fi

echo ""
echo "========================================"
echo "Success!"
echo "========================================"
echo ""
echo "Repository cloned to: $TARGET_DIR"
echo "Current commit: $COMMIT_HASH"
echo ""
echo "To view commit details:"
echo "  cd $TARGET_DIR"
echo "  git show"
echo ""
echo "To create a new branch from this commit:"
echo "  git checkout -b my-branch-name"
echo ""

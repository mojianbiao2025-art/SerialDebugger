@echo off
chcp 65001 >nul
REM Clone specific commit from main branch
REM Commit: 40f92b824712fe353961bcd499ca1190072beaa2

echo ========================================
echo Clone Specific Commit
echo ========================================
echo.
echo Commit: 40f92b824712fe353961bcd499ca1190072beaa2
echo Branch: main
echo.

REM Set repository URL (modify if needed)
set REPO_URL=https://github.com/YOUR_USERNAME/SerialDebugger.git
set COMMIT_HASH=40f92b824712fe353961bcd499ca1190072beaa2
set TARGET_DIR=SerialDebugger-40f92b8
set MAX_RETRIES=3

echo Please enter the repository URL:
echo (Press Enter to use: %REPO_URL%)
set /p USER_REPO_URL=Repository URL: 

if not "%USER_REPO_URL%"=="" set REPO_URL=%USER_REPO_URL%

echo.
echo Repository: %REPO_URL%
echo Target directory: %TARGET_DIR%
echo.

REM Check if directory already exists
if exist "%TARGET_DIR%" (
    echo WARNING: Directory %TARGET_DIR% already exists!
    echo.
    choice /C YN /M "Do you want to delete it and continue"
    if errorlevel 2 (
        echo Operation cancelled.
        pause
        exit /b 1
    )
    echo Deleting existing directory...
    rmdir /s /q "%TARGET_DIR%"
)

REM Configure git for better network handling
echo Configuring git for better network stability...
git config --global http.postBuffer 524288000
git config --global http.lowSpeedLimit 0
git config --global http.lowSpeedTime 999999

echo.
echo Step 1: Cloning repository (with retry)...
set RETRY_COUNT=0

:CLONE_RETRY
set /a RETRY_COUNT+=1
echo.
echo Attempt %RETRY_COUNT% of %MAX_RETRIES%...

git clone --progress %REPO_URL% %TARGET_DIR%

if errorlevel 1 (
    if %RETRY_COUNT% LSS %MAX_RETRIES% (
        echo Clone failed, retrying in 5 seconds...
        timeout /t 5 /nobreak >nul
        if exist "%TARGET_DIR%" rmdir /s /q "%TARGET_DIR%"
        goto CLONE_RETRY
    ) else (
        echo.
        echo ========================================
        echo ERROR: Failed to clone repository after %MAX_RETRIES% attempts!
        echo ========================================
        echo.
        echo Possible solutions:
        echo 1. Check your internet connection
        echo 2. Try using a VPN or proxy
        echo 3. Use shallow clone: git clone --depth 1 %REPO_URL%
        echo 4. Download as ZIP from GitHub and extract
        echo.
        pause
        exit /b 1
    )
)

echo.
echo Clone successful!
echo.
echo Step 2: Checking out specific commit...
cd %TARGET_DIR%

REM First, fetch all commits to ensure we have the target commit
echo Fetching all commits...
git fetch --all

git checkout %COMMIT_HASH%
if errorlevel 1 (
    echo.
    echo ERROR: Failed to checkout commit!
    echo.
    echo Possible reasons:
    echo 1. Commit hash is incorrect
    echo 2. Commit is in a different branch
    echo.
    echo Searching for commit in all branches...
    git log --all --oneline | findstr /C:"%COMMIT_HASH:~0,7%"
    echo.
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo Success!
echo ========================================
echo.
echo Repository cloned to: %TARGET_DIR%
echo Current commit: %COMMIT_HASH%
echo.
echo Commit details:
git log -1 --oneline
echo.
echo To view full commit details:
echo   cd %TARGET_DIR%
echo   git show
echo.
echo To create a new branch from this commit:
echo   git checkout -b my-branch-name
echo.
echo Note: You are in 'detached HEAD' state.
echo To make changes, create a new branch first.
echo.

pause

@echo off
chcp 65001 >nul
REM Stable clone with shallow clone option for better network reliability
REM Commit: 40f92b824712fe353961bcd499ca1190072beaa2

echo ========================================
echo Stable Clone - Specific Commit
echo ========================================
echo.

set REPO_URL=https://github.com/mojianbiao2025-art/SerialDebugger
set COMMIT_HASH=40f92b824712fe353961bcd499ca1190072beaa2
set TARGET_DIR=SerialDebugger-40f92b8

echo Repository: %REPO_URL%
echo Commit: %COMMIT_HASH%
echo Target: %TARGET_DIR%
echo.

REM Clean up if exists
if exist "%TARGET_DIR%" (
    echo Cleaning existing directory...
    rmdir /s /q "%TARGET_DIR%"
)

REM Configure git for better stability
echo Configuring git...
git config --global http.postBuffer 524288000
git config --global core.compression 0
git config --global http.lowSpeedLimit 0
git config --global http.lowSpeedTime 999999

echo.
echo Choose clone method:
echo 1. Full clone (recommended, but slower)
echo 2. Shallow clone (faster, less history)
echo 3. Partial clone (balanced)
echo.
choice /C 123 /N /M "Select option (1-3): "
set CLONE_METHOD=%ERRORLEVEL%

if "%CLONE_METHOD%"=="1" (
    echo.
    echo Method 1: Full clone...
    git clone --progress %REPO_URL% %TARGET_DIR%
) else if "%CLONE_METHOD%"=="2" (
    echo.
    echo Method 2: Shallow clone...
    git clone --depth 1 --no-single-branch %REPO_URL% %TARGET_DIR%
    cd %TARGET_DIR%
    git fetch --depth 1 origin %COMMIT_HASH%
    cd ..
) else (
    echo.
    echo Method 3: Partial clone...
    git clone --filter=blob:none --progress %REPO_URL% %TARGET_DIR%
)

if errorlevel 1 (
    echo.
    echo ========================================
    echo Clone failed!
    echo ========================================
    echo.
    echo Alternative methods:
    echo.
    echo 1. Download ZIP from GitHub:
    echo    https://github.com/mojianbiao2025-art/SerialDebugger/archive/%COMMIT_HASH%.zip
    echo.
    echo 2. Use Git with proxy:
    echo    git config --global http.proxy http://127.0.0.1:7890
    echo    git clone %REPO_URL%
    echo.
    echo 3. Use SSH instead of HTTPS:
    echo    git clone git@github.com:mojianbiao2025-art/SerialDebugger.git
    echo.
    pause
    exit /b 1
)

echo.
echo Checking out commit...
cd %TARGET_DIR%
git fetch origin %COMMIT_HASH% 2>nul
git checkout %COMMIT_HASH%

if errorlevel 1 (
    echo Commit not found, fetching all branches...
    git fetch --all
    git checkout %COMMIT_HASH%
)

if errorlevel 1 (
    echo.
    echo ERROR: Cannot checkout commit %COMMIT_HASH%
    echo.
    echo Available recent commits:
    git log --oneline -10
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo Success!
echo ========================================
echo.
git log -1
echo.
echo Repository: %CD%
echo.

pause

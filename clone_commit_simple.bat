@echo off
REM Simple clone specific commit
REM Usage: Modify REPO_URL below, then run this script

REM ===== CONFIGURATION =====
REM Replace with your actual repository URL
set REPO_URL=https://github.com/YOUR_USERNAME/SerialDebugger.git
set COMMIT_HASH=40f92b824712fe353961bcd499ca1190072beaa2
set TARGET_DIR=SerialDebugger-40f92b8
REM =========================

echo Cloning %REPO_URL%...
git clone %REPO_URL% %TARGET_DIR%

echo Checking out commit %COMMIT_HASH%...
cd %TARGET_DIR%
git checkout %COMMIT_HASH%

echo.
echo Done! Repository is in: %TARGET_DIR%
pause

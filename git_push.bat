@echo off
echo ====================================
echo Git Push Helper Script
echo ====================================
echo.

REM 设置你的 GitHub 仓库地址
set REPO_URL=https://github.com/mojianbiao2025-art/SerialDebugger.git

REM 设置你的 GitHub 凭据（仅本地使用，不要分享此文件！）
set GIT_USERNAME=mojianbiao2025-art
set GIT_TOKEN=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

REM 检查是否已初始化 Git
if not exist ".git" (
    echo [Info] Initializing Git repository...
    git init
    echo.
)

REM 添加所有文件
echo [Info] Adding files...
git add .
echo.

REM 提交
set /p commit_msg="Enter commit message (or press Enter for default): "
if "%commit_msg%"=="" set commit_msg=Update: Serial Port Debugger

echo [Info] Committing changes...
git commit -m "%commit_msg%"
echo.

REM 检查是否已添加远程仓库
git remote -v | findstr origin >nul
if errorlevel 1 (
    echo [Info] Adding remote repository...
    echo Repository: %REPO_URL%
    git remote add origin %REPO_URL%
    echo.
) else (
    echo [Info] Remote repository already exists.
    echo Repository: %REPO_URL%
    echo.
)

REM 推送到 GitHub（使用保存的凭据）
echo [Info] Pushing to GitHub...
git branch -M main

REM 使用 token 进行认证推送
git push https://%GIT_USERNAME%:%GIT_TOKEN%@github.com/mojianbiao2025-art/SerialDebugger.git main

if errorlevel 1 (
    echo.
    echo [Error] Push failed!
    echo.
    echo Possible reasons:
    echo 1. Token may be invalid or expired
    echo 2. Network connection issue
    echo 3. Repository permissions issue
    echo.
    echo To update token:
    echo 1. Go to: https://github.com/settings/tokens
    echo 2. Generate new token
    echo 3. Edit this file and update GIT_TOKEN variable
    echo.
    pause
    exit /b 1
)

echo.
echo ====================================
echo [Success] Code pushed to GitHub!
echo ====================================
echo.
echo Next steps:
echo 1. Go to: https://github.com/mojianbiao2025-art/SerialDebugger
echo 2. Click on "Actions" tab
echo 3. Wait for the build to complete (10-15 minutes)
echo 4. Download the APK from "Artifacts"
echo.
echo Repository: %REPO_URL%
echo Actions: https://github.com/mojianbiao2025-art/SerialDebugger/actions
echo.
pause

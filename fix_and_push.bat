@echo off
chcp 65001 >nul
echo ====================================
echo 修复并推送到 GitHub
echo Fix and Push to GitHub
echo ====================================
echo.

echo [步骤 1/4] 从 Git 中移除 git_push.bat...
echo [Step 1/4] Removing git_push.bat from Git...
git rm --cached git_push.bat 2>nul
echo.

echo [步骤 2/4] 添加更新的文件...
echo [Step 2/4] Adding updated files...
git add .gitignore git_push_safe.bat
echo.

echo [步骤 3/4] 提交更改...
echo [Step 3/4] Committing changes...
git commit -m "Security: Remove token from repository, add safe push script"
echo.

echo [步骤 4/4] 推送到 GitHub...
echo [Step 4/4] Pushing to GitHub...
echo.
echo 请输入你的凭据 / Please enter your credentials:
echo 用户名 / Username: mojianbiao2025-art
echo 密码 / Password: [粘贴你的 Personal Access Token]
echo.

git push -u origin main

if errorlevel 1 (
    echo.
    echo [错误] 推送失败！
    echo [Error] Push failed!
    echo.
    echo 如果仍然提示 token 问题，需要：
    echo If still showing token issue, you need to:
    echo.
    echo 1. 访问 / Visit:
    echo    https://github.com/mojianbiao2025-art/SerialDebugger/security/secret-scanning/unblock-secret/38vyV41NmsYWJ5Lhp9x6ZJJ5YzL
    echo.
    echo 2. 点击 "Allow secret" 允许这个 secret
    echo    Click "Allow secret" to allow this secret
    echo.
    echo 3. 或者撤销旧 token 并创建新的
    echo    Or revoke old token and create new one
    echo    https://github.com/settings/tokens
    echo.
    pause
    exit /b 1
)

echo.
echo ====================================
echo [成功] 代码已推送到 GitHub！
echo [Success] Code pushed to GitHub!
echo ====================================
echo.
echo 下次推送使用 / Use for next push:
echo   git_push_safe.bat
echo.
echo 查看构建 / View build:
echo   https://github.com/mojianbiao2025-art/SerialDebugger/actions
echo.
pause

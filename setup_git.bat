@echo off
chcp 65001 >nul
echo ====================================
echo Git 配置脚本
echo Git Configuration Script
echo ====================================
echo.

echo [Info] 配置 Git 用户信息...
echo [Info] Configuring Git user information...
echo.

git config --global user.name "mojianbiao2025-art"
git config --global user.email "mojianbiao2025@gmail.com"

echo ✓ 用户名 / Username: mojianbiao2025-art
echo ✓ 邮箱 / Email: mojianbiao2025@gmail.com
echo.

echo [Info] 验证配置...
echo [Info] Verifying configuration...
echo.

git config --global user.name
git config --global user.email
echo.

echo ====================================
echo [Success] Git 配置完成！
echo [Success] Git configuration complete!
echo ====================================
echo.

echo 下一步 / Next steps:
echo 1. 创建 Personal Access Token
echo    Create Personal Access Token
echo    https://github.com/settings/tokens
echo.
echo 2. 运行 git_push.bat 推送代码
echo    Run git_push.bat to push code
echo.

pause

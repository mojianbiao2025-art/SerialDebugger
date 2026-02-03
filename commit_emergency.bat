@echo off
chcp 65001 >nul
echo ========================================
echo 紧急修复 - 简化加载逻辑
echo ========================================
echo.

echo 正在添加文件...
git add index-simple.html
git add diagnose.html
git add commit_diagnose.bat
git add .github/workflows/build-all.yml

echo.
git status

echo.
echo 修复内容：
echo 1. index-simple.html - 简化的加载页面
echo    - 更清晰的错误提示
echo    - 超时检测（30秒）
echo    - 详细的加载日志
echo.
echo 2. diagnose.html - 诊断工具
echo    - 检查文件大小
echo    - 检查浏览器支持
echo    - 快速操作按钮
echo.

set /p confirm="确认提交紧急修复吗？(Y/N): "
if /i "%confirm%" NEQ "Y" (
    echo 取消提交。
    pause
    exit /b
)

echo.
echo 正在提交...
git commit -m "Emergency fix: Add simplified loading page and diagnostic tool

Changes:
1. index-simple.html
   - Simplified loading logic
   - Better error messages
   - 30-second timeout detection
   - Detailed loading logs

2. diagnose.html
   - File size checker
   - Browser support checker
   - Quick action buttons

3. Deploy both pages in GitHub Actions

Usage:
- Main: https://mojianbiao2025-art.github.io/SerialDebugger/index-simple.html
- Diagnose: https://mojianbiao2025-art.github.io/SerialDebugger/diagnose.html

This should help identify why the application is stuck at loading."

if %errorlevel% neq 0 (
    echo 提交失败！
    pause
    exit /b 1
)

echo.
echo 正在推送...
git push

if %errorlevel% neq 0 (
    echo 推送失败！
    pause
    exit /b 1
)

echo.
echo ========================================
echo ✓ 紧急修复已部署！
echo ========================================
echo.
echo 等待 1-2 分钟后访问：
echo.
echo 1. 简化页面（推荐）：
echo    https://mojianbiao2025-art.github.io/SerialDebugger/index-simple.html
echo.
echo 2. 诊断工具：
echo    https://mojianbiao2025-art.github.io/SerialDebugger/diagnose.html
echo.
echo 3. 测试页面：
echo    https://mojianbiao2025-art.github.io/SerialDebugger/test-minimal.html
echo.
echo 简化页面会显示：
echo - 详细的加载进度
echo - 清晰的错误信息
echo - 30秒超时提示
echo.
echo 如果仍然卡住，请：
echo 1. 按 Ctrl+Shift+R 强制刷新
echo 2. 清除浏览器缓存
echo 3. 查看诊断工具的文件大小
echo.
pause

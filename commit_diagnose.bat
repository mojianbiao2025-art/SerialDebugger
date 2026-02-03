@echo off
chcp 65001 >nul
echo ========================================
echo 添加诊断工具 - 快速提交
echo ========================================
echo.

echo 正在添加文件...
git add diagnose.html
git add .github/workflows/build-all.yml

echo.
git status

echo.
set /p confirm="确认提交诊断工具吗？(Y/N): "
if /i "%confirm%" NEQ "Y" (
    echo 取消提交。
    pause
    exit /b
)

echo.
echo 正在提交...
git commit -m "Add diagnostic tool for WebAssembly debugging

- Add diagnose.html: comprehensive diagnostic page
- Check file sizes and availability
- Check browser support
- Check network status
- Quick actions (clear cache, navigate to test pages)

Usage:
https://mojianbiao2025-art.github.io/SerialDebugger/diagnose.html"

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
echo ✓ 诊断工具已部署！
echo ========================================
echo.
echo 等待 1-2 分钟后访问：
echo https://mojianbiao2025-art.github.io/SerialDebugger/diagnose.html
echo.
echo 这个工具可以帮助诊断：
echo - 文件是否正确部署
echo - 文件大小是否正确
echo - 浏览器是否支持
echo - 网络连接是否正常
echo.
pause

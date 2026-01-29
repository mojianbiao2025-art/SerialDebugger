@echo off
chcp 65001 >nul
echo ====================================
echo 串口调试助手 - 清理脚本
echo ====================================
echo.

if not exist build (
    echo [信息] build 目录不存在，无需清理
    pause
    exit /b 0
)

echo [警告] 即将删除 build 目录及其所有内容
choice /C YN /M "确认删除"

if errorlevel 2 (
    echo [取消] 已取消清理操作
    pause
    exit /b 0
)

if errorlevel 1 (
    echo [清理] 正在删除 build 目录...
    rmdir /s /q build
    echo [完成] 清理完成！
)

echo.
pause

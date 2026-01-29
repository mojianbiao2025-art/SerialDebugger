@echo off
chcp 65001 >nul
echo ====================================
echo 串口调试助手 - 重新编译脚本
echo ====================================
echo.

echo [清理] 删除旧的编译文件...
if exist build (
    rmdir /s /q build
    echo [完成] 清理完成
) else (
    echo [信息] 无需清理
)

echo.
echo [编译] 开始重新编译...
echo.

call build.bat

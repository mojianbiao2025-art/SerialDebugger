@echo off
chcp 65001 >nul
echo ====================================
echo 串口调试助手 - 编译脚本
echo ====================================
echo.

REM 检查是否存在 build 目录
if exist build (
    echo [信息] 检测到 build 目录已存在
    choice /C YN /M "是否清理旧的编译文件"
    if errorlevel 2 goto :skip_clean
    if errorlevel 1 (
        echo [清理] 删除旧的 build 目录...
        rmdir /s /q build
        echo [完成] 清理完成
        echo.
    )
)

:skip_clean
REM 创建 build 目录
if not exist build (
    echo [创建] 创建 build 目录...
    mkdir build
)

REM 进入 build 目录
cd build

echo [配置] 运行 CMake 配置...
echo.

REM 尝试使用 MinGW 或 NMake
where /q mingw32-make
if %errorlevel% equ 0 (
    echo [信息] 检测到 MinGW，使用 MinGW Makefiles
    cmake .. -G "MinGW Makefiles"
) else (
    echo [信息] 使用 NMake Makefiles
    cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
)

if errorlevel 1 (
    echo.
    echo [错误] CMake 配置失败！
    echo 请检查：
    echo   1. 是否已安装 CMake
    echo   2. 是否已安装 Qt5
    echo   3. 是否已设置 Qt5 环境变量
    echo.
    pause
    exit /b 1
)

echo.
echo [编译] 开始编译项目...
echo.

REM 根据生成器选择编译命令
where /q mingw32-make
if %errorlevel% equ 0 (
    mingw32-make
) else (
    nmake
)

if errorlevel 1 (
    echo.
    echo [错误] 编译失败！
    echo 请检查编译错误信息
    echo.
    cd ..
    pause
    exit /b 1
)

echo.
echo ====================================
echo [成功] 编译完成！
echo ====================================
echo 可执行文件位置: build\bin\SerialDebugger.exe
echo.

cd ..

REM 询问是否运行程序
choice /C YN /M "是否立即运行程序"
if errorlevel 2 goto :end
if errorlevel 1 (
    echo [运行] 启动程序...
    start build\bin\SerialDebugger.exe
)

:end
pause

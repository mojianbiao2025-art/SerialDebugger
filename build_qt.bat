@echo off
chcp 65001 >nul
echo ====================================
echo 串口调试助手 - Qt 编译脚本
echo ====================================
echo.

REM 请根据你的 Qt 安装路径修改以下变量
REM 示例: set QT_DIR=C:\Qt\5.15.2\msvc2019_64
set QT_DIR=D:\Qt\Qt5.6.3\5.6.3\msvc2013_64

if "%QT_DIR%"=="" (
    echo [错误] 请先设置 QT_DIR 变量！
    echo.
    echo 请编辑 build_qt.bat 文件，设置 QT_DIR 为你的 Qt 安装路径
    echo 例如: set QT_DIR=C:\Qt\5.15.2\msvc2019_64
    echo       set QT_DIR=C:\Qt\5.12.12\mingw73_64
    echo.
    pause
    exit /b 1
)

if not exist "%QT_DIR%" (
    echo [错误] Qt 目录不存在: %QT_DIR%
    echo 请检查路径是否正确
    echo.
    pause
    exit /b 1
)

echo [信息] Qt 路径: %QT_DIR%
echo.

REM 设置 Qt 环境变量
set PATH=%QT_DIR%\bin;%PATH%
set CMAKE_PREFIX_PATH=%QT_DIR%

REM 设置 MSVC 2013 环境变量
if exist "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" (
    call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
)

REM 检测编译器类型
if exist "%QT_DIR%\bin\qmake.exe" (
    echo [检测] 正在检测编译器类型...
    echo %QT_DIR% | findstr /i "mingw" >nul
    if %errorlevel% equ 0 (
        set COMPILER_TYPE=MinGW
        set GENERATOR=MinGW Makefiles
        set BUILD_CMD=mingw32-make
    ) else (
        set COMPILER_TYPE=MSVC
        set GENERATOR=NMake Makefiles
        set BUILD_CMD=nmake
    )
    echo [信息] 编译器类型: %COMPILER_TYPE%
) else (
    echo [错误] 未找到 qmake.exe
    pause
    exit /b 1
)

REM 清理或创建 build 目录
if exist build (
    echo [信息] 检测到 build 目录已存在
    echo [清理] 删除旧的 build 目录...
    rmdir /s /q build
)

if not exist build mkdir build
cd build

echo.
echo [配置] 运行 CMake 配置...
cmake .. -G "%GENERATOR%" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release

if errorlevel 1 (
    echo.
    echo [错误] CMake 配置失败！
    cd ..
    pause
    exit /b 1
)

echo.
echo [编译] 开始编译项目...
%BUILD_CMD%

if errorlevel 1 (
    echo.
    echo [错误] 编译失败！
    cd ..
    pause
    exit /b 1
)

echo.
echo ====================================
echo [成功] 编译完成！
echo ====================================
echo 可执行文件: build\bin\SerialDebugger.exe
echo.

cd ..

echo.
echo 按任意键退出...
pause >nul

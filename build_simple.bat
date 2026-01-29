@echo off
chcp 65001 >nul
echo ====================================
echo Serial Port Debugger - Build Script
echo ====================================
echo.

REM Set Qt path
set QT_DIR=D:\Qt\Qt5.6.3\5.6.3\msvc2013_64
set PATH=%QT_DIR%\bin;%PATH%
set CMAKE_PREFIX_PATH=%QT_DIR%

echo [Info] Qt Path: %QT_DIR%
echo.

REM Setup MSVC 2013 environment
echo [Info] Setting up MSVC 2013 environment...
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
echo.

REM Clean and create build directory
if exist build (
    echo [Clean] Removing old build directory...
    rmdir /s /q build
)
mkdir build
cd build

echo.
echo [Configure] Running CMake...
cmake .. -G "NMake Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release

if errorlevel 1 (
    echo.
    echo [Error] CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo [Build] Compiling project...
nmake

if errorlevel 1 (
    echo.
    echo [Error] Build failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ====================================
echo [Success] Build completed!
echo ====================================
echo Executable: build\bin\SerialDebugger.exe
echo.

cd ..
pause

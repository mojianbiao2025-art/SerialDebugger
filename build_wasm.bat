@echo off
REM WebAssembly Build Script for Serial Debugger
REM Author: 莫建标 (Mo Jianbiao)
REM Company: 上海大族富创得股份有限公司

echo ====================================
echo WebAssembly Build Script
echo ====================================
echo.

REM Check if Emscripten is installed
where emcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [Error] Emscripten not found!
    echo Please install Emscripten first:
    echo 1. Download from: https://emscripten.org/docs/getting_started/downloads.html
    echo 2. Run: emsdk install latest
    echo 3. Run: emsdk activate latest
    echo 4. Run: emsdk_env.bat
    pause
    exit /b 1
)

REM Set Qt WebAssembly path
set QT_WASM_DIR=D:\Qt\Qt5.15.2\5.15.2\wasm_32
if not exist "%QT_WASM_DIR%" (
    echo [Error] Qt for WebAssembly not found at: %QT_WASM_DIR%
    echo Please install Qt 5.15.2 for WebAssembly
    echo Download from: https://www.qt.io/download
    pause
    exit /b 1
)

echo [Info] Using Qt WebAssembly: %QT_WASM_DIR%
set PATH=%QT_WASM_DIR%\bin;%PATH%
set Qt5_DIR=%QT_WASM_DIR%

REM Create build directory
if not exist build-wasm (
    echo [Info] Creating build directory...
    mkdir build-wasm
)

cd build-wasm

REM Configure CMake
echo [Info] Configuring CMake...
emcmake cmake .. ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH=%Qt5_DIR% ^
    -G "MinGW Makefiles"

if %ERRORLEVEL% NEQ 0 (
    echo [Error] CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

REM Build
echo [Info] Building...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo [Error] Build failed!
    cd ..
    pause
    exit /b 1
)

REM Prepare deployment
echo [Info] Preparing deployment files...
if not exist deploy (
    mkdir deploy
)

copy /Y bin\SerialDebugger.* deploy\
copy /Y ..\index.html deploy\
copy /Y ..\lang_*.ini deploy\
copy /Y ..\styles.qss deploy\

cd ..

echo.
echo ====================================
echo Build completed successfully!
echo ====================================
echo.
echo Deployment files are in: build-wasm\deploy\
echo.
echo To test locally:
echo 1. cd build-wasm\deploy
echo 2. python -m http.server 8000
echo 3. Open browser: http://localhost:8000
echo.
echo Note: Use Chrome 89+, Edge 89+, or Opera 75+
echo.
pause

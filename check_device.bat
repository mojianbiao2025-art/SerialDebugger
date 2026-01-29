@echo off
chcp 65001 >nul
echo ====================================
echo Android 设备信息检查工具
echo Android Device Information Checker
echo ====================================
echo.

REM 检查 ADB 是否可用
where adb >nul 2>&1
if errorlevel 1 (
    echo [错误] 未找到 ADB 工具！
    echo [Error] ADB not found!
    echo.
    echo 请先安装 Android Platform Tools:
    echo Please install Android Platform Tools:
    echo https://developer.android.com/studio/releases/platform-tools
    echo.
    echo 或者使用 Android Studio 自带的 ADB:
    echo Or use ADB from Android Studio:
    echo C:\Users\YourName\AppData\Local\Android\Sdk\platform-tools\
    echo.
    pause
    exit /b 1
)

echo [1] 检查连接的设备...
echo [1] Checking connected devices...
adb devices
echo.

REM 检查是否有设备连接
adb devices | findstr "device$" >nul
if errorlevel 1 (
    echo [错误] 未检测到设备！
    echo [Error] No device detected!
    echo.
    echo 请确保：
    echo Please make sure:
    echo 1. 设备已通过 USB 连接到电脑
    echo    Device is connected via USB
    echo 2. 已启用 USB 调试模式
    echo    USB debugging is enabled
    echo 3. 已授权此电脑进行调试
    echo    This computer is authorized for debugging
    echo.
    echo 如何启用 USB 调试：
    echo How to enable USB debugging:
    echo 1. 设置 -^> 关于手机 -^> 连续点击"版本号"7次
    echo    Settings -^> About phone -^> Tap "Build number" 7 times
    echo 2. 设置 -^> 开发者选项 -^> 启用"USB调试"
    echo    Settings -^> Developer options -^> Enable "USB debugging"
    echo.
    pause
    exit /b 1
)

echo [2] 获取设备信息...
echo [2] Getting device information...
echo.

echo ====================================
echo 设备信息 / Device Information
echo ====================================
echo.

echo [设备型号 / Device Model]
for /f "tokens=*" %%a in ('adb shell getprop ro.product.model 2^>nul') do (
    echo %%a
    set DEVICE_MODEL=%%a
)
echo.

echo [Android 版本 / Android Version]
for /f "tokens=*" %%a in ('adb shell getprop ro.build.version.release 2^>nul') do (
    echo %%a
    set ANDROID_VERSION=%%a
)
echo.

echo [API Level / SDK 版本]
for /f "tokens=*" %%a in ('adb shell getprop ro.build.version.sdk 2^>nul') do (
    echo %%a
    set API_LEVEL=%%a
)
echo.

echo [CPU 架构 / CPU Architecture]
for /f "tokens=*" %%a in ('adb shell getprop ro.product.cpu.abi 2^>nul') do (
    echo %%a
    set CPU_ABI=%%a
)
echo.

echo [支持的所有架构 / All Supported ABIs]
for /f "tokens=*" %%a in ('adb shell getprop ro.product.cpu.abilist 2^>nul') do (
    echo %%a
    set ABI_LIST=%%a
)
echo.

echo [制造商 / Manufacturer]
for /f "tokens=*" %%a in ('adb shell getprop ro.product.manufacturer 2^>nul') do (
    echo %%a
)
echo.

echo ====================================
echo GitHub Actions 配置建议
echo GitHub Actions Configuration
echo ====================================
echo.

echo 在 .github/workflows/android-build.yml 中使用：
echo Use in .github/workflows/android-build.yml:
echo.
echo   -DANDROID_ABI=%CPU_ABI%
echo   -DANDROID_PLATFORM=android-%API_LEVEL%
echo.

echo ====================================
echo 架构说明 / Architecture Info
echo ====================================
echo.

if "%CPU_ABI%"=="arm64-v8a" (
    echo ✓ 64位 ARM 架构（最常见，推荐）
    echo ✓ 64-bit ARM architecture ^(Most common, Recommended^)
    echo.
    echo 这是现代 Android 设备的标准架构
    echo This is the standard architecture for modern Android devices
) else if "%CPU_ABI%"=="armeabi-v7a" (
    echo ✓ 32位 ARM 架构（较老设备）
    echo ✓ 32-bit ARM architecture ^(Older devices^)
    echo.
    echo 适用于 2016 年前的设备
    echo Suitable for devices before 2016
) else if "%CPU_ABI%"=="x86_64" (
    echo ✓ 64位 x86 架构（模拟器）
    echo ✓ 64-bit x86 architecture ^(Emulator^)
    echo.
    echo 这通常是 Android 模拟器
    echo This is usually an Android emulator
) else if "%CPU_ABI%"=="x86" (
    echo ✓ 32位 x86 架构（模拟器）
    echo ✓ 32-bit x86 architecture ^(Emulator^)
    echo.
    echo 这通常是 Android 模拟器
    echo This is usually an Android emulator
) else (
    echo ? 未知架构
    echo ? Unknown architecture
)
echo.

echo ====================================
echo API Level 说明 / API Level Info
echo ====================================
echo.

if %API_LEVEL% GEQ 30 (
    echo ✓ Android 11+ ^(API %API_LEVEL%^)
    echo   支持最新特性
    echo   Supports latest features
) else if %API_LEVEL% GEQ 24 (
    echo ✓ Android 7.0+ ^(API %API_LEVEL%^)
    echo   良好的兼容性
    echo   Good compatibility
) else if %API_LEVEL% GEQ 21 (
    echo ✓ Android 5.0+ ^(API %API_LEVEL%^)
    echo   最低支持版本
    echo   Minimum supported version
) else (
    echo ✗ Android 版本过低 ^(API %API_LEVEL%^)
    echo ✗ Android version too old ^(API %API_LEVEL%^)
    echo.
    echo 警告：此设备可能无法运行构建的 APK
    echo Warning: This device may not run the built APK
    echo 建议使用 Android 5.0 ^(API 21^) 或更高版本
    echo Recommend Android 5.0 ^(API 21^) or higher
)
echo.

echo ====================================
echo 配置文件生成 / Config File Generation
echo ====================================
echo.

REM 生成配置文件
echo # Android Build Configuration > android_config.txt
echo # Generated on %date% %time% >> android_config.txt
echo. >> android_config.txt
echo Device Model: %DEVICE_MODEL% >> android_config.txt
echo Android Version: %ANDROID_VERSION% >> android_config.txt
echo API Level: %API_LEVEL% >> android_config.txt
echo CPU ABI: %CPU_ABI% >> android_config.txt
echo ABI List: %ABI_LIST% >> android_config.txt
echo. >> android_config.txt
echo GitHub Actions Configuration: >> android_config.txt
echo   -DANDROID_ABI=%CPU_ABI% >> android_config.txt
echo   -DANDROID_PLATFORM=android-%API_LEVEL% >> android_config.txt

echo ✓ 配置已保存到 android_config.txt
echo ✓ Configuration saved to android_config.txt
echo.

echo ====================================
echo 下一步 / Next Steps
echo ====================================
echo.
echo 1. 复制上面的配置到 .github/workflows/android-build.yml
echo    Copy the configuration above to .github/workflows/android-build.yml
echo.
echo 2. 或者使用默认配置（推荐，支持大多数设备）：
echo    Or use default configuration ^(Recommended, supports most devices^):
echo    -DANDROID_ABI=arm64-v8a
echo    -DANDROID_PLATFORM=android-21
echo.
echo 3. 推送代码到 GitHub 开始构建
echo    Push code to GitHub to start building
echo.

pause

# 如何查看 Android 设备的 ABI 和 API Level

## 方法一：使用 ADB 命令（最准确）

### 1. 安装 ADB

#### Windows
1. 下载 [Android Platform Tools](https://developer.android.com/studio/releases/platform-tools)
2. 解压到任意目录，例如 `C:\adb`
3. 添加到系统环境变量 PATH

#### 或者使用 Android Studio
如果已安装 Android Studio，ADB 位于：
```
C:\Users\YourName\AppData\Local\Android\Sdk\platform-tools\
```

### 2. 启用 USB 调试

在 Android 设备上：
1. 设置 -> 关于手机
2. 连续点击"版本号" 7次，启用开发者选项
3. 设置 -> 开发者选项
4. 启用"USB 调试"

### 3. 连接设备并查询

打开命令行（PowerShell 或 CMD），执行：

```bash
# 检查设备是否连接
adb devices

# 查看 CPU 架构（ABI）
adb shell getprop ro.product.cpu.abi

# 查看所有支持的 ABI
adb shell getprop ro.product.cpu.abilist

# 查看 Android 版本（API Level）
adb shell getprop ro.build.version.sdk

# 查看 Android 版本名称
adb shell getprop ro.build.version.release

# 查看设备型号
adb shell getprop ro.product.model

# 一次性查看所有信息
adb shell getprop | findstr "cpu.abi\|version.sdk\|version.release\|product.model"
```

### 4. 结果解读

#### CPU 架构（ABI）对应关系：

| ABI 输出 | 说明 | GitHub Actions 配置 |
|---------|------|-------------------|
| `arm64-v8a` | 64位 ARM（最常见） | `-DANDROID_ABI=arm64-v8a` |
| `armeabi-v7a` | 32位 ARM | `-DANDROID_ABI=armeabi-v7a` |
| `x86_64` | 64位 x86（模拟器） | `-DANDROID_ABI=x86_64` |
| `x86` | 32位 x86（模拟器） | `-DANDROID_ABI=x86` |

#### API Level 对应关系：

| SDK 输出 | Android 版本 | GitHub Actions 配置 |
|---------|-------------|-------------------|
| `21` | Android 5.0 | `-DANDROID_PLATFORM=android-21` |
| `23` | Android 6.0 | `-DANDROID_PLATFORM=android-23` |
| `24` | Android 7.0 | `-DANDROID_PLATFORM=android-24` |
| `26` | Android 8.0 | `-DANDROID_PLATFORM=android-26` |
| `28` | Android 9.0 | `-DANDROID_PLATFORM=android-28` |
| `29` | Android 10 | `-DANDROID_PLATFORM=android-29` |
| `30` | Android 11 | `-DANDROID_PLATFORM=android-30` |
| `31` | Android 12 | `-DANDROID_PLATFORM=android-31` |
| `33` | Android 13 | `-DANDROID_PLATFORM=android-33` |

## 方法二：使用 Android 应用

### 推荐应用：

1. **CPU-Z**
   - Google Play 下载
   - 查看 "SOC" 标签页
   - 显示 CPU 架构和 Android 版本

2. **AIDA64**
   - Google Play 下载
   - 查看 "CPU" 和 "Android" 部分
   - 详细的硬件信息

3. **DevCheck**
   - Google Play 下载
   - 查看 "Hardware" 和 "System" 标签
   - 显示 ABI 和 API Level

### 查看步骤：
1. 安装上述任一应用
2. 打开应用
3. 查找以下信息：
   - **CPU 架构** / **Instruction Set** / **ABI**
   - **Android 版本** / **API Level** / **SDK Version**

## 方法三：在设备上直接查看

### 使用终端模拟器应用

1. 安装 "Termux" 或 "Terminal Emulator"
2. 打开应用，输入：

```bash
# 查看 ABI
getprop ro.product.cpu.abi

# 查看 API Level
getprop ro.build.version.sdk

# 查看 Android 版本
getprop ro.build.version.release
```

## 方法四：查看设备设置

### 简单方法（不太准确）

1. 设置 -> 关于手机
2. 查看 "Android 版本"
3. 根据版本推测 API Level（见上表）

**注意**：这个方法无法查看 CPU 架构

## 常见设备示例

### 主流手机 CPU 架构：

| 品牌/型号 | CPU 架构 | 推荐 ABI |
|---------|---------|---------|
| 小米、华为、OPPO、vivo（2017年后） | ARM64 | `arm64-v8a` |
| 三星 Galaxy（2016年后） | ARM64 | `arm64-v8a` |
| 老旧设备（2016年前） | ARM32 | `armeabi-v7a` |
| Android 模拟器 | x86_64 | `x86_64` |

### 推荐配置：

**大多数现代设备（推荐）：**
```yaml
-DANDROID_ABI=arm64-v8a
-DANDROID_PLATFORM=android-21  # 支持 Android 5.0+
```

**兼容更多设备：**
```yaml
-DANDROID_ABI=armeabi-v7a
-DANDROID_PLATFORM=android-21
```

## 修改 GitHub Actions 配置

根据你的设备信息，修改 `.github/workflows/android-build.yml`：

### 示例 1：64位 ARM 设备（最常见）

```yaml
- name: Configure CMake
  working-directory: build-android
  run: |
    cmake .. \
      -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-21 \
      ...
```

### 示例 2：32位 ARM 设备

```yaml
-DANDROID_ABI=armeabi-v7a \
-DANDROID_PLATFORM=android-21 \
```

### 示例 3：构建多个架构（推荐）

如果想支持更多设备，可以构建多个 APK：

```yaml
strategy:
  matrix:
    abi: [arm64-v8a, armeabi-v7a]

- name: Configure CMake
  run: |
    cmake .. \
      -DANDROID_ABI=${{ matrix.abi }} \
      ...
```

## 快速检查脚本

创建 `check_device.bat`：

```batch
@echo off
echo ====================================
echo Android Device Information Checker
echo ====================================
echo.

echo Checking connected devices...
adb devices
echo.

echo [CPU Architecture]
adb shell getprop ro.product.cpu.abi
echo.

echo [Supported ABIs]
adb shell getprop ro.product.cpu.abilist
echo.

echo [Android API Level]
adb shell getprop ro.build.version.sdk
echo.

echo [Android Version]
adb shell getprop ro.build.version.release
echo.

echo [Device Model]
adb shell getprop ro.product.model
echo.

echo ====================================
echo Configuration Recommendation:
echo ====================================

for /f "tokens=*" %%a in ('adb shell getprop ro.product.cpu.abi') do set ABI=%%a
for /f "tokens=*" %%a in ('adb shell getprop ro.build.version.sdk') do set SDK=%%a

echo.
echo Use these settings in GitHub Actions:
echo   -DANDROID_ABI=%ABI%
echo   -DANDROID_PLATFORM=android-%SDK%
echo.

pause
```

## 总结

### 最简单的方法：
1. 安装 CPU-Z 应用
2. 查看 CPU 架构和 Android 版本
3. 根据上面的对照表配置

### 最准确的方法：
1. 启用 USB 调试
2. 连接电脑
3. 运行 `adb shell getprop ro.product.cpu.abi`
4. 运行 `adb shell getprop ro.build.version.sdk`

### 默认推荐配置（适用于大多数设备）：
```yaml
-DANDROID_ABI=arm64-v8a
-DANDROID_PLATFORM=android-21
```

这个配置支持 2017 年后的大多数 Android 设备！

## 需要帮助？

如果你告诉我你的手机型号和 Android 版本，我可以直接告诉你应该使用什么配置！

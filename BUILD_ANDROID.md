# 快速构建 Android APK

## 简化步骤

由于完整的 Android 构建需要大量配置，这里提供最简单的方法：

### 方法一：使用 Qt Creator（最简单）

1. **安装 Qt for Android**
   - 下载 Qt 在线安装器
   - 选择 Qt 5.15.2 或更高版本
   - 勾选 Android 组件（ARMv7, ARM64）

2. **配置 Android 环境**
   - 在 Qt Creator 中：工具 -> 选项 -> 设备 -> Android
   - 设置 JDK 路径（需要 JDK 8）
   - 设置 Android SDK 路径
   - 设置 Android NDK 路径
   - 点击"应用"让 Qt Creator 自动下载缺失组件

3. **打开项目**
   - 文件 -> 打开文件或项目
   - 选择 `CMakeLists.txt`
   - 在 Kit 选择中勾选 Android Kit

4. **构建 APK**
   - 左侧选择 Android Kit
   - 构建 -> 构建 Android APK
   - 选择发布模式
   - 等待构建完成
   - APK 位于 `build-android/android-build/build/outputs/apk/`

### 方法二：在线构建服务

如果本地配置困难，可以使用在线服务：

1. **使用 GitHub Actions**
   - 将代码推送到 GitHub
   - 配置 GitHub Actions 自动构建
   - 下载构建好的 APK

2. **使用 Qt Cloud**
   - 上传项目到 Qt Cloud
   - 选择 Android 平台
   - 自动构建并下载

## 当前项目状态

已创建的 Android 配置文件：
- ✅ `android/AndroidManifest.xml` - Android 清单文件
- ✅ `android/res/xml/device_filter.xml` - USB 设备过滤器
- ✅ `android/res/drawable/icon.png` - 应用图标
- ✅ `ANDROID_BUILD_GUIDE.md` - 详细构建指南

## 重要提示

### Android 串口限制
由于 Android 系统限制，串口访问需要：
1. **USB OTG 支持** - 设备必须支持 USB Host 模式
2. **USB 权限** - 用户需要授予 USB 设备访问权限
3. **特定驱动** - 需要使用 Android USB Serial 库

### 推荐方案
对于 Android 平台，建议：
1. 使用蓝牙串口（更通用）
2. 使用 WiFi 串口转换器
3. 使用专门的 Android 串口库（如 usb-serial-for-android）

## 下一步

如果你想继续 Android 开发，需要：

1. **安装开发环境**
   ```
   - Qt 5.15.2+ for Android
   - Android Studio (SDK + NDK)
   - JDK 8
   ```

2. **修改代码以支持 Android**
   - 添加 Android 权限请求
   - 使用 Android USB Serial 库
   - 适配触摸屏界面

3. **测试**
   - 在 Android 设备上测试
   - 测试 USB OTG 功能
   - 测试不同屏幕尺寸

## 替代方案

如果 Android 移植过于复杂，可以考虑：

1. **Web 版本**
   - 使用 Qt WebAssembly
   - 通过浏览器访问
   - 使用 Web Serial API

2. **远程控制**
   - Windows 程序作为服务器
   - Android 应用作为客户端
   - 通过网络控制串口

3. **专用 Android 应用**
   - 使用 Java/Kotlin 重写
   - 使用 Android 原生 USB API
   - 更好的 Android 集成

## 需要帮助？

如果需要完整的 Android 移植，建议：
1. 阅读 `ANDROID_BUILD_GUIDE.md` 详细指南
2. 参考 Qt 官方 Android 示例
3. 考虑聘请 Android 开发人员协助

当前 Windows 版本已经完整可用，Android 版本需要额外的开发工作。

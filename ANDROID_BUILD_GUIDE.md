# Android APK 构建指南

## 前提条件

### 1. 安装必要工具

#### Android Studio 和 SDK
1. 下载并安装 [Android Studio](https://developer.android.com/studio)
2. 安装 Android SDK (API Level 21 或更高)
3. 安装 Android NDK (r21 或更高)
4. 配置环境变量：
   ```
   ANDROID_SDK_ROOT=C:\Users\YourName\AppData\Local\Android\Sdk
   ANDROID_NDK_ROOT=C:\Users\YourName\AppData\Local\Android\Sdk\ndk\21.x.xxxx
   ```

#### Qt for Android
1. 下载 [Qt 5.15.2 for Android](https://www.qt.io/download)
2. 安装时选择 Android 组件：
   - Android ARMv7
   - Android ARM64
   - Android x86
   - Android x86_64

#### Java JDK
1. 下载并安装 [JDK 8](https://www.oracle.com/java/technologies/javase/javase-jdk8-downloads.html)
2. 配置 JAVA_HOME 环境变量

### 2. 项目配置

#### 创建 Android 清单文件

创建 `android/AndroidManifest.xml`：

```xml
<?xml version="1.0"?>
<manifest package="com.hanscnc.serialdebugger" 
          xmlns:android="http://schemas.android.com/apk/res/android" 
          android:versionName="1.0" 
          android:versionCode="1" 
          android:installLocation="auto">
    
    <application android:hardwareAccelerated="true" 
                 android:name="org.qtproject.qt5.android.bindings.QtApplication" 
                 android:label="串口调试助手"
                 android:icon="@drawable/icon">
        
        <activity android:configChanges="orientation|uiMode|screenLayout|screenSize|smallestScreenSize|layoutDirection|locale|fontScale|keyboard|keyboardHidden|navigation"
                  android:name="org.qtproject.qt5.android.bindings.QtActivity"
                  android:label="串口调试助手"
                  android:screenOrientation="unspecified"
                  android:launchMode="singleTop">
            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>
            
            <!-- USB 设备权限 -->
            <intent-filter>
                <action android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED"/>
            </intent-filter>
            
            <meta-data android:name="android.hardware.usb.action.USB_DEVICE_ATTACHED"
                       android:resource="@xml/device_filter"/>
            
            <meta-data android:name="android.app.lib_name"
                       android:value="SerialDebugger"/>
            <meta-data android:name="android.app.qt_sources_resource_id"
                       android:resource="@array/qt_sources"/>
            <meta-data android:name="android.app.repository"
                       android:value="default"/>
            <meta-data android:name="android.app.qt_libs_resource_id"
                       android:resource="@array/qt_libs"/>
            <meta-data android:name="android.app.bundled_libs_resource_id"
                       android:resource="@array/bundled_libs"/>
            <meta-data android:name="android.app.extract_android_style"
                       android:value="minimal"/>
        </activity>
    </application>
    
    <!-- 权限 -->
    <uses-sdk android:minSdkVersion="21" android:targetSdkVersion="30"/>
    <supports-screens android:largeScreens="true" 
                      android:normalScreens="true" 
                      android:anyDensity="true" 
                      android:smallScreens="true"/>
    
    <!-- USB 权限 -->
    <uses-feature android:name="android.hardware.usb.host"/>
    <uses-permission android:name="android.permission.USB_PERMISSION"/>
    <uses-permission android:name="android.permission.INTERNET"/>
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
</manifest>
```

#### 创建 USB 设备过滤器

创建 `android/res/xml/device_filter.xml`：

```xml
<?xml version="1.0" encoding="utf-8"?>
<resources>
    <!-- USB Serial 设备 -->
    <usb-device vendor-id="1027" product-id="24577"/> <!-- FTDI -->
    <usb-device vendor-id="9025" product-id="32822"/> <!-- Prolific -->
    <usb-device vendor-id="4292" product-id="60000"/> <!-- CH340 -->
    <usb-device vendor-id="6790" product-id="29987"/> <!-- CP210x -->
</resources>
```

### 3. 修改 CMakeLists.txt

添加 Android 支持：

```cmake
# Android 特定配置
if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    set(ANDROID_MIN_SDK_VERSION 21)
    set(ANDROID_TARGET_SDK_VERSION 30)
    
    # 添加 Android 权限
    set(ANDROID_EXTRA_LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/path_to_libs/libssl.so
        ${CMAKE_CURRENT_SOURCE_DIR}/path_to_libs/libcrypto.so
    )
endif()
```

### 4. 构建步骤

#### 使用 Qt Creator (推荐)

1. 打开 Qt Creator
2. 打开项目 (CMakeLists.txt)
3. 在左侧工具栏选择 "Projects"
4. 点击 "Add Kit" 选择 Android Kit
5. 配置 Android 设置：
   - Android SDK location
   - Android NDK location
   - JDK location
6. 选择 Android Kit
7. 点击 "Build" -> "Build Project"
8. 点击 "Build" -> "Build Android APK"

#### 使用命令行

```bash
# 设置环境变量
set ANDROID_SDK_ROOT=C:\Users\YourName\AppData\Local\Android\Sdk
set ANDROID_NDK_ROOT=%ANDROID_SDK_ROOT%\ndk\21.x.xxxx
set JAVA_HOME=C:\Program Files\Java\jdk1.8.0_xxx
set Qt5_DIR=D:\Qt\5.15.2\android

# 创建构建目录
mkdir build-android
cd build-android

# 配置 CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK_ROOT%/build/cmake/android.toolchain.cmake ^
         -DANDROID_ABI=armeabi-v7a ^
         -DANDROID_PLATFORM=android-21 ^
         -DCMAKE_PREFIX_PATH=%Qt5_DIR% ^
         -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release

# 创建 APK
androiddeployqt --input android-SerialDebugger-deployment-settings.json ^
                --output android-build ^
                --android-platform android-30 ^
                --jdk %JAVA_HOME% ^
                --gradle
```

### 5. Android 特定代码修改

#### 串口访问

Android 上的串口访问需要使用 USB Host API。需要添加 Android 特定代码：

创建 `android_serial.h`:

```cpp
#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>

class AndroidSerial {
public:
    static bool requestUsbPermission();
    static QStringList getUsbDevices();
};
#endif
```

#### 权限请求

在 `main.cpp` 中添加：

```cpp
#ifdef Q_OS_ANDROID
#include <QtAndroid>

void requestAndroidPermissions() {
    QtAndroid::requestPermissionsSync(QStringList() 
        << "android.permission.WRITE_EXTERNAL_STORAGE"
        << "android.permission.READ_EXTERNAL_STORAGE");
}
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
#ifdef Q_OS_ANDROID
    requestAndroidPermissions();
#endif
    
    MainWindow w;
    w.show();
    return a.exec();
}
```

### 6. 界面适配

#### 触摸屏优化

修改 `styles.qss` 增加触摸目标大小：

```css
/* Android 触摸优化 */
QPushButton {
    min-height: 48px;  /* Android 推荐最小触摸目标 */
    min-width: 48px;
}

QComboBox {
    min-height: 48px;
}
```

#### 屏幕方向

在 `mainwindow.cpp` 构造函数中：

```cpp
#ifdef Q_OS_ANDROID
    // 设置为横屏模式
    setWindowState(Qt::WindowFullScreen);
#endif
```

### 7. 测试

1. 连接 Android 设备到电脑
2. 启用 USB 调试模式
3. 在 Qt Creator 中点击 "Run" 按钮
4. APK 将自动安装并运行在设备上

### 8. 发布

#### 签名 APK

```bash
# 生成密钥库
keytool -genkey -v -keystore my-release-key.keystore ^
        -alias my-key-alias -keyalg RSA -keysize 2048 -validity 10000

# 签名 APK
jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 ^
          -keystore my-release-key.keystore ^
          android-build/build/outputs/apk/release/android-build-release-unsigned.apk ^
          my-key-alias

# 对齐 APK
zipalign -v 4 android-build-release-unsigned.apk SerialDebugger-release.apk
```

## 注意事项

### 1. 串口限制
- Android 需要 USB OTG 支持
- 需要 USB Host 模式
- 某些设备可能不支持所有串口芯片

### 2. 权限
- 首次运行需要授予 USB 权限
- 需要存储权限来保存日志

### 3. 性能
- Android 设备性能可能较低
- 建议限制绘图数据点数量
- 优化界面刷新频率

### 4. 兼容性
- 最低支持 Android 5.0 (API 21)
- 推荐 Android 7.0+ (API 24+)
- 需要支持 USB Host 的设备

## 常见问题

### Q: 找不到串口设备？
A: 检查：
1. 设备是否支持 USB OTG
2. 是否授予了 USB 权限
3. USB 转串口芯片是否在 device_filter.xml 中

### Q: APK 安装失败？
A: 检查：
1. Android SDK/NDK 版本是否正确
2. 签名是否正确
3. 设备 Android 版本是否满足要求

### Q: 程序崩溃？
A: 检查：
1. Qt 库是否正确打包
2. 权限是否正确配置
3. 查看 logcat 日志

## 相关资源

- [Qt for Android 官方文档](https://doc.qt.io/qt-5/android.html)
- [Android USB Host API](https://developer.android.com/guide/topics/connectivity/usb/host)
- [Qt Android 示例](https://doc.qt.io/qt-5/examples-android.html)

## 技术支持

如遇到问题，请提供：
1. Android 设备型号和系统版本
2. Qt 版本
3. 错误日志 (logcat)
4. 构建配置信息

# Android平台特定修复说明

## 问题分析

### 1. Tab标签白色文字问题
**根本原因：**
- Android平台的Qt渲染引擎对样式表的处理与桌面版本不同
- 全局样式表（styles.qss）中的Tab样式在Android上可能被系统主题覆盖
- 需要使用内联样式（inline stylesheet）并指定完整的样式属性

**解决方案：**
- 为Android平台单独设置Tab样式，使用更大的字体（18pt）
- 明确指定所有状态下的颜色为黑色 `#000000`
- 使用完整的样式定义，包括背景、边框、内边距等

### 2. 语言切换不生效问题
**根本原因：**
- Android上的UI更新可能需要显式调用 `update()` 方法
- Tab标签的文本更新后，TabBar需要重新绘制
- 菜单栏在Android上被隐藏，使用自定义工具栏菜单

**解决方案：**
- 在 `retranslateUI()` 中强制调用 `tabBar()->update()` 和 `mainTabWidget->update()`
- 跳过桌面版菜单的更新（Android使用自定义菜单）
- 更新状态栏文本以提供视觉反馈

## 代码修改详情

### 修改1：setupAdvancedUI() - Tab样式设置

**位置：** mainwindow.cpp，约第850-900行

```cpp
#ifdef Q_OS_ANDROID
    // Android needs inline styles with complete property definitions
    mainTabWidget->tabBar()->setStyleSheet(
        "QTabBar::tab { "
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f0f0f0, stop: 1 #e0e0e0); "
        "    color: #000000; "  // 黑色文字
        "    border: 2px solid #bdc3c7; "
        "    border-bottom: none; "
        "    border-top-left-radius: 8px; "
        "    border-top-right-radius: 8px; "
        "    padding: 12px 45px 12px 45px; "
        "    margin-right: 6px; "
        "    margin-top: 2px; "
        "    font-weight: bold; "
        "    font-size: 18pt; "  // 更大的字体
        "    min-width: 120px; "
        "    min-height: 20px; "
        "} "
        "QTabBar::tab:selected { "
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #ffffff, stop: 1 #f8f8f8); "
        "    color: #000000; "  // 选中状态也是黑色
        "    border-color: #3498db; "
        "    border-bottom: 2px solid white; "
        "    margin-bottom: -2px; "
        "    padding: 12px 45px 12px 45px; "
        "    font-size: 18pt; "
        "} "
        "QTabBar::tab:hover { "
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e8e8e8, stop: 1 #d8d8d8); "
        "    color: #000000; "  // 悬停状态也是黑色
        "    border-color: #95a5a6; "
        "}"
    );
#else
    // Desktop version - simpler style
    mainTabWidget->tabBar()->setStyleSheet(
        "QTabBar::tab { color: #000000; }"
        "QTabBar::tab:selected { color: #000000; }"
        "QTabBar::tab:hover { color: #000000; }"
    );
#endif
```

**关键点：**
1. 使用 `#ifdef Q_OS_ANDROID` 条件编译
2. Android版本使用完整的样式定义
3. 字体大小设置为 18pt（Android上更清晰）
4. 所有状态明确指定黑色文字

### 修改2：retranslateUI() - 强制更新Tab标签

**位置：** mainwindow.cpp，约第700-800行

```cpp
void MainWindow::retranslateUI()
{
    QMap<QString, QString> trans = Translations::getTranslations(currentLanguage);
    
    // Window title
    setWindowTitle(trans["window_title"]);
    
    // Tab titles - CRITICAL: Force update tab text
    if (mainTabWidget) {
        mainTabWidget->setTabText(0, trans["tab_main"]);
        mainTabWidget->setTabText(1, trans["tab_plotter"]);
        
#ifdef Q_OS_ANDROID
        // Android: Force repaint to ensure text is visible
        mainTabWidget->tabBar()->update();
        mainTabWidget->update();
#endif
    }
    
    // ... 其他UI更新 ...
    
#ifndef Q_OS_ANDROID
    // Menu - only update if not Android (Android uses custom menu)
    ui->menuFile->setTitle(trans["menu_file"]);
    ui->menuView->setTitle(trans["menu_view"]);
    ui->menuLanguage->setTitle(trans["menu_language"]);
    ui->menuHelp->setTitle(trans["menu_help"]);
#endif
    
    // ... 其他UI更新 ...
    
#ifdef Q_OS_ANDROID
    // Android: Show a brief message to confirm language change
    statusLabel->setText(trans["status_disconnected"]);
#endif
}
```

**关键点：**
1. 更新Tab文本后立即调用 `update()` 方法
2. 跳过Android上不存在的菜单栏更新
3. 更新状态栏提供视觉反馈

## 测试步骤

### 1. 测试Tab标签显示

**步骤：**
1. 在Android设备上安装并启动应用
2. 观察"Main"和"Plotter"标签
3. 检查文字是否为黑色且清晰可见
4. 点击切换标签，检查选中和未选中状态

**预期结果：**
- 所有状态下文字都是黑色
- 字体大小适中（18pt）
- 选中状态有明显的视觉区分（蓝色边框）

### 2. 测试语言切换

**步骤：**
1. 点击工具栏的"☰ Menu"按钮
2. 选择"🌐 Language"
3. 选择"English"
4. 观察界面变化

**预期结果：**
- Tab标签文字立即从"主界面/波形图"变为"Main/Plotter"
- 所有按钮、标签、菜单项都更新为英文
- 状态栏显示"Status: Disconnected"

**继续测试：**
5. 切换到其他语言（日语、德语、法语）
6. 每次切换后检查Tab标签是否正确更新
7. 返回中文，确认所有文字恢复

### 3. 完整功能测试

**步骤：**
1. 切换到英文界面
2. 打开串口（如果有可用设备）
3. 发送和接收数据
4. 切换到"Plotter"标签
5. 再次切换语言
6. 检查所有功能是否正常

**预期结果：**
- 语言切换不影响串口连接状态
- 数据接收和发送功能正常
- Tab切换流畅
- 所有文字显示正确

## 调试技巧

### 如果Tab标签仍然看不清

1. **检查设备主题：**
   - 某些Android设备的系统主题可能影响应用样式
   - 尝试在设备设置中切换到浅色主题

2. **增加字体大小：**
   ```cpp
   "    font-size: 20pt; "  // 从18pt增加到20pt
   ```

3. **使用更强的对比色：**
   ```cpp
   "    color: #000000; "
   "    background: #FFFFFF; "  // 纯白背景
   ```

4. **添加文字阴影（可选）：**
   ```cpp
   "    text-shadow: 1px 1px 2px rgba(255,255,255,0.8); "
   ```

### 如果语言切换不生效

1. **检查翻译文件：**
   - 确认 `lang_*.ini` 文件已复制到APK中
   - 检查文件编码是否为UTF-8

2. **添加调试输出：**
   ```cpp
   qDebug() << "Current language:" << currentLanguage;
   qDebug() << "Tab 0 text:" << mainTabWidget->tabText(0);
   qDebug() << "Tab 1 text:" << mainTabWidget->tabText(1);
   ```

3. **强制重新加载：**
   ```cpp
   mainTabWidget->setCurrentIndex(1);
   mainTabWidget->setCurrentIndex(0);
   ```

## 构建和部署

### 确保翻译文件包含在APK中

在 `CMakeLists.txt` 中已经配置：

```cmake
# Copy language files to build directory
file(GLOB LANG_FILES "${CMAKE_SOURCE_DIR}/lang_*.ini")
foreach(LANG_FILE ${LANG_FILES})
    get_filename_component(LANG_FILENAME ${LANG_FILE} NAME)
    configure_file(${LANG_FILE} ${CMAKE_BINARY_DIR}/bin/${LANG_FILENAME} COPYONLY)
endforeach()

# Install language files
if(LANG_FILES)
    install(FILES ${LANG_FILES} DESTINATION bin)
endif()
```

### Android资源配置

确保 `android/AndroidManifest.xml` 中包含：

```xml
<application
    android:label="串口调试助手"
    android:theme="@android:style/Theme.Light.NoTitleBar.Fullscreen">
```

## 已知限制

1. **系统主题影响：**
   - 某些Android设备的深色主题可能影响样式
   - 建议在应用中明确设置浅色主题

2. **字体渲染：**
   - 不同Android版本的字体渲染可能略有差异
   - 已使用18pt字体以确保在大多数设备上清晰

3. **语言文件路径：**
   - Android上文件访问路径与桌面版不同
   - 翻译加载使用多路径搜索机制

## 总结

通过以上修改，Android版本应该能够：
- ✅ 显示清晰的黑色Tab标签文字
- ✅ 正确响应语言切换
- ✅ 在所有状态下保持良好的可读性
- ✅ 提供流畅的用户体验

如果问题仍然存在，请检查：
1. 是否使用最新的代码重新编译
2. APK是否包含所有翻译文件
3. 设备的Android版本和系统主题设置

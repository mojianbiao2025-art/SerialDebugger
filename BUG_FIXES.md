# 问题修复说明

## 修复的问题

### 1. WebAssembly构建链接错误
**问题描述：** WebAssembly构建时出现embind相关的未定义符号错误。

**错误信息：**
```
error: undefined symbol: _embind_register_function
error: undefined symbol: _emval_as
error: undefined symbol: _emval_call
...
```

**修复方案：**
- 在CMakeLists.txt中为WebAssembly目标添加 `-lembind` 链接标志
- 这是因为webserialport.cpp使用了Emscripten的embind库来与JavaScript交互

**修改文件：**
- `CMakeLists.txt` - 第 122 行

### 2. Tab标签字体白色看不清
**问题描述：** Tab标签（"Main"和"Plotter"）的文字颜色在某些状态下显示为白色或浅色，在浅色背景上看不清楚。

**修复方案：**
- 修改 `styles.qss` 文件中的 `QTabBar::tab` 样式
- 将所有状态下的文字颜色统一设置为黑色 `#000000`
- 在 `mainwindow.cpp` 的 `setupAdvancedUI()` 函数中添加额外的样式表强制设置黑色文字

**修改文件：**
- `styles.qss` - 第 467-495 行
- `mainwindow.cpp` - 第 858-863 行

### 3. 语言切换功能增强
**问题描述：** 语言切换后，菜单中的选中状态没有正确更新。

**修复方案：**
- 在 `switchLanguage()` 函数中添加代码，根据选择的语言更新对应菜单项的选中状态
- 确保语言切换后界面立即更新

**修改文件：**
- `mainwindow.cpp` - 第 774-790 行

### 4. Android界面显示不全
**问题描述：** 在Android设备上，界面内容被截断，无法完整显示所有控件。

**修复方案：**
- 修改ScrollArea的滚动条策略，从 `Qt::ScrollBarAlwaysOff` 改为 `Qt::ScrollBarAsNeeded`
- 为中央widget设置最小宽度 800px
- 设置中央widget的尺寸策略为 `Expanding`，允许内容自适应扩展
- 启用水平和垂直滚动条，确保所有内容都可以通过滚动访问

**修改文件：**
- `mainwindow.cpp` - 第 52-63 行

## 测试建议

### 测试WebAssembly构建
1. 运行构建脚本：`build_wasm.bat` 或使用GitHub Actions
2. 确认构建成功完成，没有链接错误
3. 在浏览器中打开生成的HTML文件
4. 测试串口功能（需要支持Web Serial API的浏览器，如Chrome/Edge）

### 测试Tab标签显示
1. 编译并运行应用程序
2. 检查"Main"和"Plotter"标签的文字是否清晰可见
3. 切换标签，确认选中和未选中状态下文字都是黑色

### 测试语言切换
1. 打开应用程序
2. 点击菜单 Language -> English
3. 确认界面文字切换为英文，且English菜单项显示为选中状态
4. 切换到其他语言（中文、日语、德语、法语）
5. 确认每次切换后界面文字正确更新，对应菜单项显示选中状态

### 测试Android界面
1. 在Android设备上安装并运行应用
2. 检查所有控件是否可见
3. 尝试水平和垂直滚动，确认可以访问所有内容
4. 测试不同屏幕尺寸和方向（竖屏/横屏）

## 技术细节

### WebAssembly embind库
Emscripten的embind库用于在C++和JavaScript之间进行互操作。`webserialport.cpp`使用embind来：
- 调用浏览器的Web Serial API
- 处理JavaScript回调
- 在C++和JavaScript之间传递数据

链接时必须包含 `-lembind` 标志，否则会出现未定义符号错误。

### Tab标签颜色问题的根本原因
原始样式表中：
- 未选中状态：`color: #2c3e50` (深灰色)
- 选中状态：`color: #3498db` (蓝色)
- 悬停状态：`color: #2980b9` (深蓝色)

在某些渲染情况下，这些颜色可能与背景色对比度不足。修复后统一使用黑色 `#000000`，确保在所有背景下都有足够的对比度。

### 语言切换机制
应用使用 `Translations::getTranslations()` 函数从 `.ini` 文件加载翻译。`retranslateUI()` 函数遍历所有UI元素并应用翻译。添加的代码确保菜单项的选中状态与当前语言同步。

### Android滚动区域
Android设备屏幕尺寸差异很大。通过启用滚动条并设置合理的最小尺寸，确保：
- 内容不会被强制压缩
- 用户可以通过滚动访问所有功能
- 界面在不同设备上保持一致性

## 编译和部署

### 桌面版本
```bash
build_simple.bat
```

### Android版本
参考 `BUILD_ANDROID.md` 或 `ANDROID_BUILD_GUIDE.md`

### WebAssembly版本
```bash
build_wasm.bat
```

## 后续建议

1. **响应式设计：** 考虑为不同屏幕尺寸实现更智能的布局调整
2. **主题系统：** 添加深色主题选项，提供更好的可访问性
3. **字体大小调整：** 允许用户自定义字体大小
4. **持久化设置：** 保存用户的语言选择和其他偏好设置

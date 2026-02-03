# WebAssembly 错误修复说明

## 🎯 您遇到的错误

根据您提供的控制台日志，有两个关键错误：

### 错误 1：Module.canvas 已弃用 ⚠️
```
Module.canvas is deprecated. Instead, set Module.qtCanvasElements to be an array
```

### 错误 2：无法读取 undefined 的属性 ❌
```
TypeError: Cannot read properties of undefined (reading 'apply')
at __emval_call
```

## 🔍 问题原因

### 原因 1：Qt API 更新
Qt for WebAssembly 更新了 API，不再使用 `Module.canvas`，而是使用 `Module.qtCanvasElements`。

### 原因 2：缺少函数导出
WebAssembly 模块没有导出必要的运行时方法（如 `ccall`、`cwrap`）和 C++ 函数，导致 JavaScript 无法调用 C++ 代码。

具体来说：
- `webserialport.cpp` 中的 JavaScript 代码尝试调用 `Module.ccall()`
- 但 `ccall` 没有被导出到 JavaScript 环境
- 导致 `undefined.apply()` 错误

## ✅ 我已经实施的修复

### 修复 1：更新 index.html ✅
将 `Module.canvas` 改为 `Module.qtCanvasElements`：

```javascript
// 修改前（已弃用）
var Module = {
    canvas: document.getElementById('canvas'),
    // ...
};

// 修改后（新 API）
var Module = {
    qtCanvasElements: [document.getElementById('canvas')],
    // ...
};
```

### 修复 2：更新 CMakeLists.txt ✅
添加必要的导出配置：

```cmake
# 修改前
LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind"

# 修改后
LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind \
    -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','allocateUTF8'] \
    -s EXPORTED_FUNCTIONS=['_malloc','_free','_webserial_onOpened_c','_webserial_onError_c','_webserial_onDataReceived_c']"
```

**导出的内容：**
- `ccall`: 从 JavaScript 调用 C++ 函数
- `cwrap`: 包装 C++ 函数
- `allocateUTF8`: 分配 UTF-8 字符串
- `_malloc` / `_free`: 内存管理
- `_webserial_*`: 串口回调函数

## 🔄 下一步操作

### 方案 A：等待 GitHub Actions 自动构建（推荐）

1. **提交修改：**
   ```bash
   git add CMakeLists.txt index.html
   git commit -m "Fix: Export runtime methods for WebAssembly"
   git push
   ```

2. **等待构建完成：**
   - 访问：https://github.com/mojianbiao2025-art/SerialDebugger/actions
   - 等待绿色勾号（约 10-15 分钟）

3. **测试新版本：**
   - 访问：https://mojianbiao2025-art.github.io/SerialDebugger/
   - 按 `Ctrl+F5` 强制刷新（清除缓存）

### 方案 B：本地重新构建和测试

1. **清理旧构建：**
   ```bash
   rm -rf build-wasm
   ```

2. **重新构建：**
   ```bash
   source ~/emsdk/emsdk_env.sh
   mkdir build-wasm && cd build-wasm
   emcmake cmake .. -G Ninja
   ninja
   ```

3. **部署文件：**
   ```bash
   cd ..
   mkdir -p deploy
   cp build-wasm/bin/SerialDebugger.* deploy/
   cp index.html deploy/
   cp test-wasm-loading.html deploy/
   cp lang_*.ini deploy/
   cp styles.qss deploy/
   ```

4. **启动服务器测试：**
   ```bash
   cd deploy
   python -m http.server 8000
   ```

5. **访问测试：**
   - 先测试诊断工具：http://localhost:8000/test-wasm-loading.html
   - 再测试主应用：http://localhost:8000/index.html

## 🎯 预期结果

### 修复后应该看到：

**浏览器控制台（F12）：**
```
✓ 使用 http: 协议
✓ 浏览器支持 Web Serial API
✓ SerialDebugger.js 找到 (450 KB)
✓ SerialDebugger.wasm 找到 (3.50 MB)
✓ 所有必需文件检查通过
正在下载应用程序...
正在加载依赖项 (0/1)
所有依赖项已加载
✓ WebAssembly 运行时初始化成功！
✓ 应用程序加载完成！
```

**页面显示：**
- ✅ Qt 应用程序完整界面
- ✅ 菜单栏（文件、编辑、视图、语言、帮助）
- ✅ Main 和 Plotter 标签页
- ✅ 串口设置区域
- ✅ 发送/接收文本框
- ✅ 连接按钮可以点击

**不应该再看到：**
- ❌ `Module.canvas is deprecated` 警告
- ❌ `TypeError: Cannot read properties of undefined` 错误
- ❌ 一直停留在加载界面

## 🔍 如何验证修复成功

### 检查 1：控制台没有错误
按 `F12` 打开控制台，应该没有红色错误信息。

### 检查 2：应用界面显示
页面应该显示完整的 Qt 应用程序界面，不是加载界面。

### 检查 3：串口功能可用
点击"连接"按钮，应该弹出串口选择对话框（需要 Chrome/Edge 89+）。

### 检查 4：语言切换正常
菜单栏 → 语言 → 选择不同语言，界面应该立即更新。

## 🚨 如果问题仍然存在

### 步骤 1：清除浏览器缓存
```
Chrome/Edge: Ctrl+Shift+Delete
选择"缓存的图片和文件"
点击"清除数据"
```

### 步骤 2：硬刷新页面
```
Ctrl+F5 或 Ctrl+Shift+R
```

### 步骤 3：使用诊断工具
访问 `test-wasm-loading.html`，查看详细的诊断信息。

### 步骤 4：提供以下信息
如果问题持续，请提供：

1. **诊断工具结果：**
   - 访问 `test-wasm-loading.html`
   - 点击"下载日志"
   - 发送日志文件

2. **控制台完整输出：**
   - 按 `F12`
   - Console 标签的所有内容（截图）

3. **Network 标签信息：**
   - 按 `F12`
   - Network 标签
   - 刷新页面
   - 截图显示所有文件的加载状态

## 📋 技术细节（供参考）

### 为什么需要导出这些函数？

**EXPORTED_RUNTIME_METHODS:**
- Emscripten 默认不导出运行时方法以减小文件大小
- 我们的代码需要 `ccall` 来从 JavaScript 调用 C++
- 需要 `allocateUTF8` 来传递字符串

**EXPORTED_FUNCTIONS:**
- C++ 函数默认不对 JavaScript 可见
- 虽然使用了 `EMSCRIPTEN_KEEPALIVE`，但还需要显式导出
- 函数名前需要加下划线（C 命名约定）

### webserialport.cpp 的工作原理

```
JavaScript (Web Serial API)
    ↓
EM_JS 宏（JavaScript 代码）
    ↓
Module.ccall('webserial_onOpened_c')  ← 需要导出 ccall
    ↓
C++ 函数 webserial_onOpened_c()       ← 需要导出函数
    ↓
WebSerialPort 类方法
    ↓
Qt 信号（readyRead, errorOccurred）
```

## 📚 相关文档

- `WASM_EMVAL_FIX.md` - 详细的技术说明（英文）
- `WASM_LOADING_SOLUTION.md` - 加载问题完整解决方案
- `浏览器加载问题解决方案.md` - 之前的诊断指南

---

**更新时间：** 2025-02-03  
**状态：** 已修复，需要重新构建  
**关键文件：**
- ✅ `index.html` - 已更新
- ✅ `CMakeLists.txt` - 已更新
- ⏳ 需要重新构建 WebAssembly

**推荐操作：** 提交代码，等待 GitHub Actions 自动构建，然后测试新版本。

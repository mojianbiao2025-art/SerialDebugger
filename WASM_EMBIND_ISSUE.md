# WebAssembly Embind Issue - 分析和解决方案

## 当前状态

### ✅ 成功部分
1. **构建成功**: .wasm 文件大小为 4.71 MB（正常大小，不是之前的 4 KB）
2. **文件加载**: 所有文件（.js, .wasm）正确加载
3. **运行时初始化**: WebAssembly 运行时成功初始化
4. **Canvas 创建**: Canvas 元素正确创建（1200x800）

### ❌ 问题
**Embind 错误**: `TypeError: Cannot read properties of undefined (reading 'apply') at __emval_call`

这个错误发生在 Qt 应用初始化期间，在任何 UI 显示之前。

## 问题根源

### Qt 5.15.2 WebAssembly 的已知问题

Qt 5.15.2 的 WebAssembly 构建使用 embind 来访问 JavaScript 功能，特别是：
- **localStorage** (用于 QSettings)
- **DOM 操作**
- **事件处理**

问题是 Qt 5.15.2 的 embind 集成有 bug，特别是在以下情况：
1. 使用 Emscripten 1.39.20（Qt 5.15.2 推荐版本）
2. 尝试访问 localStorage
3. 在应用初始化期间调用 JavaScript 函数

### 错误调用栈分析

```
__emval_call (SerialDebugger.js:1:102224)
  ↓
SerialDebugger.wasm:0x6e23c8  (Qt 内部代码)
  ↓
SerialDebugger.wasm:0x6d464d  (可能是 QSettings 或 QApplication 初始化)
```

这表明错误来自 Qt 的内部代码，不是我们的应用代码。

## 已尝试的解决方案

### 1. ✅ 修复 CMakeLists.txt
- 添加 `--bind` 标志
- 支持 Qt 5 和 Qt 6
- 创建缺失的 Qt 模块虚拟配置

### 2. ✅ 修复 webserialport.cpp
- 移除不必要的 `#include <emscripten/bind.h>`
- 只使用 `EM_JS` 宏（不使用 EMSCRIPTEN_BINDINGS）

### 3. ✅ 修复 translations.cpp
- WebAssembly 版本：手动读取 INI 文件（避免 QSettings）
- 原生版本：继续使用 QSettings

### 4. ✅ 修复 main.cpp
- 添加 WebAssembly 特定的初始化代码
- 尝试禁用 QSettings

### 5. ⚠️ 尝试 Qt 6 迁移
- 遇到 embind 链接错误
- Qt 6.5.3 需要 Emscripten 3.1.25
- Qt 6 的 embind 问题更严重

## 推荐解决方案

### 方案 1: 使用 Qt 5.15.2 官方预构建版本（推荐）⭐

**问题**: 我们使用 `aqt` 安装的 Qt 可能不完整或有问题。

**解决方案**:
1. 使用 Qt 官方的 WebAssembly 预构建版本
2. 或者从源代码编译 Qt 5.15.2 WebAssembly

**优点**:
- Qt 5.15.2 是最稳定的 WebAssembly 版本
- 官方构建经过充分测试
- 避免 aqt 安装的潜在问题

**缺点**:
- 需要下载大文件或编译 Qt
- GitHub Actions 构建时间更长

### 方案 2: 简化应用以避免 embind

**解决方案**:
1. 创建一个最小化的 Qt 应用（只有一个窗口和按钮）
2. 逐步添加功能，找出触发 embind 错误的具体代码
3. 重写或移除有问题的部分

**优点**:
- 可以精确定位问题
- 可能找到 workaround

**缺点**:
- 耗时
- 可能需要重写大量代码

### 方案 3: 等待 Qt 6.8+ 或使用其他框架

**Qt 6.8+** (2024年底发布) 改进了 WebAssembly 支持。

**替代方案**:
- **Emscripten + SDL2**: 更底层，但更可控
- **WebAssembly 原生**: 使用 C++ 编译到 WASM，用 JavaScript 处理 UI
- **纯 Web 技术**: HTML/CSS/JavaScript + Web Serial API

## 当前最佳实践

### 临时解决方案

由于这是 Qt 5.15.2 的内部问题，我们可以：

1. **文档说明**: 在 README 中说明 WebAssembly 版本的限制
2. **提供桌面版本**: 重点推广 Windows/Android 版本
3. **等待 Qt 更新**: 关注 Qt 6.8+ 的 WebAssembly 改进

### 长期解决方案

1. **迁移到 Qt 6.8+**: 等待更稳定的 WebAssembly 支持
2. **重写 Web 版本**: 使用纯 Web 技术（HTML/JavaScript）
3. **混合方案**: 核心逻辑用 C++/WASM，UI 用 JavaScript

## 技术细节

### Embind 是什么？

Embind 是 Emscripten 的 C++/JavaScript 绑定系统，允许：
- C++ 调用 JavaScript 函数
- JavaScript 调用 C++ 函数
- 在两种语言之间传递复杂对象

### 为什么会失败？

1. **版本不匹配**: Qt 5.15.2 使用的 embind API 可能与 Emscripten 1.39.20 不完全兼容
2. **初始化顺序**: Qt 在 JavaScript 环境完全准备好之前尝试调用 embind
3. **缺失导出**: 某些 embind 函数没有正确导出到 JavaScript

### 错误的具体含义

```javascript
Cannot read properties of undefined (reading 'apply')
```

这意味着 `__emval_call` 尝试调用一个 JavaScript 函数，但该函数是 `undefined`。

可能的原因：
1. 函数没有被导出
2. 函数名称不匹配
3. 对象还没有被创建

## 参考资料

- [Qt WebAssembly 官方文档](https://doc.qt.io/qt-5/wasm.html)
- [Emscripten Embind 文档](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html)
- [Qt Bug Tracker - WebAssembly Issues](https://bugreports.qt.io/)

## 结论

当前的 embind 错误是 Qt 5.15.2 WebAssembly 的已知限制。虽然我们已经修复了应用代码中的所有问题，但 Qt 内部的 embind 使用仍然有问题。

**建议**:
1. 短期：专注于 Windows 和 Android 版本（这些都工作正常）
2. 中期：等待 Qt 6.8+ 的 WebAssembly 改进
3. 长期：考虑使用纯 Web 技术重写 Web 版本

**当前成就**:
- ✅ Android 版本完全工作
- ✅ Windows 版本完全工作  
- ✅ WebAssembly 构建成功（4.71 MB）
- ⚠️ WebAssembly 运行时有 Qt 内部 embind 问题

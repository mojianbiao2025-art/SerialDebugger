# WebAssembly Emval 错误修复

## 🔍 问题诊断

### 错误信息
```
TypeError: Cannot read properties of undefined (reading 'apply')
at __emval_call (SerialDebugger.js:1:102218)
```

### 根本原因
WebAssembly 模块没有正确导出运行时方法和 C++ 函数，导致 JavaScript 无法调用 C++ 代码。

## 🐛 具体问题

### 问题 1：Module.canvas 已弃用
```
Module.canvas is deprecated. Instead, set Module.qtCanvasElements to be an array
```

**原因：** Qt for WebAssembly 更新了 API，不再使用 `Module.canvas`

**修复：** 在 `index.html` 中使用 `Module.qtCanvasElements`

### 问题 2：ccall 未定义
```
TypeError: Cannot read properties of undefined (reading 'apply')
```

**原因：** `Module.ccall` 没有被导出，JavaScript 无法调用 C++ 函数

**修复：** 在 `CMakeLists.txt` 中添加 `EXPORTED_RUNTIME_METHODS`

### 问题 3：C++ 函数未导出
```
at webserial_onOpened_c
at webserial_onError_c
at webserial_onDataReceived_c
```

**原因：** 这些 C++ 函数虽然标记了 `EMSCRIPTEN_KEEPALIVE`，但没有在链接时显式导出

**修复：** 在 `CMakeLists.txt` 中添加 `EXPORTED_FUNCTIONS`

## ✅ 已实施的修复

### 修复 1：更新 index.html

**修改前：**
```javascript
var Module = {
    canvas: (function() {
        var canvas = document.getElementById('canvas');
        // ...
        return canvas;
    })(),
    // ...
};
```

**修改后：**
```javascript
var canvas = document.getElementById('canvas');
// ...

var Module = {
    qtCanvasElements: [canvas],  // 使用新的 API
    // ...
};
```

### 修复 2：更新 CMakeLists.txt

**修改前：**
```cmake
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind"
)
```

**修改后：**
```cmake
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','allocateUTF8'] -s EXPORTED_FUNCTIONS=['_malloc','_free','_webserial_onOpened_c','_webserial_onError_c','_webserial_onDataReceived_c']"
)
```

## 📋 导出的函数说明

### EXPORTED_RUNTIME_METHODS
这些是 Emscripten 运行时提供的 JavaScript 函数：

- **ccall**: 从 JavaScript 调用 C++ 函数
  ```javascript
  Module.ccall('functionName', 'returnType', ['argType'], [argValue]);
  ```

- **cwrap**: 包装 C++ 函数为 JavaScript 函数
  ```javascript
  var func = Module.cwrap('functionName', 'returnType', ['argType']);
  ```

- **allocateUTF8**: 在 WebAssembly 内存中分配 UTF-8 字符串
  ```javascript
  var ptr = allocateUTF8("Hello");
  ```

### EXPORTED_FUNCTIONS
这些是我们的 C++ 函数，需要从 JavaScript 调用：

- **_malloc**: 分配内存
- **_free**: 释放内存
- **_webserial_onOpened_c**: 串口打开回调
- **_webserial_onError_c**: 串口错误回调
- **_webserial_onDataReceived_c**: 串口数据接收回调

注意：C++ 函数名前需要加下划线 `_`

## 🔄 webserialport.cpp 中的调用流程

### JavaScript → C++ 调用
```javascript
// 在 EM_JS 中调用 C++ 函数
Module.ccall('webserial_onOpened_c', 'void', [], []);
Module.ccall('webserial_onError_c', 'void', ['number'], [errorPtr]);
Module.ccall('webserial_onDataReceived_c', 'void', ['number', 'number'], [dataPtr, length]);
```

### C++ 函数定义
```cpp
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void webserial_onOpened_c() {
        webserial_onOpened();
    }

    EMSCRIPTEN_KEEPALIVE
    void webserial_onError_c(const char* error) {
        webserial_onError(error);
    }

    EMSCRIPTEN_KEEPALIVE
    void webserial_onDataReceived_c(const char* data, int length) {
        webserial_onDataReceived(data, length);
    }
}
```

## 🧪 验证修复

### 重新构建
```bash
# 清理旧的构建
rm -rf build-wasm

# 重新构建
source ~/emsdk/emsdk_env.sh
mkdir build-wasm && cd build-wasm

# 配置（会使用新的 LINK_FLAGS）
emcmake cmake .. -G Ninja

# 构建
ninja

# 检查生成的文件
ls -lh bin/SerialDebugger.*
```

### 验证导出
```bash
# 检查 SerialDebugger.js 是否包含导出的函数
grep -o "ccall" bin/SerialDebugger.js | head -5
grep -o "cwrap" bin/SerialDebugger.js | head -5
grep -o "allocateUTF8" bin/SerialDebugger.js | head -5
```

### 测试加载
```bash
# 启动 HTTP 服务器
cd bin
python -m http.server 8000

# 访问
# http://localhost:8000/index.html
```

### 检查控制台
应该看到：
```
✓ 使用 http: 协议
✓ 浏览器支持 Web Serial API
✓ SerialDebugger.js 找到
✓ SerialDebugger.wasm 找到
✓ WebAssembly 运行时初始化成功！
✓ 应用程序加载完成！
```

**不应该再看到：**
```
❌ Module.canvas is deprecated
❌ TypeError: Cannot read properties of undefined (reading 'apply')
```

## 📝 GitHub Actions 更新

修改会自动应用到 GitHub Actions 构建中，因为：
1. `.github/workflows/build-all.yml` 使用 `emcmake cmake ..`
2. CMake 会读取更新后的 `CMakeLists.txt`
3. 新的 `LINK_FLAGS` 会自动应用

### 触发新构建
```bash
git add CMakeLists.txt index.html
git commit -m "Fix: Export runtime methods and functions for WebAssembly"
git push
```

## 🎯 预期结果

### 修复前
- ❌ 页面加载但应用不显示
- ❌ 控制台显示 `Module.canvas is deprecated`
- ❌ 控制台显示 `TypeError: Cannot read properties of undefined`
- ❌ 串口功能无法使用

### 修复后
- ✅ 页面正常加载
- ✅ Qt 应用程序界面显示
- ✅ 没有弃用警告
- ✅ 没有 TypeError 错误
- ✅ 串口功能可以正常使用（需要用户授权）

## 🔧 其他可能需要的修复

### 如果仍然有问题

#### 选项 1：增加更多导出
如果还有其他函数调用失败，可以添加更多导出：
```cmake
LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind \
    -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','allocateUTF8','UTF8ToString'] \
    -s EXPORTED_FUNCTIONS=['_malloc','_free','_webserial_onOpened_c','_webserial_onError_c','_webserial_onDataReceived_c'] \
    -s MODULARIZE=1 \
    -s EXPORT_NAME='createModule'"
```

#### 选项 2：使用 EMSCRIPTEN_BINDINGS
如果 `ccall` 方式不稳定，可以改用 embind：
```cpp
#include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(webserial) {
    emscripten::function("webserial_onOpened", &webserial_onOpened);
    emscripten::function("webserial_onError", &webserial_onError);
    emscripten::function("webserial_onDataReceived", &webserial_onDataReceived);
}
```

然后在 JavaScript 中：
```javascript
Module.webserial_onOpened();
Module.webserial_onError(errorMsg);
```

#### 选项 3：简化 JavaScript 绑定
如果问题持续，可以简化 `webserialport.cpp` 中的 JavaScript 代码，避免复杂的异步操作。

## 📚 参考资料

- [Emscripten: Interacting with code](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html)
- [Emscripten: embind](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html)
- [Qt for WebAssembly](https://doc.qt.io/qt-5/wasm.html)
- [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API)

---

**修复日期：** 2025-02-03  
**状态：** 已修复，等待重新构建和测试  
**关键修改：**
- `index.html`: 使用 `qtCanvasElements` 替代 `canvas`
- `CMakeLists.txt`: 添加 `EXPORTED_RUNTIME_METHODS` 和 `EXPORTED_FUNCTIONS`

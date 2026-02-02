# WebAssembly构建修复说明

## 问题描述

在构建WebAssembly版本时遇到链接错误：

```
error: undefined symbol: _embind_register_function
error: undefined symbol: _emval_as
error: undefined symbol: _emval_call
...
wasm-ld: error: unable to find library -lembind
```

## 根本原因

`webserialport.cpp` 使用了Emscripten的embind库来实现C++与JavaScript的互操作，但链接时使用了错误的标志。

## 错误的修复方法 ❌

```cmake
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -lembind"  # 错误！
)
```

**为什么错误：**
- `-lembind` 是传统的Unix链接器语法，用于链接名为 `libembind.a` 或 `libembind.so` 的库
- Emscripten不使用这种方式来启用embind
- wasm-ld会尝试查找一个不存在的 `libembind` 库文件

## 正确的修复方法 ✅

```cmake
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind"  # 正确！
)
```

**为什么正确：**
- `--bind` 是Emscripten的特殊标志，专门用于启用embind支持
- 它会自动链接embind库并配置必要的JavaScript绑定
- 这是Emscripten官方文档推荐的方式

## 修改步骤

### 1. 编辑 CMakeLists.txt

找到第122行左右的WebAssembly链接设置：

```cmake
if(EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
    )
    # WebAssembly specific settings - minimal flags for stability
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind"
    )
```

**关键点：**
- 使用 `--bind` 而不是 `-lembind`
- 保持其他标志不变（`-s WASM=1 -s ALLOW_MEMORY_GROWTH=1`）

### 2. 清理旧的构建文件

```bash
rm -rf build-wasm
```

### 3. 重新构建

```bash
./build_wasm.bat
# 或者
bash build_wasm.sh
```

## 验证修复

### 构建应该成功

构建输出应该显示：

```
[1/19] Building CXX object CMakeFiles/SerialDebugger.dir/...
[2/19] Building CXX object CMakeFiles/SerialDebugger.dir/...
...
[19/19] Linking CXX executable bin/SerialDebugger.js
```

**没有错误信息！**

### 检查生成的文件

```bash
ls -lh build-wasm/bin/
```

应该看到：
- `SerialDebugger.js` - JavaScript加载器
- `SerialDebugger.wasm` - WebAssembly二进制文件
- `SerialDebugger.html` - HTML入口文件（如果配置了）

### 测试运行

```bash
cd build-wasm/bin
python3 -m http.server 8000
```

然后在浏览器中打开 `http://localhost:8000/SerialDebugger.html`

## Emscripten embind 详解

### 什么是embind？

embind是Emscripten提供的C++/JavaScript绑定库，允许：
- 从JavaScript调用C++函数
- 从C++调用JavaScript函数
- 在两种语言之间传递复杂数据类型

### webserialport.cpp 如何使用embind

```cpp
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

// 调用JavaScript的Web Serial API
val navigator = val::global("navigator");
val serial = navigator["serial"];

// 请求串口访问
val promise = serial.call<val>("requestPort", options);

// 处理Promise回调
promise.call<val>("then", val::module_property("onPortSelected"));
```

### 为什么需要 --bind 标志

`--bind` 标志告诉Emscripten：
1. 包含embind运行时库
2. 生成必要的JavaScript绑定代码
3. 导出C++函数供JavaScript调用
4. 设置正确的内存管理

没有这个标志，所有embind相关的符号都会是未定义的。

## 其他Emscripten标志说明

### -s WASM=1
启用WebAssembly输出（而不是asm.js）

### -s ALLOW_MEMORY_GROWTH=1
允许WebAssembly内存动态增长，对于Qt应用是必需的

### 可选的其他标志

如果需要更多功能，可以添加：

```cmake
LINK_FLAGS "-s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --bind -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=4"
```

- `-s USE_PTHREADS=1` - 启用多线程支持
- `-s PTHREAD_POOL_SIZE=4` - 线程池大小

**注意：** Qt for WebAssembly 5.15.2默认不支持多线程，所以不需要这些标志。

## 常见问题

### Q: 为什么不能用 -lembind？

**A:** Emscripten的embind不是一个传统的库文件。它是编译器的一部分，通过 `--bind` 标志激活。

### Q: 如果还是出现链接错误怎么办？

**A:** 检查以下几点：
1. 确认使用的是最新的Emscripten版本
2. 清理所有构建缓存：`rm -rf build-wasm CMakeCache.txt`
3. 检查webserialport.cpp是否正确包含了embind头文件
4. 确认CMakeLists.txt中的LINK_FLAGS没有拼写错误

### Q: --bind 和 --embind 有什么区别？

**A:** 它们是同一个东西！`--bind` 是 `--embind` 的简写形式，两者完全等价。

### Q: 可以同时使用 --bind 和其他绑定库吗？

**A:** 可以。例如：
```cmake
LINK_FLAGS "-s WASM=1 --bind -lwebsocket.js"
```

## 调试技巧

### 查看链接器命令

```bash
cd build-wasm
VERBOSE=1 ninja
```

这会显示完整的链接器命令，可以看到 `--bind` 是否被正确传递。

### 检查符号

```bash
wasm-objdump -x SerialDebugger.wasm | grep embind
```

如果 `--bind` 正确应用，应该能看到embind相关的导入和导出。

### 启用Emscripten调试输出

```bash
export EMCC_DEBUG=1
ninja
```

这会显示Emscripten编译器的详细信息。

## 总结

| 方法 | 语法 | 结果 |
|------|------|------|
| ❌ 错误 | `-lembind` | 链接失败，找不到库 |
| ✅ 正确 | `--bind` | 成功启用embind支持 |
| ✅ 也正确 | `--embind` | 与 `--bind` 完全相同 |

**记住：** 在Emscripten中，使用 `--bind` 来启用embind，而不是 `-lembind`！

## 参考资料

- [Emscripten embind 官方文档](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html)
- [Emscripten 链接器标志](https://emscripten.org/docs/tools_reference/emcc.html#emcc-linker-flags)
- [Qt for WebAssembly 文档](https://doc.qt.io/qt-5/wasm.html)

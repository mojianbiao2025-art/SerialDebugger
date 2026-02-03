# WebAssembly工作流修复说明

## 问题诊断

### 错误现象
```
SyntaxError: Unexpected token (1616:757)
function js_openSerialPort(baudRate,dataBits,stopBits,parity){ 
  if (!navigator.serial) { 
    console.error('Web Serial API not supported'); 
    return; 
  } 
  navigator.serial.requestPort() 
    .then(port => { ... })  // 这里使用了箭头函数
    ...
  async function startReading() { ... }  // 这里使用了async/await
}
```

### 根本原因
**Emscripten版本不匹配！**

| 工作流文件 | 原始版本 | 状态 | 原因 |
|-----------|---------|------|------|
| `wasm-build-success.yml` | **1.39.20** | ✅ 成功 | 支持现代JavaScript语法 |
| `build-all.yml` | 1.39.8 | ❌ 失败 | 不支持async/await |
| `wasm-build.yml` | 2.0.14 | ❌ 失败 | 版本太新，可能有兼容性问题 |

**关键发现：**
- `webserialport.cpp`中的JavaScript代码使用了ES6+特性：
  - 箭头函数 `() =>`
  - `async/await`
  - Promise链式调用
- Emscripten 1.39.8的acorn解析器不支持这些语法
- Emscripten 1.39.20支持这些语法

## 修复方案

### 修改1：build-all.yml
```yaml
# 修改前
./emsdk install 1.39.8
./emsdk activate 1.39.8

# 修改后
./emsdk install 1.39.20
./emsdk activate 1.39.20
```

### 修改2：wasm-build.yml
```yaml
# 修改前
./emsdk install 2.0.14
./emsdk activate 2.0.14

# 修改后
./emsdk install 1.39.20
./emsdk activate 1.39.20
```

### 修改3：统一dummy config创建方式
两个文件都改为使用与`wasm-build-success.yml`相同的方式创建dummy配置：

```bash
# 不再使用heredoc方式
cat > file << 'EOF'
...
EOF

# 改为使用echo追加方式
echo 'add_library(Qt5::Svg INTERFACE IMPORTED)' > file
echo 'set(Qt5Svg_FOUND TRUE)' >> file
```

这样可以避免shell脚本中的引号和变量展开问题。

## 关键差异对比

### Emscripten版本对比

| 特性 | 1.39.8 | 1.39.20 | 2.0.14 |
|------|--------|---------|--------|
| ES6箭头函数 | ❌ | ✅ | ✅ |
| async/await | ❌ | ✅ | ✅ |
| Promise | ⚠️ 部分 | ✅ | ✅ |
| Qt 5.15.2兼容 | ✅ | ✅ | ⚠️ 可能有问题 |
| 构建稳定性 | ✅ | ✅ | ⚠️ 未知 |

### Dummy Config创建方式对比

**方式1：heredoc（原build-all.yml）**
```bash
cat > $QT_ROOT/lib/cmake/Qt5$MODULE/Qt5${MODULE}Config.cmake << 'EOFCONFIG'
add_library(Qt5::$MODULE INTERFACE IMPORTED)
set(Qt5${MODULE}_FOUND TRUE)
EOFCONFIG
```
❌ 问题：变量展开可能有问题

**方式2：echo追加（wasm-build-success.yml）**
```bash
echo 'add_library(Qt5::Svg INTERFACE IMPORTED)' > file
echo 'set(Qt5Svg_FOUND TRUE)' >> file
```
✅ 优点：简单直接，不会有引号问题

## 验证修复

### 检查点1：Emscripten版本
构建日志应该显示：
```
./emsdk install 1.39.20
./emsdk activate 1.39.20
```

### 检查点2：编译成功
应该看到：
```
[19/19] Linking CXX executable bin/SerialDebugger.js
cache:INFO: generating system library: libembind-rtti.a...
```

注意：`libembind-rtti.a`的生成说明embind正确链接。

### 检查点3：没有JavaScript语法错误
不应该再看到：
```
SyntaxError: Unexpected token
```

## 三个工作流文件的用途

### 1. wasm-build-success.yml
- **用途：** 参考模板，已验证可以成功构建
- **触发：** 手动或push到main
- **特点：** 使用Emscripten 1.39.20

### 2. build-all.yml
- **用途：** 同时构建Android和WebAssembly
- **触发：** push、PR、手动
- **特点：** 
  - 先构建Android APK
  - 再构建WebAssembly
  - 部署到GitHub Pages

### 3. wasm-build.yml
- **用途：** 仅构建WebAssembly（手动触发）
- **触发：** 仅手动（workflow_dispatch）
- **特点：** 
  - 不自动触发
  - 用于测试或单独构建WASM

## 推荐的工作流策略

### 日常开发
```yaml
# 使用 build-all.yml
on:
  push:
    branches: [ main ]
```
- 自动构建Android和WebAssembly
- 自动部署到GitHub Pages

### 测试WASM
```yaml
# 使用 wasm-build.yml
on:
  workflow_dispatch:
```
- 手动触发
- 快速测试WASM构建

### 参考模板
```yaml
# 保留 wasm-build-success.yml
```
- 作为已验证的参考
- 不自动触发

## 常见问题

### Q1: 为什么不使用最新的Emscripten？
**A:** 
- Emscripten 2.x可能与Qt 5.15.2有兼容性问题
- 1.39.20是经过验证的稳定版本
- 如果要升级，需要同时测试Qt兼容性

### Q2: 如果还是出现JavaScript错误怎么办？
**A:** 检查`webserialport.cpp`中的JavaScript代码：
```cpp
EM_ASM({
  // 确保这里的JavaScript语法是ES5兼容的
  // 或者使用Emscripten 1.39.20+
});
```

### Q3: 为什么成功的工作流使用echo而不是heredoc？
**A:** 
- echo方式更简单
- 避免shell变量展开问题
- 更容易调试

### Q4: 可以删除wasm-build-success.yml吗？
**A:** 
- 建议保留作为参考
- 或者重命名为 `wasm-build-success.yml.bak`
- 它是唯一验证成功的配置

## 修复后的文件状态

### ✅ build-all.yml
- Emscripten版本：1.39.20
- Dummy config方式：echo追加
- 状态：应该可以成功构建

### ✅ wasm-build.yml
- Emscripten版本：1.39.20
- Dummy config方式：echo追加
- 状态：应该可以成功构建

### ✅ wasm-build-success.yml
- 保持不变
- 作为参考模板

## 下一步

1. **提交修改：**
   ```bash
   git add .github/workflows/build-all.yml
   git add .github/workflows/wasm-build.yml
   git commit -m "Fix: Use Emscripten 1.39.20 for WebAssembly builds"
   git push
   ```

2. **验证构建：**
   - 检查GitHub Actions是否成功
   - 查看构建日志确认使用1.39.20
   - 测试生成的WebAssembly文件

3. **清理（可选）：**
   ```bash
   # 如果build-all.yml成功，可以考虑删除或禁用wasm-build.yml
   # 因为build-all.yml已经包含了WASM构建
   ```

---

**修复日期：** 2024  
**关键版本：** Emscripten 1.39.20  
**状态：** 已修复，等待验证

# Qt 6 迁移指南

## 🎯 迁移概述

已将项目从 Qt 5.15.2 迁移到 Qt 6.5.3，以获得更好的 WebAssembly 支持。

## ✅ 已完成的更改

### 1. CMakeLists.txt

**主要变更：**
- 移除了 Qt 5/Qt 6 自动检测逻辑
- 直接使用 Qt 6
- 使用 `qt_add_executable()` 替代 `add_executable()`
- 简化了 WebAssembly 配置
- 使用 Qt 6 的内置 WebAssembly 支持

**关键配置：**
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_add_executable(SerialDebugger ${PROJECT_SOURCES})

target_link_libraries(SerialDebugger PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Qt 6 WebAssembly 设置
set_target_properties(SerialDebugger PROPERTIES
    QT_WASM_PTHREAD_POOL_SIZE 0
    QT_WASM_INITIAL_MEMORY 128MB
)
```

### 2. GitHub Actions 工作流

**主要变更：**
- Emscripten 版本：1.39.20 → 3.1.50
- Qt 版本：5.15.2 → 6.5.3
- Qt 包名：wasm_32 → wasm_singlethread
- 使用 `qt-cmake` 替代 `emcmake cmake`
- 移除了所有 dummy config 创建代码

**新的构建流程：**
```bash
# 安装 Emscripten 3.1.50
./emsdk install 3.1.50
./emsdk activate 3.1.50

# 安装 Qt 6.5.3 for WebAssembly
aqt install-qt linux desktop 6.5.3 wasm_singlethread

# 使用 qt-cmake 构建
qt-cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## 📋 代码兼容性

### 不需要修改的代码

大部分代码与 Qt 6 兼容，包括：
- 基本的 Qt 控件（QWidget, QLabel, QPushButton 等）
- 布局管理（QVBoxLayout, QHBoxLayout 等）
- 信号槽机制
- 事件处理

### 可能需要注意的变更

#### 1. 头文件包含

Qt 6 更严格，可能需要显式包含：
```cpp
// Qt 5 可能隐式包含
#include <QWidget>

// Qt 6 可能需要显式包含
#include <QWidget>
#include <QString>
#include <QList>
```

#### 2. QString 分割

```cpp
// Qt 5
QStringList list = str.split(",", QString::SkipEmptyParts);

// Qt 6
QStringList list = str.split(",", Qt::SkipEmptyParts);
```

#### 3. QVector → QList

Qt 6 中 QVector 是 QList 的别名，推荐使用 QList：
```cpp
// Qt 5
QVector<int> vec;

// Qt 6（推荐）
QList<int> list;
```

#### 4. QRegExp → QRegularExpression

Qt 6 移除了 QRegExp，使用 QRegularExpression：
```cpp
// Qt 5
QRegExp rx("pattern");

// Qt 6
QRegularExpression rx("pattern");
```

## 🔧 WebAssembly 特定变更

### Qt 5 vs Qt 6 WebAssembly

| 特性 | Qt 5.15.2 | Qt 6.5.3 |
|------|-----------|----------|
| Emscripten | 1.39.x | 3.1.x |
| 线程支持 | 有限 | 更好 |
| 文件系统 | 需要手动配置 | 内置支持 |
| WebGL | 需要手动标志 | 自动配置 |
| 构建工具 | emcmake | qt-cmake |
| 稳定性 | 一般 | 更好 |

### Qt 6 WebAssembly 优势

1. **更好的稳定性**
   - 更少的已知问题
   - 更好的错误处理

2. **更简单的配置**
   - 不需要手动设置 Emscripten 标志
   - 自动处理 WebGL 配置

3. **更好的性能**
   - 优化的渲染管线
   - 更小的二进制文件

4. **更好的文档**
   - 官方文档更完善
   - 社区支持更好

## 🚀 构建和测试

### 本地构建（如果需要）

#### Windows
```bash
# 安装 Qt 6.5.3 for WebAssembly
# 从 Qt 官网下载安装器

# 设置环境
set PATH=C:\Qt\6.5.3\wasm_singlethread\bin;%PATH%
set PATH=C:\emsdk;%PATH%
call emsdk_env.bat

# 构建
mkdir build && cd build
qt-cmake ..
cmake --build .
```

#### Linux/Mac
```bash
# 安装 Qt 6.5.3 for WebAssembly
pip3 install aqtinstall
aqt install-qt linux desktop 6.5.3 wasm_singlethread

# 安装 Emscripten
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 3.1.50
./emsdk activate 3.1.50
source ./emsdk_env.sh

# 构建
mkdir build && cd build
qt-cmake ..
cmake --build .
```

### GitHub Actions 构建

提交代码后，GitHub Actions 会自动：
1. 安装 Qt 6.5.3 for WebAssembly
2. 安装 Emscripten 3.1.50
3. 构建 WebAssembly 应用
4. 部署到 GitHub Pages

## 📊 预期改进

### 文件大小

Qt 6 WebAssembly 应用通常比 Qt 5 更小：
- Qt 5: 4-6 MB
- Qt 6: 3-5 MB

### 加载速度

Qt 6 的优化应该带来：
- 更快的初始化
- 更快的渲染
- 更好的响应性

### 兼容性

Qt 6 对现代浏览器的支持更好：
- Chrome 89+
- Edge 89+
- Safari 15+
- Firefox 90+

## 🐛 故障排除

### 问题 1：找不到 Qt 6

**错误：**
```
CMake Error: Could not find Qt6
```

**解决：**
确保正确设置了 CMAKE_PREFIX_PATH：
```bash
export CMAKE_PREFIX_PATH=/path/to/Qt/6.5.3/wasm_singlethread
```

### 问题 2：Emscripten 版本不匹配

**错误：**
```
Emscripten version mismatch
```

**解决：**
确保使用 Emscripten 3.1.50：
```bash
./emsdk install 3.1.50
./emsdk activate 3.1.50
```

### 问题 3：链接错误

**错误：**
```
undefined reference to ...
```

**解决：**
确保所有必要的 Qt 模块都已链接：
```cmake
target_link_libraries(SerialDebugger PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
```

## 📝 下一步

1. **提交代码**
   ```bash
   git add CMakeLists.txt .github/workflows/build-all.yml Qt6迁移指南.md
   git commit -m "Migrate to Qt 6.5.3 for better WebAssembly support"
   git push
   ```

2. **等待构建**
   - 访问 GitHub Actions
   - 等待构建完成（约 15-20 分钟）

3. **测试应用**
   - 清除浏览器缓存
   - 访问 GitHub Pages
   - 验证界面显示

4. **如果成功**
   - 逐步恢复完整功能
   - 从简化版本开始添加功能

## 🎯 成功标志

构建成功后，应该看到：
- ✅ SerialDebugger.wasm 文件大小 3-5 MB（不是 4 KB！）
- ✅ SerialDebugger.js 文件存在
- ✅ 应用在浏览器中正常显示
- ✅ 界面元素可见和可交互

---

**更新时间：** 2025-02-03  
**状态：** 已完成 Qt 6 迁移配置  
**下一步：** 提交代码并测试构建

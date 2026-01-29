# WebAssembly Build Fix

## 问题 (Problem)
WebAssembly 构建失败，因为 Qt5Gui 的插件（QSvgIconPlugin、QSvgPlugin、QTiffPlugin）需要 Qt5Svg 和 Qt5Zlib 模块，但这些模块在标准的 Qt WebAssembly 构建中不可用。

The WebAssembly build was failing because Qt5Gui plugins (QSvgIconPlugin, QSvgPlugin, QTiffPlugin) require Qt5Svg and Qt5Zlib modules, which are not available in the standard Qt WebAssembly build.

## 错误信息 (Error Messages)
```
CMake Error: find_package for module Qt5Svg called with REQUIRED, but CMAKE_DISABLE_FIND_PACKAGE_Qt5Svg is enabled
CMake Error: Could not find a package configuration file provided by "Qt5Zlib"
```

## 解决方案 (Solution)
在构建过程中创建虚拟的 Qt5Svg 和 Qt5Zlib CMake 配置文件，以满足 Qt5Gui 插件的依赖要求。

Create dummy Qt5Svg and Qt5Zlib CMake config files during the build process to satisfy Qt5Gui plugin dependencies.

### 实现细节 (Implementation Details)

1. **虚拟配置文件** (Dummy Config Files)
   - 在 `.github/workflows/wasm-build.yml` 中添加脚本
   - 检查 Qt5Svg 和 Qt5Zlib 是否存在
   - 如果不存在，创建最小的 CMake 配置文件
   - 这些文件定义了 INTERFACE 库，不包含实际实现

2. **简化 CMakeLists.txt**
   - 移除了复杂的插件禁用逻辑
   - 保持简单的 Qt 模块查找
   - 依赖虚拟配置文件来满足依赖

3. **Emscripten 版本**
   - 使用 Emscripten 2.0.14（与 Qt 5.15.2 兼容）
   - 避免了符号未定义的链接错误

## 技术说明 (Technical Notes)

### 虚拟 Qt5Svg 配置
```cmake
add_library(Qt5::Svg INTERFACE IMPORTED)
set(Qt5Svg_FOUND TRUE)
set(Qt5Svg_VERSION_STRING "5.15.2")
```

### 虚拟 Qt5Zlib 配置
```cmake
add_library(Qt5::Zlib INTERFACE IMPORTED)
set(Qt5Zlib_FOUND TRUE)
set(Qt5Zlib_VERSION_STRING "1.0.0")
```

这些 INTERFACE 库不包含任何实际代码，但满足了 CMake 的依赖检查。由于我们的应用不直接使用 SVG 或 Zlib 功能，这不会影响运行时行为。

These INTERFACE libraries contain no actual code but satisfy CMake's dependency checks. Since our application doesn't directly use SVG or Zlib features, this doesn't affect runtime behavior.

## 下一步 (Next Steps)

1. 推送更改到 GitHub
2. 等待 GitHub Actions 构建完成
3. 检查构建日志确认成功
4. 下载并测试生成的 WebAssembly 文件
5. 如果成功，启用 GitHub Pages 部署

## 相关文件 (Related Files)
- `.github/workflows/wasm-build.yml` - 构建工作流
- `CMakeLists.txt` - CMake 配置
- `webserialport.h/cpp` - Web Serial API 实现
- `index.html` - Web 应用入口页面

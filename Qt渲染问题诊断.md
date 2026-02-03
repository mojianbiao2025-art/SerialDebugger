# Qt WebAssembly 渲染问题诊断

## 🔍 问题现象

- ✅ WebAssembly 加载完成
- ✅ Canvas 有正确的尺寸 (1200x800)
- ✅ 没有 JavaScript 错误
- ❌ Qt 应用界面元素不显示（空白）

## 🐛 可能的原因

### 原因 1：Qt 应用未正确初始化
Qt for WebAssembly 需要特定的初始化流程，可能因为以下原因失败：
- QApplication 未正确创建
- 主窗口未正确显示
- 事件循环未启动

### 原因 2：资源文件未加载
Qt 应用依赖的资源可能未正确打包或加载：
- .qrc 资源文件
- 样式表 (styles.qss)
- 语言文件 (lang_*.ini)
- 图标文件

### 原因 3：Canvas 渲染上下文问题
- WebGL 上下文未正确初始化
- Qt 无法获取渲染上下文
- 渲染缓冲区问题

### 原因 4：Emscripten 链接标志不完整
可能缺少必要的 Emscripten 标志：
- FORCE_FILESYSTEM=1 (文件系统支持)
- FETCH=1 (网络请求支持)
- 其他 Qt 特定标志

## ✅ 已实施的修复

### 修复 1：添加文件系统支持
在 CMakeLists.txt 中添加：
```cmake
-s FORCE_FILESYSTEM=1 -s FETCH=1
```

### 修复 2：增强调试信息
在 index.html 中添加：
- Canvas 内容检测
- Qt 初始化状态检查
- 强制重绘尝试

### 修复 3：创建诊断工具
创建 `test-qt-render.html` 用于：
- 检查 Canvas 状态
- 检查 Module 配置
- 检测渲染内容
- 强制重绘测试

## 🔧 诊断步骤

### 步骤 1：使用渲染测试页面

1. 重新构建并部署：
```bash
# 提交修改
git add CMakeLists.txt index.html test-qt-render.html
git commit -m "Add filesystem support and rendering diagnostics"
git push

# 等待 GitHub Actions 构建完成
```

2. 访问测试页面：
```
https://mojianbiao2025-art.github.io/SerialDebugger/test-qt-render.html
```

3. 点击"检查 Canvas"按钮，查看：
   - Canvas 尺寸是否正确
   - Canvas 内容是否有像素数据
   - WebGL 是否可用

4. 点击"检查 Module"按钮，查看：
   - Module 对象是否存在
   - qtCanvasElements 是否设置
   - ccall 函数是否可用

### 步骤 2：检查浏览器控制台

查找以下信息：

**正常情况应该看到：**
```
[时间] ✓ WebAssembly 运行时初始化成功！
[时间] Canvas 元素: 已找到
[时间] Canvas 尺寸: 1200x800
[时间] Canvas 可见: 是
[时间] Canvas 内容: 有内容
```

**如果看到以下信息，说明有问题：**
```
[时间] Canvas 内容: 空白
[时间] Canvas 内容: 完全透明
```

### 步骤 3：检查 Network 标签

确认以下文件都已加载：
- ✅ SerialDebugger.js (200 OK)
- ✅ SerialDebugger.wasm (200 OK)
- ✅ SerialDebugger.data (200 OK) - 如果存在
- ✅ lang_zh.ini (200 OK)
- ✅ styles.qss (200 OK)

## 🔍 深入诊断

### 检查 1：Qt 应用是否崩溃

在控制台运行：
```javascript
// 检查 Qt 应用状态
console.log('Module:', Module);
console.log('QApplication:', Module._QApplication);

// 尝试调用 Qt 函数
if (Module.ccall) {
    try {
        Module.ccall('main', 'number', [], []);
    } catch(e) {
        console.error('Qt main 调用失败:', e);
    }
}
```

### 检查 2：Canvas 渲染测试

在控制台运行：
```javascript
var canvas = document.getElementById('canvas');
var ctx = canvas.getContext('2d');

// 手动绘制测试
ctx.fillStyle = 'red';
ctx.fillRect(0, 0, 100, 100);

// 检查是否可见
console.log('手动绘制的红色方块是否可见？');
```

如果红色方块可见，说明 Canvas 本身没问题，是 Qt 渲染的问题。

### 检查 3：WebGL 测试

```javascript
var canvas = document.getElementById('canvas');
var gl = canvas.getContext('webgl');

if (gl) {
    console.log('WebGL 版本:', gl.getParameter(gl.VERSION));
    console.log('WebGL 供应商:', gl.getParameter(gl.VENDOR));
    console.log('WebGL 渲染器:', gl.getParameter(gl.RENDERER));
    
    // 测试清屏
    gl.clearColor(0, 1, 0, 1); // 绿色
    gl.clear(gl.COLOR_BUFFER_BIT);
    console.log('WebGL 清屏测试：应该看到绿色');
} else {
    console.error('WebGL 不可用！');
}
```

## 🚨 常见问题和解决方案

### 问题 1：Canvas 完全空白，没有任何内容

**可能原因：**
- Qt 应用崩溃
- 主窗口未显示
- 渲染循环未启动

**解决方案：**
1. 检查控制台是否有 Qt 错误信息
2. 确认 `w.show()` 在 main.cpp 中被调用
3. 检查是否有资源加载失败

### 问题 2：Canvas 有背景色但没有控件

**可能原因：**
- 样式表问题
- 控件未正确创建
- 布局问题

**解决方案：**
1. 检查 styles.qss 是否加载
2. 检查 mainwindow.ui 是否正确
3. 尝试禁用样式表测试

### 问题 3：WebGL 上下文丢失

**症状：**
```
WebGL context lost
```

**解决方案：**
1. 关闭其他占用 GPU 的标签页
2. 重启浏览器
3. 更新显卡驱动

### 问题 4：内存不足

**症状：**
```
RangeError: WebAssembly.Memory(): could not allocate memory
```

**解决方案：**
1. 关闭其他标签页
2. 增加 ALLOW_MEMORY_GROWTH
3. 减小应用程序大小

## 🔧 可能的修复方案

### 方案 1：简化 main.cpp

创建最小化的测试版本：
```cpp
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QLabel *label = new QLabel("Hello Qt WebAssembly!");
    label->setStyleSheet("QLabel { font-size: 24px; color: red; }");
    label->show();
    
    return a.exec();
}
```

如果这个可以显示，说明是 MainWindow 的问题。

### 方案 2：添加更多 Emscripten 标志

在 CMakeLists.txt 中尝试：
```cmake
-s WASM=1
-s ALLOW_MEMORY_GROWTH=1
-s FORCE_FILESYSTEM=1
-s FETCH=1
-s FULL_ES2=1
-s USE_WEBGL2=1
-s MAX_WEBGL_VERSION=2
-s OFFSCREENCANVAS_SUPPORT=1
```

### 方案 3：使用 qtloader.js

Qt 官方推荐使用 qtloader.js：
```html
<script src="qtloader.js"></script>
<script>
    QtLoader.loadEmscriptenModule("SerialDebugger");
</script>
```

### 方案 4：检查 Qt 版本兼容性

确认使用的 Qt 版本与 Emscripten 版本兼容：
- Qt 5.15.2 + Emscripten 1.39.20 ✅
- Qt 6.x + Emscripten 3.x ✅

## 📊 调试清单

- [ ] Canvas 尺寸正确 (1200x800)
- [ ] Canvas 可见 (display: block)
- [ ] WebGL 可用
- [ ] Module 对象存在
- [ ] qtCanvasElements 已设置
- [ ] ccall 函数可用
- [ ] SerialDebugger.js 加载成功
- [ ] SerialDebugger.wasm 加载成功
- [ ] 没有 JavaScript 错误
- [ ] 没有 WebAssembly 错误
- [ ] 资源文件加载成功
- [ ] Canvas 有像素内容

## 📝 下一步

1. **提交当前修复：**
```bash
git add CMakeLists.txt index.html test-qt-render.html Qt渲染问题诊断.md
git commit -m "Add filesystem support and Qt rendering diagnostics"
git push
```

2. **等待构建完成**

3. **测试渲染诊断页面：**
```
https://mojianbiao2025-art.github.io/SerialDebugger/test-qt-render.html
```

4. **提供诊断结果：**
   - 点击"检查 Canvas"的输出
   - 点击"检查 Module"的输出
   - 控制台的完整日志
   - 是否看到任何内容（即使是部分）

---

**更新时间：** 2025-02-03  
**状态：** 诊断中  
**关键问题：** Qt 应用加载但不渲染

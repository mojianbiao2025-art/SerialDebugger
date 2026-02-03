# Canvas 空白问题修复

## 🔍 问题现象

WebAssembly 应用加载完成，控制台显示：
```
✓ WebAssembly 运行时初始化成功！
✓ 应用程序加载完成！
```

但是页面上的 canvas 区域是空白的，Qt 应用程序界面没有显示。

## 🐛 问题原因

### 原因：Canvas 没有设置尺寸

Qt for WebAssembly 需要 canvas 元素有明确的宽度和高度才能渲染。

**问题代码：**
```html
<canvas id="canvas" style="display: none;"></canvas>
```

CSS 中也没有设置尺寸：
```css
#canvas {
    display: block;
    margin: 0 auto;
    border: 1px solid #ddd;
    /* 缺少 width 和 height */
}
```

**结果：**
- Canvas 默认尺寸是 300x150 像素（太小）
- Qt 应用无法正确渲染
- 显示为空白区域

## ✅ 修复方案

### 修复 1：CSS 设置 Canvas 尺寸

```css
#canvas {
    display: block;
    margin: 0 auto;
    border: 1px solid #ddd;
    border-radius: 4px;
    width: 100%;              /* 响应式宽度 */
    max-width: 1200px;        /* 最大宽度 */
    height: 800px;            /* 固定高度 */
    background: #f5f5f5;      /* 背景色，便于调试 */
}
```

### 修复 2：JavaScript 设置 Canvas 属性

```javascript
var canvas = document.getElementById('canvas');

// 设置 canvas 的实际渲染尺寸
canvas.width = 1200;
canvas.height = 800;
```

### 修复 3：增强调试信息

```javascript
onRuntimeInitialized: function() {
    addDebugLog('✓ WebAssembly 运行时初始化成功！');
    addDebugLog('Canvas 元素: ' + (canvas ? '已找到' : '未找到'));
},
setStatus: function(text) {
    if (!text) {
        // 应用加载完成
        addDebugLog('Canvas 尺寸: ' + canvas.width + 'x' + canvas.height);
    }
}
```

## 🎯 Canvas 尺寸说明

### CSS 尺寸 vs Canvas 属性

**CSS 尺寸（显示尺寸）：**
```css
width: 100%;
max-width: 1200px;
height: 800px;
```
- 控制 canvas 在页面上的显示大小
- 响应式布局

**Canvas 属性（渲染尺寸）：**
```javascript
canvas.width = 1200;
canvas.height = 800;
```
- 控制 canvas 的实际渲染分辨率
- 影响图像质量

### 推荐尺寸

| 用途 | 宽度 | 高度 | 说明 |
|------|------|------|------|
| 桌面 | 1200px | 800px | 标准桌面应用尺寸 |
| 笔记本 | 1024px | 768px | 适合小屏幕 |
| 平板 | 800px | 600px | 平板设备 |
| 全屏 | window.innerWidth | window.innerHeight | 自适应屏幕 |

## 🔄 完整的修复代码

### HTML
```html
<canvas id="canvas" style="display: none;"></canvas>
```

### CSS
```css
#canvas {
    display: block;
    margin: 0 auto;
    border: 1px solid #ddd;
    border-radius: 4px;
    width: 100%;
    max-width: 1200px;
    height: 800px;
    background: #f5f5f5;
}
```

### JavaScript
```javascript
var canvas = document.getElementById('canvas');

// 设置 canvas 尺寸
canvas.width = 1200;
canvas.height = 800;

var Module = {
    qtCanvasElements: [canvas],
    onRuntimeInitialized: function() {
        console.log('Canvas size:', canvas.width, 'x', canvas.height);
    },
    setStatus: function(text) {
        if (!text) {
            // 加载完成，显示 canvas
            document.getElementById('loading').style.display = 'none';
            canvas.style.display = 'block';
        }
    }
};
```

## 🧪 验证修复

### 检查 1：Canvas 尺寸
在浏览器控制台运行：
```javascript
var canvas = document.getElementById('canvas');
console.log('CSS 尺寸:', canvas.offsetWidth, 'x', canvas.offsetHeight);
console.log('渲染尺寸:', canvas.width, 'x', canvas.height);
```

应该看到：
```
CSS 尺寸: 1200 x 800
渲染尺寸: 1200 x 800
```

### 检查 2：Canvas 可见性
```javascript
var canvas = document.getElementById('canvas');
console.log('Display:', window.getComputedStyle(canvas).display);
console.log('Visibility:', window.getComputedStyle(canvas).visibility);
```

应该看到：
```
Display: block
Visibility: visible
```

### 检查 3：Qt 应用渲染
Canvas 应该显示：
- Qt 窗口背景
- 菜单栏
- 工具栏
- 主界面控件

## 🚨 常见问题

### 问题 1：Canvas 显示但仍然空白

**可能原因：**
- WebGL 初始化失败
- Qt 应用崩溃
- 内存不足

**检查方法：**
```javascript
// 检查 WebGL
var gl = canvas.getContext('webgl') || canvas.getContext('experimental-webgl');
console.log('WebGL:', gl ? '支持' : '不支持');

// 检查 Module
console.log('Module:', Module);
console.log('Module.canvas:', Module.qtCanvasElements);
```

### 问题 2：Canvas 尺寸不正确

**可能原因：**
- CSS 和 canvas 属性不匹配
- 父容器限制了尺寸

**解决方法：**
```javascript
// 强制设置尺寸
canvas.width = 1200;
canvas.height = 800;
canvas.style.width = '1200px';
canvas.style.height = '800px';
```

### 问题 3：响应式布局问题

**解决方法：**
```javascript
// 自适应窗口大小
function resizeCanvas() {
    var container = document.querySelector('.content');
    var maxWidth = Math.min(container.offsetWidth - 80, 1200);
    canvas.width = maxWidth;
    canvas.height = Math.floor(maxWidth * 0.67); // 3:2 比例
}

window.addEventListener('resize', resizeCanvas);
resizeCanvas();
```

## 📊 调试技巧

### 技巧 1：添加背景色
```css
#canvas {
    background: #f5f5f5; /* 灰色背景 */
}
```
如果看到灰色区域，说明 canvas 已显示，只是内容没有渲染。

### 技巧 2：添加边框
```css
#canvas {
    border: 2px solid red; /* 红色边框 */
}
```
确认 canvas 的实际尺寸和位置。

### 技巧 3：控制台日志
```javascript
Module.setStatus = function(text) {
    console.log('[Status]', text);
    if (!text) {
        console.log('[Canvas] Width:', canvas.width, 'Height:', canvas.height);
        console.log('[Canvas] Display:', canvas.style.display);
        console.log('[Canvas] Visible:', canvas.offsetWidth > 0);
    }
};
```

## 🎯 预期结果

### 修复前
- ✗ Canvas 区域空白
- ✗ 没有 Qt 应用界面
- ✗ Canvas 尺寸可能是 300x150（默认值）

### 修复后
- ✓ Canvas 显示 Qt 应用界面
- ✓ 菜单栏、工具栏、控件都可见
- ✓ Canvas 尺寸是 1200x800
- ✓ 界面响应用户操作

## 📝 提交修改

```bash
git add index.html
git commit -m "Fix: Set canvas size for Qt WebAssembly rendering

- 添加 CSS 尺寸：width 100%, max-width 1200px, height 800px
- 设置 canvas 属性：width 1200, height 800
- 添加背景色便于调试
- 增强调试日志显示 canvas 尺寸

修复了应用加载完成但 canvas 空白的问题。"
git push
```

## 🔗 相关问题

- **Module.canvas is deprecated** → 已修复，使用 `qtCanvasElements`
- **TypeError: Cannot read properties of undefined** → 已修复，导出运行时方法
- **Canvas 空白** → 本次修复，设置 canvas 尺寸

---

**修复日期：** 2025-02-03  
**状态：** 已修复  
**关键修改：** 设置 canvas 的 CSS 尺寸和渲染尺寸

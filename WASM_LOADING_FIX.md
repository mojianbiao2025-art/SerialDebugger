# WebAssembly加载问题诊断和修复

## 问题现象

浏览器中打开页面后，只显示加载界面，Qt应用程序没有显示出来。

## 可能的原因

### 1. 文件路径问题 ❌
**检查：** 打开浏览器开发者工具（F12），查看Console标签

**常见错误：**
```
Failed to load resource: net::ERR_FILE_NOT_FOUND
SerialDebugger.js:1 Failed to load
```

**原因：** 
- `SerialDebugger.js` 文件不存在
- 文件路径不正确
- 没有通过HTTP服务器访问

### 2. CORS策略问题 ❌
**错误信息：**
```
Access to script at 'file:///...' from origin 'null' has been blocked by CORS policy
```

**原因：** 直接双击打开HTML文件（file://协议），浏览器的CORS策略阻止加载WebAssembly

### 3. 文件未部署 ❌
**原因：** GitHub Actions构建成功，但文件没有正确部署到GitHub Pages

## 解决方案

### 方案1：使用本地HTTP服务器（推荐）

#### Python 3
```bash
# 进入包含HTML文件的目录
cd deploy  # 或者 build-wasm/bin

# 启动HTTP服务器
python -m http.server 8000

# 或者指定端口
python3 -m http.server 8080
```

然后访问：`http://localhost:8000/index.html`

#### Python 2
```bash
python -m SimpleHTTPServer 8000
```

#### Node.js (http-server)
```bash
# 安装
npm install -g http-server

# 运行
http-server -p 8000
```

#### PHP
```bash
php -S localhost:8000
```

### 方案2：检查文件是否存在

**必需的文件：**
```
deploy/
├── index.html
├── SerialDebugger.js
├── SerialDebugger.wasm
├── SerialDebugger.data (可能)
├── lang_zh.ini
├── lang_en.ini
├── lang_ja.ini
├── lang_de.ini
├── lang_fr.ini
└── styles.qss
```

**检查命令：**
```bash
ls -la deploy/
# 或
dir deploy\
```

**如果文件不存在：**
```bash
# 从构建目录复制
cp build-wasm/bin/SerialDebugger.* deploy/
```

### 方案3：修复GitHub Pages部署

#### 检查GitHub Pages设置

1. 访问仓库设置：`Settings` → `Pages`
2. 确认：
   - Source: `Deploy from a branch`
   - Branch: `gh-pages` / `(root)`
   - 或者 Branch: `main` / `docs`

#### 检查部署状态

1. 访问：`Actions` 标签
2. 查看最新的工作流运行
3. 确认 "Deploy to GitHub Pages" 步骤成功

#### 访问GitHub Pages

URL格式：
```
https://[username].github.io/[repository]/
```

例如：
```
https://mojianbiao2025-art.github.io/SerialDebugger/
```

### 方案4：检查浏览器控制台

**打开开发者工具：**
- Chrome/Edge: `F12` 或 `Ctrl+Shift+I`
- 点击 `Console` 标签

**查找错误信息：**

#### 错误1：404 Not Found
```
GET https://example.com/SerialDebugger.js 404 (Not Found)
```
**解决：** 文件路径错误或文件不存在

#### 错误2：MIME类型错误
```
Failed to load module script: Expected a JavaScript module script but the server responded with a MIME type of "text/plain"
```
**解决：** 服务器配置问题，使用正确的HTTP服务器

#### 错误3：WebAssembly加载失败
```
CompileError: WebAssembly.instantiate(): Compiling function failed
```
**解决：** WASM文件损坏或不完整，重新构建

#### 错误4：内存不足
```
RangeError: WebAssembly.Memory(): could not allocate memory
```
**解决：** 浏览器内存限制，关闭其他标签页或重启浏览器

### 方案5：修改index.html以显示更多调试信息

在 `index.html` 的 `<script>` 部分添加：

```html
<script>
    // 添加详细的错误处理
    var Module = {
        preRun: [],
        postRun: [],
        print: function(text) {
            console.log('[stdout]', text);
            document.getElementById('status').textContent = text;
        },
        printErr: function(text) {
            console.error('[stderr]', text);
            document.getElementById('status').innerHTML = 
                '<span style="color: red;">错误: ' + text + '</span>';
        },
        canvas: document.getElementById('canvas'),
        setStatus: function(text) {
            console.log('[status]', text);
            if (text) {
                document.getElementById('status').textContent = text;
            } else {
                document.getElementById('loading').style.display = 'none';
                document.getElementById('canvas').style.display = 'block';
            }
        },
        onRuntimeInitialized: function() {
            console.log('Runtime initialized successfully!');
            Module.setStatus('');
        }
    };

    // 检查文件是否可以加载
    fetch('SerialDebugger.js')
        .then(response => {
            if (!response.ok) {
                throw new Error('SerialDebugger.js not found (404)');
            }
            console.log('SerialDebugger.js found!');
        })
        .catch(error => {
            console.error('Failed to load SerialDebugger.js:', error);
            document.getElementById('status').innerHTML = 
                '<span style="color: red;">错误: 无法加载 SerialDebugger.js<br>' +
                '请确保通过HTTP服务器访问此页面</span>';
        });

    fetch('SerialDebugger.wasm')
        .then(response => {
            if (!response.ok) {
                throw new Error('SerialDebugger.wasm not found (404)');
            }
            console.log('SerialDebugger.wasm found!');
        })
        .catch(error => {
            console.error('Failed to load SerialDebugger.wasm:', error);
        });
</script>
```

### 方案6：创建简单的测试页面

创建 `test.html`：

```html
<!DOCTYPE html>
<html>
<head>
    <title>WebAssembly Test</title>
</head>
<body>
    <h1>WebAssembly Loading Test</h1>
    <div id="status">Testing...</div>
    <div id="files"></div>
    
    <script>
        // 测试文件是否存在
        const files = [
            'SerialDebugger.js',
            'SerialDebugger.wasm',
            'index.html'
        ];
        
        const filesDiv = document.getElementById('files');
        
        files.forEach(file => {
            fetch(file)
                .then(response => {
                    const status = response.ok ? '✓ Found' : '✗ Not Found';
                    const color = response.ok ? 'green' : 'red';
                    filesDiv.innerHTML += `<p style="color: ${color}">${file}: ${status}</p>`;
                })
                .catch(error => {
                    filesDiv.innerHTML += `<p style="color: red">${file}: ✗ Error - ${error.message}</p>`;
                });
        });
        
        // 检查协议
        const protocol = window.location.protocol;
        document.getElementById('status').innerHTML = 
            `Protocol: ${protocol}<br>` +
            `${protocol === 'file:' ? 
                '<span style="color: red;">⚠️ Using file:// protocol. Please use HTTP server!</span>' : 
                '<span style="color: green;">✓ Using HTTP protocol</span>'}`;
    </script>
</body>
</html>
```

## 完整的本地测试流程

### 步骤1：构建WebAssembly
```bash
# 使用之前的脚本
./rebuild_wasm_clean.sh

# 或手动构建
cd build-wasm
ninja
```

### 步骤2：准备部署目录
```bash
mkdir -p deploy
cp build-wasm/bin/SerialDebugger.* deploy/
cp index.html deploy/
cp lang_*.ini deploy/
cp styles.qss deploy/
```

### 步骤3：启动HTTP服务器
```bash
cd deploy
python3 -m http.server 8000
```

### 步骤4：打开浏览器
```
http://localhost:8000/index.html
```

### 步骤5：检查控制台
- 按 `F12` 打开开发者工具
- 查看 `Console` 标签
- 查看 `Network` 标签（检查文件加载状态）

## 常见问题排查

### Q1: 页面一直显示"正在加载..."
**检查：**
1. 打开开发者工具 → Network标签
2. 刷新页面
3. 查看 `SerialDebugger.js` 和 `SerialDebugger.wasm` 的状态

**可能原因：**
- 文件404（不存在）
- 文件下载中断
- CORS错误

### Q2: 显示"浏览器不支持 Web Serial API"
**解决：**
- 使用 Chrome 89+ 或 Edge 89+
- 确保不是在隐私/无痕模式
- 检查浏览器版本

### Q3: Canvas显示但是空白
**可能原因：**
- Qt应用程序崩溃
- WebGL不支持
- 内存不足

**检查：**
```javascript
// 在Console中运行
console.log(Module);
```

### Q4: GitHub Pages显示404
**检查：**
1. 仓库是否公开
2. GitHub Pages是否启用
3. 分支是否正确（gh-pages）
4. 文件是否在正确的目录

**正确的URL：**
```
https://[username].github.io/[repo]/
```

**错误的URL：**
```
https://github.com/[username]/[repo]/  # 这是仓库页面，不是Pages
```

## 推荐的开发工作流

### 本地开发
```bash
# 1. 构建
./rebuild_wasm_clean.sh

# 2. 测试
cd build-wasm/bin
python3 -m http.server 8000

# 3. 访问
# http://localhost:8000/index.html
```

### 部署到GitHub Pages
```bash
# 1. 准备文件
mkdir -p deploy
cp build-wasm/bin/SerialDebugger.* deploy/
cp index.html deploy/
cp lang_*.ini deploy/
cp styles.qss deploy/

# 2. 提交
git add deploy/
git commit -m "Update WebAssembly build"
git push

# 3. 等待GitHub Actions完成
# 4. 访问 https://[username].github.io/[repo]/
```

## 成功的标志

### 浏览器Console应该显示：
```
[status] 正在下载应用程序...
[status] 正在加载依赖项 (0/1)
[status] 所有依赖项已加载
Runtime initialized successfully!
```

### Network标签应该显示：
```
SerialDebugger.js    200  OK  (几百KB)
SerialDebugger.wasm  200  OK  (几MB)
```

### 页面应该显示：
- Qt应用程序界面
- 串口设置控件
- 发送/接收区域
- 菜单栏

---

**如果以上方法都不行，请提供：**
1. 浏览器Console的完整错误信息
2. Network标签的截图
3. 使用的浏览器和版本
4. 访问的URL

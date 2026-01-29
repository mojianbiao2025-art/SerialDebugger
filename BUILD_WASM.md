# WebAssembly 构建指南

## 概述

本项目支持编译为 WebAssembly，可在浏览器中直接运行，无需安装任何软件。

## 功能特性

- ✅ 完整的串口调试功能
- ✅ 使用 Web Serial API 访问串口
- ✅ 支持 HEX 发送/接收
- ✅ 数据绘图功能
- ✅ 多语言支持
- ✅ 响应式界面

## 浏览器要求

### 支持的浏览器

Web Serial API 需要以下浏览器之一：

- ✅ **Chrome 89+** (推荐)
- ✅ **Microsoft Edge 89+**
- ✅ **Opera 75+**

### 不支持的浏览器

- ❌ Firefox (暂不支持 Web Serial API)
- ❌ Safari (暂不支持 Web Serial API)

### 安全要求

- 必须通过 **HTTPS** 访问（或 localhost）
- 需要用户手动授权串口访问权限

## 本地构建

### 1. 安装 Emscripten

```bash
# 下载 Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk

# 安装和激活
./emsdk install 3.1.37
./emsdk activate 3.1.37
source ./emsdk_env.sh
```

### 2. 安装 Qt for WebAssembly

```bash
# 安装 aqtinstall
pip3 install aqtinstall

# 下载 Qt 5.15.2 for WebAssembly
aqt install-qt linux desktop 5.15.2 wasm_32 -m qtcharts
```

### 3. 构建项目

```bash
# 设置环境变量
export Qt5_DIR=/path/to/Qt/5.15.2/wasm_32
export PATH=$Qt5_DIR/bin:$PATH

# 创建构建目录
mkdir build-wasm
cd build-wasm

# 配置 CMake
emcmake cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=$Qt5_DIR \
  -G Ninja

# 编译
cmake --build . --config Release
```

### 4. 部署文件

构建完成后，需要以下文件：

```
deploy/
├── index.html              # 主页面
├── SerialDebugger.js       # JavaScript 加载器
├── SerialDebugger.wasm     # WebAssembly 二进制
├── SerialDebugger.data     # 资源文件
├── qtloader.js             # Qt 加载器
├── lang_*.ini              # 语言文件
└── styles.qss              # 样式表
```

### 5. 本地测试

```bash
# 使用 Python 启动本地服务器
cd deploy
python3 -m http.server 8000

# 访问 http://localhost:8000
```

## GitHub Actions 自动构建

项目已配置 GitHub Actions 自动构建 WebAssembly 版本。

### 触发构建

1. 推送代码到 `main` 或 `master` 分支
2. 创建 Pull Request
3. 手动触发工作流

### 下载构建产物

1. 访问 [Actions 页面](https://github.com/mojianbiao2025-art/SerialDebugger/actions)
2. 选择最新的 "Build WebAssembly" 工作流
3. 下载 `SerialDebugger-WebAssembly` 压缩包

## GitHub Pages 部署

### 启用 GitHub Pages

1. 进入仓库 Settings
2. 找到 Pages 设置
3. Source 选择 `gh-pages` 分支
4. 保存设置

### 访问地址

部署成功后，可通过以下地址访问：

```
https://mojianbiao2025-art.github.io/SerialDebugger/
```

## 使用说明

### 1. 打开网页

使用支持的浏览器（Chrome/Edge/Opera）访问部署的网页。

### 2. 连接串口

1. 点击"打开串口"按钮
2. 浏览器会弹出串口选择对话框
3. 选择要连接的串口设备
4. 点击"连接"

### 3. 配置参数

- 波特率：9600 - 115200
- 数据位：5 - 8
- 停止位：1, 1.5, 2
- 校验位：无、奇、偶

### 4. 发送数据

- 在发送框输入数据
- 选择 HEX 模式或文本模式
- 点击"发送"按钮

### 5. 接收数据

- 接收到的数据显示在接收框
- 可选择 HEX 显示模式
- 可显示时间戳

## Web Serial API 限制

### 安全限制

- 只能在 HTTPS 或 localhost 访问
- 需要用户手动授权
- 每次刷新页面需要重新授权

### 功能限制

- 不支持自动检测串口列表
- 需要用户手动选择串口
- 某些高级串口功能可能不可用

## 故障排除

### 问题：浏览器提示不支持 Web Serial API

**解决方案：**
- 使用 Chrome 89+、Edge 89+ 或 Opera 75+
- 确保浏览器已更新到最新版本

### 问题：无法访问串口

**解决方案：**
- 确保使用 HTTPS 或 localhost
- 检查浏览器权限设置
- 确认串口设备已连接

### 问题：页面加载失败

**解决方案：**
- 检查网络连接
- 清除浏览器缓存
- 确认所有文件都已正确部署

### 问题：数据发送/接收异常

**解决方案：**
- 检查串口参数配置
- 确认设备连接正常
- 查看浏览器控制台错误信息

## 性能优化

### 减小文件大小

```bash
# 使用优化标志编译
emcmake cmake .. \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_PREFIX_PATH=$Qt5_DIR
```

### 启用压缩

在 Web 服务器配置中启用 gzip 或 brotli 压缩：

```nginx
# Nginx 配置示例
location ~ \.(wasm|js|data)$ {
    gzip on;
    gzip_types application/wasm application/javascript;
}
```

## 技术细节

### 架构

```
┌─────────────────┐
│   index.html    │  HTML 页面
└────────┬────────┘
         │
┌────────▼────────┐
│ SerialDebugger  │  JavaScript 加载器
│      .js        │
└────────┬────────┘
         │
┌────────▼────────┐
│ SerialDebugger  │  WebAssembly 二进制
│     .wasm       │
└────────┬────────┘
         │
┌────────▼────────┐
│  Web Serial API │  浏览器串口接口
└─────────────────┘
```

### Web Serial API 接口

```javascript
// 请求串口访问
const port = await navigator.serial.requestPort();

// 打开串口
await port.open({
    baudRate: 115200,
    dataBits: 8,
    stopBits: 1,
    parity: 'none'
});

// 读取数据
const reader = port.readable.getReader();
while (true) {
    const { value, done } = await reader.read();
    if (done) break;
    // 处理数据
}

// 写入数据
const writer = port.writable.getWriter();
await writer.write(data);
writer.releaseLock();

// 关闭串口
await port.close();
```

## 参考资料

- [Qt for WebAssembly](https://doc.qt.io/qt-5/wasm.html)
- [Emscripten](https://emscripten.org/)
- [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API)
- [Can I use Web Serial API](https://caniuse.com/web-serial)

## 联系方式

- 作者：莫建标
- 邮箱：mojianbiao2025@gmail.com
- GitHub：https://github.com/mojianbiao2025-art/SerialDebugger

---

© 2025 上海大族富创得股份有限公司

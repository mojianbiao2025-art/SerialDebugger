# Web 版本使用指南

## 🌐 在线访问

直接访问：**https://mojianbiao2025-art.github.io/SerialDebugger/**

无需安装任何软件，浏览器直接运行！

## 📋 系统要求

### 支持的浏览器

| 浏览器 | 最低版本 | 状态 |
|--------|---------|------|
| Google Chrome | 89+ | ✅ 完全支持 |
| Microsoft Edge | 89+ | ✅ 完全支持 |
| Opera | 75+ | ✅ 完全支持 |
| Firefox | - | ❌ 暂不支持 |
| Safari | - | ❌ 暂不支持 |

### 安全要求

- ✅ 必须通过 **HTTPS** 访问（或 localhost）
- ✅ 需要用户手动授权串口访问权限
- ✅ 每次刷新页面需要重新授权

## 🚀 快速开始

### 1. 打开网页

使用 Chrome、Edge 或 Opera 浏览器访问：
```
https://mojianbiao2025-art.github.io/SerialDebugger/
```

### 2. 连接串口设备

1. 将串口设备连接到电脑
2. 点击"打开串口"按钮
3. 浏览器会弹出串口选择对话框
4. 选择要连接的串口设备
5. 点击"连接"按钮

![串口选择对话框](docs/web-serial-dialog.png)

### 3. 配置串口参数

- **波特率**: 9600, 19200, 38400, 57600, 115200
- **数据位**: 5, 6, 7, 8
- **停止位**: 1, 1.5, 2
- **校验位**: 无、奇、偶

### 4. 发送和接收数据

#### 发送数据
1. 在发送框输入数据
2. 选择发送模式：
   - 文本模式：直接发送文本
   - HEX 模式：发送十六进制数据
3. 点击"发送"按钮

#### 接收数据
- 接收到的数据自动显示在接收框
- 可选择 HEX 显示模式
- 可显示时间戳

### 5. 数据绘图

1. 切换到"波形图"标签页
2. 发送格式化数据：
   ```
   plotter 1.23 4.56 7.89
   ```
   或直接发送数字：
   ```
   1.23 4.56 7.89
   ```
3. 最多支持 6 个通道同时绘图

## 💡 功能特性

### ✅ 已支持功能

- ✅ 串口通信（所有标准参数）
- ✅ HEX 发送/接收模式
- ✅ 时间戳显示
- ✅ 字节计数器（RX/TX）
- ✅ 自动发送功能
- ✅ 命令列表管理
- ✅ 6 通道数据绘图
- ✅ 多语言支持
- ✅ 现代化 UI

### ⚠️ 限制说明

由于 Web Serial API 的限制：

- ❌ 无法自动列出可用串口
- ❌ 需要用户手动选择串口
- ❌ 每次刷新需要重新授权
- ❌ 某些高级串口功能可能不可用

## 🔧 常见问题

### Q1: 浏览器提示不支持 Web Serial API

**原因**: 浏览器版本过低或不支持

**解决方案**:
1. 更新浏览器到最新版本
2. 使用 Chrome 89+、Edge 89+ 或 Opera 75+
3. 检查浏览器是否启用了 Web Serial API

### Q2: 无法访问串口设备

**原因**: 权限问题或安全限制

**解决方案**:
1. 确保通过 HTTPS 或 localhost 访问
2. 检查浏览器权限设置
3. 确认串口设备已正确连接
4. 尝试重新授权串口访问

### Q3: 页面加载很慢

**原因**: WebAssembly 文件较大

**解决方案**:
1. 等待首次加载完成（会缓存）
2. 检查网络连接
3. 清除浏览器缓存后重试

### Q4: 数据发送/接收异常

**原因**: 串口参数配置错误

**解决方案**:
1. 检查波特率、数据位等参数
2. 确认设备连接正常
3. 查看浏览器控制台错误信息
4. 尝试重新连接串口

### Q5: 刷新页面后需要重新连接

**原因**: Web Serial API 安全限制

**说明**: 这是正常行为，每次刷新页面都需要重新授权串口访问

### Q6: Firefox 或 Safari 无法使用

**原因**: 这些浏览器暂不支持 Web Serial API

**解决方案**: 使用 Chrome、Edge 或 Opera 浏览器

## 🎯 使用技巧

### 1. 快速测试串口

```
1. 打开网页
2. 连接串口
3. 发送测试数据：AT
4. 查看接收框响应
```

### 2. 数据监控

```
1. 启用时间戳显示
2. 启用 HEX 显示模式
3. 观察数据流
```

### 3. 自动发送

```
1. 勾选"自动发送"
2. 设置发送间隔（毫秒）
3. 输入要发送的数据
4. 自动循环发送
```

### 4. 命令管理

```
1. 在发送框输入常用命令
2. 点击"添加命令"
3. 双击命令列表快速发送
```

### 5. 数据绘图

```
1. 切换到波形图标签
2. 发送格式：plotter 1.23 4.56
3. 实时查看波形
4. 查看统计信息（最大、最小、平均值）
```

## 🔐 隐私和安全

### 数据安全

- ✅ 所有数据在本地处理
- ✅ 不上传任何数据到服务器
- ✅ 不收集用户信息
- ✅ 开源代码，可审计

### 权限说明

Web 版本需要以下权限：

1. **串口访问权限**
   - 用途：读写串口数据
   - 授权方式：用户手动授权
   - 范围：仅限用户选择的串口

2. **本地存储权限**
   - 用途：保存用户设置
   - 授权方式：自动授权
   - 范围：仅限本应用数据

## 📊 性能优化

### 减少加载时间

1. **首次访问**：等待 WebAssembly 文件下载（约 5-10MB）
2. **后续访问**：浏览器会缓存文件，加载更快

### 提高运行性能

1. 关闭不需要的功能（如时间戳、绘图）
2. 定期清空接收缓冲区
3. 使用 HEX 模式处理二进制数据

## 🌍 多语言支持

Web 版本支持以下语言：

- 🇨🇳 中文（简体）
- 🇺🇸 English
- 🇯🇵 日本語
- 🇩🇪 Deutsch
- 🇫🇷 Français

切换方法：点击菜单栏"语言"选择

## 📱 移动设备支持

### Android

- ✅ Chrome 89+ for Android
- ✅ 需要 USB OTG 支持
- ✅ 需要 USB 转串口适配器

### iOS

- ❌ Safari 暂不支持 Web Serial API
- ❌ Chrome for iOS 使用 Safari 内核，也不支持

## 🔄 离线使用

### PWA 支持（计划中）

未来版本将支持：

- 📦 离线安装
- 🚀 快速启动
- 📱 添加到主屏幕
- 🔔 后台通知

## 📞 技术支持

### 问题反馈

- GitHub Issues: https://github.com/mojianbiao2025-art/SerialDebugger/issues
- Email: mojianbiao2025@gmail.com

### 功能建议

欢迎提交功能建议和改进意见！

## 🎓 开发者信息

### 技术栈

- **前端**: HTML5, CSS3, JavaScript
- **框架**: Qt 5.15.2 for WebAssembly
- **编译**: Emscripten 3.1.37
- **API**: Web Serial API
- **部署**: GitHub Pages

### 源代码

- GitHub: https://github.com/mojianbiao2025-art/SerialDebugger
- 许可证: MIT

### 构建说明

详见 [BUILD_WASM.md](BUILD_WASM.md)

## 📚 相关资源

### 官方文档

- [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API)
- [Qt for WebAssembly](https://doc.qt.io/qt-5/wasm.html)
- [Emscripten](https://emscripten.org/)

### 浏览器兼容性

- [Can I use Web Serial API](https://caniuse.com/web-serial)

### 示例代码

- [Web Serial API Examples](https://github.com/GoogleChromeLabs/serial-terminal)

## 🙏 致谢

感谢以下项目和技术：

- Qt Framework
- Emscripten
- Web Serial API
- GitHub Pages

---

**作者**: 莫建标  
**公司**: 上海大族富创得股份有限公司  
**邮箱**: mojianbiao2025@gmail.com  
**GitHub**: [@mojianbiao2025-art](https://github.com/mojianbiao2025-art)

© 2025 上海大族富创得股份有限公司

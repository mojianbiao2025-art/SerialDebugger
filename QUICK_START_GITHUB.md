# 🚀 快速开始：使用 GitHub Actions 构建 Android APK

## 📋 准备工作清单

- [ ] GitHub 账号
- [ ] Git 已安装
- [ ] 项目代码已准备好
- [ ] （可选）了解你的 Android 设备架构

## 🔍 检查你的 Android 设备（可选）

如果你想为特定设备优化构建，可以先检查设备信息：

### 方法 A：使用自动检查脚本（推荐）

1. 在 Android 设备上启用 USB 调试：
   - 设置 -> 关于手机 -> 连续点击"版本号"7次
   - 设置 -> 开发者选项 -> 启用"USB调试"

2. 用 USB 连接设备到电脑

3. 运行 `check_device.bat`

4. 查看输出的配置建议

### 方法 B：使用手机应用

1. 在 Google Play 下载 "CPU-Z" 应用
2. 查看 CPU 架构（通常是 `arm64-v8a`）
3. 查看 Android 版本

### 默认配置（推荐）

如果不确定，使用默认配置即可，它支持大多数现代 Android 设备：
```yaml
-DANDROID_ABI=arm64-v8a
-DANDROID_PLATFORM=android-21
```

详细说明请查看：[CHECK_ANDROID_DEVICE.md](CHECK_ANDROID_DEVICE.md)

## ⚡ 三步完成构建

### 第 1 步：创建 GitHub 仓库

1. 访问 https://github.com/new
2. 填写仓库名称：`SerialDebugger`
3. 选择 Public（公开）或 Private（私有）
4. 点击 "Create repository"
5. **记下仓库 URL**，例如：`https://github.com/YOUR_USERNAME/SerialDebugger.git`

### 第 2 步：推送代码

#### 方法 A：使用自动脚本（推荐）

双击运行 `git_push.bat`，按提示操作：
1. 输入提交信息（或按回车使用默认）
2. 输入 GitHub 仓库 URL
3. 输入 GitHub 用户名和密码（使用 Personal Access Token）

#### 方法 B：手动命令

打开 PowerShell 或 Git Bash，在项目目录执行：

```bash
# 1. 初始化 Git
git init

# 2. 添加所有文件
git add .

# 3. 提交
git commit -m "Initial commit: Serial Port Debugger"

# 4. 添加远程仓库（替换 YOUR_USERNAME 和 YOUR_REPO）
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git

# 5. 推送
git branch -M main
git push -u origin main
```

**注意**：如果需要密码，使用 Personal Access Token：
1. GitHub 头像 -> Settings -> Developer settings
2. Personal access tokens -> Generate new token
3. 勾选 `repo` 权限
4. 复制生成的 token 作为密码使用

### 第 3 步：下载 APK

1. 访问你的 GitHub 仓库
2. 点击 **"Actions"** 标签
3. 等待构建完成（约 10-15 分钟）
4. 点击最新的成功构建（绿色 ✓）
5. 向下滚动到 **"Artifacts"** 部分
6. 点击 **"SerialDebugger-Android-APK"** 下载
7. 解压 ZIP 文件得到 APK

## 📱 安装到 Android 设备

### 准备设备
1. 设置 -> 安全 -> 允许安装未知来源应用
2. 将 APK 传输到设备

### 安装
1. 在设备上找到 APK 文件
2. 点击安装
3. 授予必要权限
4. 完成！

## 🎯 构建状态

构建完成后，你会看到：

✅ **成功**：绿色勾号，可以下载 APK
❌ **失败**：红色叉号，点击查看错误日志
🟡 **进行中**：黄色圆圈，正在构建

## 📊 已创建的文件

```
项目目录/
├── .github/
│   └── workflows/
│       └── android-build.yml      ✅ GitHub Actions 配置
├── android/
│   ├── AndroidManifest.xml        ✅ Android 清单
│   └── res/
│       ├── xml/
│       │   └── device_filter.xml  ✅ USB 设备过滤
│       └── drawable/
│           └── icon.png           ✅ 应用图标
├── .gitignore                     ✅ Git 忽略文件
├── README.md                      ✅ 项目说明
├── LICENSE                        ✅ 开源许可证
├── GITHUB_SETUP.md                ✅ 详细设置指南
├── ANDROID_BUILD_GUIDE.md         ✅ Android 构建指南
└── git_push.bat                   ✅ 推送辅助脚本
```

## 🔧 常见问题

### Q: 推送失败，提示认证错误？
**A**: 使用 Personal Access Token 而不是账号密码
1. GitHub -> Settings -> Developer settings
2. Personal access tokens -> Generate new token
3. 勾选 `repo` 权限
4. 使用 token 作为密码

### Q: Actions 构建失败？
**A**: 点击失败的构建查看日志，常见原因：
- Qt 下载超时：重新运行构建
- NDK 版本问题：等待自动重试
- 网络问题：稍后重试

### Q: APK 无法安装？
**A**: 
1. 确认已启用"未知来源"安装
2. 检查 Android 版本（需要 5.0+）
3. 卸载旧版本后重试

### Q: 找不到串口设备？
**A**: 
1. 确认设备支持 USB OTG
2. 授予 USB 权限
3. 检查串口芯片是否支持

## 📚 更多资源

- **详细设置指南**: [GITHUB_SETUP.md](GITHUB_SETUP.md)
- **Android 构建指南**: [ANDROID_BUILD_GUIDE.md](ANDROID_BUILD_GUIDE.md)
- **项目说明**: [README.md](README.md)

## 🎉 完成！

现在你可以：
1. ✅ 自动构建 Android APK
2. ✅ 分享给其他人使用
3. ✅ 持续更新和改进
4. ✅ 收集用户反馈

每次推送代码，GitHub Actions 都会自动构建新的 APK！

---

**需要帮助？** 查看 [GITHUB_SETUP.md](GITHUB_SETUP.md) 获取详细说明。

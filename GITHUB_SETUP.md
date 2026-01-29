# GitHub Actions 自动构建 Android APK 指南

## 步骤 1: 准备 GitHub 仓库

### 1.1 创建 GitHub 账号
如果还没有 GitHub 账号：
1. 访问 https://github.com
2. 点击 "Sign up"
3. 按照提示完成注册

### 1.2 创建新仓库
1. 登录 GitHub
2. 点击右上角 "+" -> "New repository"
3. 填写信息：
   - Repository name: `SerialDebugger` (或其他名称)
   - Description: `串口调试助手 - Serial Port Debugger`
   - 选择 Public 或 Private
   - 不要勾选 "Initialize this repository with a README"
4. 点击 "Create repository"

## 步骤 2: 安装 Git

### Windows
1. 下载 Git: https://git-scm.com/download/win
2. 运行安装程序，使用默认设置
3. 打开 Git Bash 或 PowerShell

### 配置 Git
```bash
git config --global user.name "你的名字"
git config --global user.email "your.email@example.com"
```

## 步骤 3: 推送代码到 GitHub

### 3.1 初始化本地仓库
在项目目录中打开 PowerShell 或 Git Bash：

```bash
# 初始化 Git 仓库
git init

# 添加所有文件
git add .

# 提交
git commit -m "Initial commit: Serial Port Debugger v1.0"
```

### 3.2 连接到 GitHub 仓库
```bash
# 添加远程仓库（替换 YOUR_USERNAME 和 YOUR_REPO）
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git

# 推送代码
git branch -M main
git push -u origin main
```

如果需要输入用户名和密码：
- 用户名：你的 GitHub 用户名
- 密码：使用 Personal Access Token（不是账号密码）

### 3.3 创建 Personal Access Token（如果需要）
1. GitHub 右上角头像 -> Settings
2. 左侧菜单最底部 -> Developer settings
3. Personal access tokens -> Tokens (classic)
4. Generate new token (classic)
5. 勾选 `repo` 权限
6. 生成并复制 token（只显示一次！）
7. 使用 token 作为密码推送代码

## 步骤 4: 启用 GitHub Actions

### 4.1 检查 Actions 是否启用
1. 进入你的 GitHub 仓库
2. 点击 "Actions" 标签
3. 如果看到 "Get started with GitHub Actions"，说明已启用
4. 如果看到禁用提示，点击 "Enable Actions"

### 4.2 查看构建状态
1. 推送代码后，Actions 会自动开始构建
2. 在 "Actions" 标签页可以看到构建进度
3. 点击具体的工作流可以查看详细日志

## 步骤 5: 下载构建的 APK

### 方法一：从 Actions 下载
1. 进入 "Actions" 标签
2. 点击最新的成功构建（绿色勾）
3. 向下滚动到 "Artifacts" 部分
4. 点击 "SerialDebugger-Android-APK" 下载
5. 解压 ZIP 文件得到 APK

### 方法二：从 Releases 下载（如果创建了 Release）
1. 进入 "Releases" 标签
2. 点击最新版本
3. 在 "Assets" 部分下载 APK

## 步骤 6: 安装 APK 到 Android 设备

### 6.1 准备设备
1. 在 Android 设备上启用"未知来源"安装：
   - 设置 -> 安全 -> 未知来源（允许）
   - 或：设置 -> 应用 -> 特殊访问权限 -> 安装未知应用

2. 将 APK 传输到设备：
   - 通过 USB 连接电脑，复制 APK
   - 或通过邮件/云盘下载到设备
   - 或直接在设备浏览器下载

### 6.2 安装
1. 在设备上找到 APK 文件
2. 点击安装
3. 授予必要权限
4. 安装完成后打开应用

### 6.3 使用 USB 串口
1. 连接 USB OTG 转接头
2. 连接 USB 转串口设备
3. 打开应用，授予 USB 权限
4. 选择串口设备开始使用

## 故障排除

### 问题 1: 推送代码失败
**错误**: `Authentication failed`

**解决**:
1. 使用 Personal Access Token 而不是密码
2. 检查 token 权限是否包含 `repo`
3. 确认用户名和 token 正确

### 问题 2: Actions 构建失败
**错误**: 在 Actions 日志中看到错误

**解决**:
1. 点击失败的构建查看详细日志
2. 常见问题：
   - Qt 安装失败：等待重试或检查网络
   - NDK 版本问题：修改 workflow 中的 NDK 版本
   - CMake 配置错误：检查 CMakeLists.txt

### 问题 3: APK 安装失败
**错误**: `App not installed`

**解决**:
1. 确认已启用"未知来源"安装
2. 检查设备 Android 版本（需要 5.0+）
3. 卸载旧版本后重新安装
4. 检查存储空间是否充足

### 问题 4: 找不到串口设备
**错误**: 应用中看不到串口

**解决**:
1. 确认设备支持 USB OTG
2. 检查 USB 转串口芯片是否支持（查看 device_filter.xml）
3. 授予 USB 权限
4. 重新插拔 USB 设备

## 高级配置

### 自动发布 Release

在 `.github/workflows/android-build.yml` 中已配置自动发布。

创建 Release：
```bash
# 创建标签
git tag -a v1.0.0 -m "Release version 1.0.0"

# 推送标签
git push origin v1.0.0
```

GitHub Actions 会自动构建并创建 Release。

### 自定义构建

修改 `.github/workflows/android-build.yml`：

```yaml
# 修改 Android ABI（架构）
-DANDROID_ABI=arm64-v8a  # 改为 armeabi-v7a, x86, x86_64

# 修改最低 Android 版本
-DANDROID_PLATFORM=android-21  # 改为其他版本

# 修改 Qt 版本
version: '5.15.2'  # 改为其他版本
```

### 添加构建徽章

在 README.md 中添加：
```markdown
[![Build Status](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/android-build.yml/badge.svg)](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/android-build.yml)
```

## 完整流程总结

1. ✅ 创建 GitHub 仓库
2. ✅ 安装并配置 Git
3. ✅ 推送代码到 GitHub
4. ✅ GitHub Actions 自动构建
5. ✅ 下载构建的 APK
6. ✅ 安装到 Android 设备
7. ✅ 测试串口功能

## 需要帮助？

- GitHub 文档: https://docs.github.com
- GitHub Actions 文档: https://docs.github.com/actions
- Qt for Android: https://doc.qt.io/qt-5/android.html

## 下一步

构建成功后，你可以：
1. 分享 APK 给其他人使用
2. 发布到应用商店
3. 继续开发新功能
4. 收集用户反馈

祝你构建成功！🎉

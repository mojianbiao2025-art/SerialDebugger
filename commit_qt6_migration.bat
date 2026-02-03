@echo off
chcp 65001 >nul
echo ========================================
echo WebAssembly 构建修复 - 提交脚本
echo ========================================
echo.

echo 正在添加修改的文件...
git add CMakeLists.txt
git add .github/workflows/build-all.yml
git add Qt6迁移指南.md
git add 总结和建议.md

echo.
echo 查看将要提交的文件...
git status

echo.
echo ========================================
echo 修复内容摘要：
echo ========================================
echo 1. CMakeLists.txt
echo    - 支持 Qt 5 和 Qt 6
echo    - WebAssembly 使用 Qt 5（更稳定）
echo    - Android 使用 Qt 6（更好支持）
echo    - 添加 --bind 标志
echo.
echo 2. GitHub Actions
echo    - WebAssembly: Qt 5.15.2 + Emscripten 1.39.20
echo    - Android: Qt 6.5.3
echo    - 创建缺失模块的虚拟配置
echo.
echo 3. 预期改进
echo    - 避免 Qt 6 embind 链接问题
echo    - 更稳定的 WebAssembly 构建
echo    - .wasm 文件应该是 3-5 MB（不是 4 KB）
echo    - 更好的浏览器兼容性
echo ========================================
echo.

set /p confirm="确认提交 WebAssembly 构建修复吗？(Y/N): "
if /i "%confirm%" NEQ "Y" (
    echo 取消提交。
    pause
    exit /b
)

echo.
echo 正在提交...
git commit -m "Fix WebAssembly build with Qt 5 and proper embind linking

Major changes:
- Keep Qt 5.15.2 for WebAssembly (more stable than Qt 6)
- Keep Qt 6.5.3 for Android (better support)
- Use Emscripten 1.39.20 (matches Qt 5.15.2)
- Add --bind flag for proper embind linking
- Support both Qt 5 and Qt 6 in CMakeLists.txt
- Create dummy configs for missing Qt modules
- Simplify build process

Benefits:
- Avoid Qt 6 embind linking issues
- More stable WebAssembly builds
- Proper .wasm file generation (3-5 MB instead of 4 KB)
- Better browser compatibility
- Proven configuration

This should fix the issue where .wasm file was only 4 KB."

if %errorlevel% neq 0 (
    echo.
    echo 提交失败！请检查错误信息。
    pause
    exit /b 1
)

echo.
echo ✓ 提交成功！
echo.

set /p push="是否立即推送到 GitHub？(Y/N): "
if /i "%push%" NEQ "Y" (
    echo.
    echo 提交已完成，但未推送。
    echo 您可以稍后使用以下命令推送：
    echo   git push
    pause
    exit /b
)

echo.
echo 正在推送到 GitHub...
git push

if %errorlevel% neq 0 (
    echo.
    echo 推送失败！请检查网络连接和权限。
    pause
    exit /b 1
)

echo.
echo ========================================
echo ✓ WebAssembly 构建修复完成！
echo ========================================
echo.
echo 下一步：
echo 1. 访问 GitHub Actions 查看构建状态
echo    https://github.com/mojianbiao2025-art/SerialDebugger/actions
echo.
echo 2. 等待构建完成（约 10-15 分钟）
echo    Qt 5 构建比 Qt 6 更快更稳定
echo.
echo 3. 检查构建日志
echo    确认 .wasm 文件大小是 3-5 MB（不是 4 KB）
echo.
echo 4. 测试应用
echo    https://mojianbiao2025-art.github.io/SerialDebugger/
echo    按 Ctrl+F5 强制刷新
echo.
echo 5. 验证界面
echo    应该能看到完整的 Qt 应用界面
echo.
echo 关键指标：
echo - SerialDebugger.wasm: 3-5 MB ✅
echo - 界面元素显示 ✅
echo - 控件可交互 ✅
echo.
echo 按任意键退出...
pause >nul

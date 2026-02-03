@echo off
chcp 65001 >nul
echo ========================================
echo Qt 6 迁移 - 提交脚本
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
echo 迁移内容摘要：
echo ========================================
echo 1. CMakeLists.txt
echo    - 从 Qt 5.15.2 迁移到 Qt 6.5.3
echo    - 使用 qt_add_executable()
echo    - 简化 WebAssembly 配置
echo.
echo 2. GitHub Actions
echo    - Emscripten: 1.39.20 → 3.1.50
echo    - Qt: 5.15.2 → 6.5.3
echo    - 使用 qt-cmake 构建
echo.
echo 3. 预期改进
echo    - 更好的 WebAssembly 支持
echo    - 更稳定的渲染
echo    - 更小的文件大小
echo    - .wasm 文件应该是 3-5 MB（不是 4 KB）
echo ========================================
echo.

set /p confirm="确认提交 Qt 6 迁移吗？(Y/N): "
if /i "%confirm%" NEQ "Y" (
    echo 取消提交。
    pause
    exit /b
)

echo.
echo 正在提交...
git commit -m "Migrate to Qt 6.5.3 for better WebAssembly support

Major changes:
- Upgrade from Qt 5.15.2 to Qt 6.5.3
- Upgrade Emscripten from 1.39.20 to 3.1.50
- Use qt_add_executable() instead of add_executable()
- Use qt-cmake for WebAssembly builds
- Install desktop Qt first (required for WebAssembly)
- Remove invalid module dependencies
- Simplify WebAssembly configuration

Benefits:
- Better WebAssembly stability
- Improved rendering performance
- Proper .wasm file generation (3-5 MB instead of 4 KB)
- Better browser compatibility
- More reliable builds

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
echo ✓ Qt 6 迁移完成！
echo ========================================
echo.
echo 下一步：
echo 1. 访问 GitHub Actions 查看构建状态
echo    https://github.com/mojianbiao2025-art/SerialDebugger/actions
echo.
echo 2. 等待构建完成（约 15-20 分钟）
echo    Qt 6 构建可能比 Qt 5 稍慢，但更可靠
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

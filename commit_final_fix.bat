@echo off
chcp 65001 >nul
echo ========================================
echo WebAssembly 彻底修复 - 提交脚本
echo ========================================
echo.

echo 正在添加修改的文件...
git add CMakeLists.txt
git add main.cpp
git add test-minimal.html
git add .github/workflows/build-all.yml
git add FINAL_SOLUTION.md
git add commit_final_fix.bat

echo.
echo 查看将要提交的文件...
git status

echo.
echo ========================================
echo 彻底修复方案：
echo ========================================
echo.
echo 1. CMakeLists.txt
echo    ✓ 添加编译定义禁用 Qt 功能
echo      - QT_NO_SETTINGS
echo      - QT_NO_SESSIONMANAGER  
echo      - QT_NO_CLIPBOARD
echo    ✓ 添加链接标志
echo      - ERROR_ON_UNDEFINED_SYMBOLS=0
echo      - DISABLE_EXCEPTION_CATCHING=0
echo      - ASSERTIONS=1
echo.
echo 2. main.cpp
echo    ✓ 在 Qt 初始化前阻止 localStorage
echo    ✓ 设置环境变量禁用 Qt 功能
echo    ✓ 移除 WebAssembly 的图标设置
echo.
echo 3. test-minimal.html
echo    ✓ 新的测试页面
echo    ✓ 在 JavaScript 层面阻止 localStorage
echo    ✓ 详细的日志输出
echo.
echo 4. GitHub Actions
echo    ✓ 部署测试页面
echo    ✓ 检查 .wasm 文件大小
echo.
echo 原理：
echo - Qt 5.15.2 的 embind 错误来自 localStorage 访问
echo - 在多个层面阻止 localStorage：
echo   1. JavaScript 层（test-minimal.html）
echo   2. C++ 层（main.cpp）
echo   3. 编译层（CMakeLists.txt）
echo - 禁用所有可能触发 embind 的 Qt 功能
echo.
echo ========================================
echo.

set /p confirm="确认提交彻底修复方案吗？(Y/N): "
if /i "%confirm%" NEQ "Y" (
    echo 取消提交。
    pause
    exit /b
)

echo.
echo 正在提交...
git commit -m "Complete fix for WebAssembly embind issues

This is a comprehensive solution that blocks Qt's localStorage access
at multiple levels to prevent embind errors.

Changes:
1. CMakeLists.txt
   - Add compile definitions: QT_NO_SETTINGS, QT_NO_SESSIONMANAGER, QT_NO_CLIPBOARD
   - Add linker flags: ERROR_ON_UNDEFINED_SYMBOLS=0, ASSERTIONS=1
   - Disable all Qt features that might trigger embind

2. main.cpp
   - Block localStorage access before Qt initialization
   - Set environment variables to disable Qt features
   - Remove icon setting for WebAssembly (not needed)

3. test-minimal.html
   - New minimal test page with localStorage blocking
   - Detailed logging for debugging
   - Cleaner interface

4. GitHub Actions
   - Deploy test-minimal.html
   - Add .wasm file size check

Strategy:
- Block localStorage at JavaScript level (before Qt loads)
- Block localStorage at C++ level (in main.cpp)
- Disable Qt features at compile time (CMakeLists.txt)
- This multi-layer approach should completely prevent embind errors

Expected result:
- No more 'Cannot read properties of undefined' errors
- Qt application should render correctly in canvas
- All UI elements should be visible and functional"

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
echo ✓ 彻底修复已提交并推送！
echo ========================================
echo.
echo 下一步：
echo.
echo 1. 等待 GitHub Actions 构建完成（约 10-15 分钟）
echo    https://github.com/mojianbiao2025-art/SerialDebugger/actions
echo.
echo 2. 测试新版本：
echo    主页面: https://mojianbiao2025-art.github.io/SerialDebugger/
echo    测试页面: https://mojianbiao2025-art.github.io/SerialDebugger/test-minimal.html
echo.
echo 3. 检查浏览器控制台：
echo    - 应该看到 "localStorage 访问被阻止" 消息
echo    - 不应该有 embind 错误
echo    - 应该看到 Qt 界面渲染
echo.
echo 4. 如果仍有问题：
echo    - 按 Ctrl+Shift+R 强制刷新
echo    - 清除浏览器缓存
echo    - 检查 .wasm 文件大小（应该是 4-5 MB）
echo.
echo 预期结果：
echo ✅ 没有 embind 错误
echo ✅ Qt 界面正确显示
echo ✅ 所有控件可以交互
echo.
echo 按任意键退出...
pause >nul

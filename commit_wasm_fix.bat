@echo off
chcp 65001 >nul
echo ========================================
echo WebAssembly 错误修复 - 提交脚本
echo ========================================
echo.

echo 正在添加修改的文件...
git add CMakeLists.txt
git add index.html
git add test-wasm-loading.html
git add .github/workflows/build-all.yml
git add .github/workflows/wasm-build.yml
git add WASM_EMVAL_FIX.md
git add WASM_LOADING_SOLUTION.md
git add WebAssembly错误修复说明.md
git add 浏览器加载问题解决方案.md
git add Canvas空白问题修复.md

echo.
echo 查看将要提交的文件...
git status

echo.
echo ========================================
echo 修复内容摘要：
echo ========================================
echo 1. CMakeLists.txt
echo    - 添加 EXPORTED_RUNTIME_METHODS (ccall, cwrap, allocateUTF8)
echo    - 添加 EXPORTED_FUNCTIONS (串口回调函数)
echo.
echo 2. index.html
echo    - 使用 qtCanvasElements 替代已弃用的 canvas
echo    - 设置 canvas 尺寸 (1200x800)
echo    - 增强错误处理和调试信息
echo.
echo 3. test-wasm-loading.html
echo    - 新增诊断工具页面
echo.
echo 4. 工作流文件
echo    - 部署时包含诊断工具
echo    - 验证关键文件
echo.
echo 5. 文档
echo    - 详细的修复说明（中英文）
echo ========================================
echo.

set /p confirm="确认提交这些修改吗？(Y/N): "
if /i "%confirm%" NEQ "Y" (
    echo 取消提交。
    pause
    exit /b
)

echo.
echo 正在提交...
git commit -m "Fix: Export runtime methods and set canvas size for WebAssembly

- 修复 Module.canvas 已弃用警告，使用 qtCanvasElements
- 添加 EXPORTED_RUNTIME_METHODS: ccall, cwrap, allocateUTF8
- 添加 EXPORTED_FUNCTIONS: 串口回调函数
- 修复 TypeError: Cannot read properties of undefined (reading 'apply')
- 设置 canvas 尺寸为 1200x800 解决空白问题
- 增强 index.html 调试功能
- 添加 test-wasm-loading.html 诊断工具
- 更新工作流以部署诊断工具

这些修复解决了 WebAssembly 应用无法加载和 canvas 空白的问题。"

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
echo ✓ 所有操作完成！
echo ========================================
echo.
echo 下一步：
echo 1. 访问 GitHub Actions 查看构建状态
echo    https://github.com/mojianbiao2025-art/SerialDebugger/actions
echo.
echo 2. 等待构建完成（约 10-15 分钟）
echo.
echo 3. 测试修复后的应用
echo    https://mojianbiao2025-art.github.io/SerialDebugger/
echo.
echo 4. 如果需要诊断，访问
echo    https://mojianbiao2025-art.github.io/SerialDebugger/test-wasm-loading.html
echo.
echo 按任意键退出...
pause >nul

# 应用程序图标说明

## 图标设计

串口调试助手的图标采用现代化设计，包含以下元素：
- **蓝色圆形背景**：代表通信和连接
- **DB9串口连接器**：9针串口接口的经典形象
- **绿色信号波形**：表示数据传输和实时通信

## 图标文件

### 生成的文件
- `app_icon.ico` - Windows图标文件（包含多种尺寸：16x16, 32x32, 48x48, 64x64, 128x128, 256x256）
- `app_icon_16.png` - 16x16 PNG图标
- `app_icon_32.png` - 32x32 PNG图标
- `app_icon_48.png` - 48x48 PNG图标
- `app_icon_64.png` - 64x64 PNG图标
- `app_icon_128.png` - 128x128 PNG图标
- `app_icon_256.png` - 256x256 PNG图标

### 资源文件
- `app_icon.rc` - Windows资源脚本（用于嵌入EXE文件图标）
- `resources.qrc` - Qt资源文件（用于窗口图标）

## 图标应用位置

1. **文件图标**：通过 `app_icon.rc` 嵌入到 EXE 文件中，在文件资源管理器中显示
2. **任务栏图标**：Windows任务栏显示的图标
3. **标题栏图标**：窗口左上角显示的小图标
4. **Alt+Tab切换**：任务切换时显示的图标

## 重新生成图标

如果需要修改图标设计，可以：

1. 编辑 `create_icon.py` 脚本
2. 运行脚本重新生成图标：
   ```bash
   python create_icon.py
   ```
3. 重新编译程序：
   ```bash
   .\build_simple.bat
   ```

## 技术实现

### Windows资源文件（app_icon.rc）
```rc
IDI_ICON1 ICON DISCARDABLE "app_icon.ico"
```
这会将图标嵌入到EXE文件中，作为文件的默认图标。

### Qt资源文件（resources.qrc）
```xml
<RCC>
    <qresource prefix="/icons">
        <file>app_icon.ico</file>
        ...
    </qresource>
</RCC>
```
这允许在程序运行时访问图标资源。

### 代码设置
```cpp
// 在 main.cpp 中设置应用程序图标
a.setWindowIcon(QIcon(":/icons/app_icon.ico"));

// 在 mainwindow.cpp 中设置窗口图标
setWindowIcon(QIcon(":/icons/app_icon.ico"));
```

## 依赖项

图标生成脚本需要 Python 和 Pillow 库：
```bash
pip install Pillow
```

## 图标颜色方案

- 主色调：#3498db (蓝色)
- 深蓝色：#2980b9
- 连接器：#ecf0f1 (浅灰)
- 引脚：#34495e (深灰)
- 信号波形：#2ecc71 (绿色)

## 版权信息

图标由程序自动生成，属于串口调试助手项目的一部分。
作者：莫建标
公司：上海大族富创得股份有限公司

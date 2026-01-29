# 语言配置文件说明 / Language Configuration Guide

## 概述 / Overview

本程序使用 INI 格式的配置文件来管理多语言翻译，而不是硬编码在源代码中。

This program uses INI format configuration files to manage multilingual translations instead of hardcoding them in source code.

## 文件格式 / File Format

语言文件命名格式：`lang_XX.ini`，其中 XX 是语言代码。

Language files are named: `lang_XX.ini`, where XX is the language code.

### 支持的语言 / Supported Languages

- `lang_en.ini` - English (英语)
- `lang_zh.ini` - Chinese (中文)
- `lang_ja.ini` - Japanese (日本語)
- `lang_de.ini` - German (Deutsch)
- `lang_fr.ini` - French (Français)

## 文件结构 / File Structure

每个语言文件包含以下部分：

Each language file contains the following sections:

```ini
[General]
language_name=语言名称
language_code=语言代码

[UI]
# 用户界面文本
window_title=窗口标题
port_settings=端口设置
...

[Status]
# 状态栏文本
status_disconnected=状态: 未连接
...

[Menu]
# 菜单项文本
menu_file=文件
...

[Plot]
# 绘图相关文本
plot_title=实时数据波形
...
```

## 添加新语言 / Adding New Language

1. 复制 `lang_en.ini` 文件
2. 重命名为 `lang_XX.ini`（XX 为新语言代码）
3. 翻译所有文本内容
4. 将文件放在程序目录下
5. 重启程序，新语言会自动出现在语言菜单中

Steps:
1. Copy `lang_en.ini` file
2. Rename to `lang_XX.ini` (XX is the new language code)
3. Translate all text content
4. Place the file in the program directory
5. Restart the program, the new language will automatically appear in the language menu

## 修改现有翻译 / Modifying Existing Translations

1. 用文本编辑器打开对应的语言文件
2. 修改需要更改的文本
3. 保存文件
4. 重启程序以加载新的翻译

Steps:
1. Open the corresponding language file with a text editor
2. Modify the text that needs to be changed
3. Save the file
4. Restart the program to load the new translations

## 文件位置 / File Location

程序会按以下顺序搜索语言文件：

The program searches for language files in the following order:

1. 当前工作目录 / Current working directory
2. 程序所在目录 / Application directory
3. 资源文件（如果编译时包含）/ Resource files (if included during compilation)

## 注意事项 / Notes

- 文件必须使用 UTF-8 编码保存
- 特殊字符（如换行符）使用 `\n` 表示
- 键名（等号左边）不要修改，只修改值（等号右边）
- 如果某个键缺失，程序会使用英文作为后备

Important:
- Files must be saved in UTF-8 encoding
- Special characters (like newlines) use `\n`
- Do not modify key names (left side of =), only modify values (right side)
- If a key is missing, the program will use English as fallback

## 示例 / Example

```ini
[UI]
window_title=Serial Port Debugger
open_port=Open Port
close_port=Close Port
```

修改为中文 / Change to Chinese:

```ini
[UI]
window_title=串口调试助手
open_port=打开串口
close_port=关闭串口
```

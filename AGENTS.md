# MotionGloveUI 工程说明

Qt5 / C++14 界面库（产物 `MotionGloveUI.dll`），被上级工程 `MotionGlove` 以
submodule 方式引入，也可单独 clone 构建。

## 本地工具链（D:\autotools）

以下工具**未加入系统 `PATH`**（`python` 除外，位于 `C:\python\python.exe`），
调用时请使用完整路径。

| 工具 | 用途 | 路径 | 可执行文件 |
| --- | --- | --- | --- |
| Pandoc | 文档格式转换（Markdown ↔ Word/HTML/PDF 等） | `D:\autotools\Pandoc` | `pandoc.exe` |
| wkhtmltopdf | HTML 转 PDF / 图片 | `D:\autotools\wkhtmltopdf\bin` | `wkhtmltopdf.exe` / `wkhtmltoimage.exe` |
| FFmpeg | 音视频转码、剪辑与流处理 | `D:\autotools\ffmpeg\bin` | `ffmpeg.exe` / `ffprobe.exe` |
| Python | Python 运行环境（已在 `PATH`） | `C:\python` | `python.exe` |
| Gost | 端口转发 / 代理隧道（目录内只有 `forward_*.bat`，`gost.exe` 未落地） | `D:\autotools\gost` | 批处理脚本 |
| wget | 命令行文件下载 | `D:\autotools\wget` | `wget.exe` |
| aria2 | 多协议多线程下载（HTTP/HTTPS/FTP/BT/磁力） | `D:\autotools\aria2-1.37.0-win-64bit-build1` | `aria2c.exe` |
| MuPDF | PDF 查看与处理 | `D:\autotools\mupdf` | `mutool.exe` |
| iperf3 | 网络带宽与性能测试 | `D:\autotools\iperf` | `iperf3.exe` |
| Poppler | PDF 转换工具集 | `D:\autotools\pdftohtml` | `pdftotext.exe` / `pdftoppm.exe` / `pdftocairo.exe` / `pdfimages.exe` |
| 7-Zip Extra | 命令行压缩 / 解压（7z / zip / gz / xz / tar） | `D:\autotools\7z` | `7za.exe` |
| Doxygen | 源码文档生成（附 doxyindexer / doxysearch） | `D:\autotools\doxygen-1.11.0` | `doxygen.exe` / `doxywizard.exe` |
| gallery-dl | 命令行图集下载（Pixiv / X / Danbooru 等） | `D:\autotools\gallery_dl` | `gallery-dl.exe` |
| claude-skills | 技能包（pdf / pptx / video-downloader） | `D:\autotools\claude-skills` | — |
| RhinoAI | Rhino 8 MCP 连接 + 技能包 | `D:\autotools\rhinoai` | — |
| Crawl4AI | 网页爬取与数据提取技能包 | `D:\autotools\crawl4ai` | — |

已核对版本：pandoc 3.9 / wkhtmltopdf 0.12.6 / ffmpeg 2026-02-26 / python 3.10.11 /
wget 1.21.4 / aria2 1.37.0 / mutool 1.27.0 / iperf 3.20 / poppler 26.09.0 /
7za 26.04 / doxygen 1.11.0。

调用示例：

```bat
& "D:\autotools\Pandoc\pandoc.exe" --version
& "D:\autotools\doxygen-1.11.0\doxygen.exe" Doxyfile
& "D:\autotools\7z\7za.exe" a lib.7z .\lib
& "D:\autotools\ffmpeg\bin\ffmpeg.exe" -i in.mp4 -c copy out.mkv
```

## 构建（已验证）

```bat
cmake -S . -B out/build/vs-x64-debug -G "Visual Studio 17 2022" ^
      -DCMAKE_BUILD_TYPE=Debug -DXSTYPES_FETCH_VIA_SSH=OFF -A x64
cmake --build out/build/vs-x64-debug --config Debug --target MotionGloveUI
```

更多编译说明见 [README.md](README.md)。

## 套装配置的读写时机（h2Widget）

配置存在 `<exe 目录>/Resources/h2GloveConfig/h2GloveConfig_<HEX SN>.json`，由 MotionGloveLLSDK 读写。
时机约定（改这块前先看 [src/core/h2GloveConfigSync.h](src/core/h2GloveConfigSync.h)）：

| 时机 | 动作 | 入口 |
|---|---|---|
| 任一只手**上线**（离线→在线跳变） | 取配置；缺失则创建默认配置并落盘；随后回填模式单选按钮与各面板控件 | `h2Widget::loadGloveConfigToUI()` |
| 任一只手**下线**（在线→离线跳变） | 把界面当前值叠加到已有配置上再写盘 | `h2Widget::saveGloveConfigFromUI()` |
| 稳态（一直在线/一直离线） | 什么都不做 | `h2core::decideGloveConfigSyncAction()` 返回 `None` |
| 用户拖滑块**松手** | 只保存该滑块的字段 | 各面板的 `onSlider…Released()` |

- **不要**把保存写成"查到配置才写"：全新机器上第一份配置正是靠上线的 `ensure` 创建的，
  早期实现在这里静默返回，导致所有保存都失效。
- 下线保存必须"先取已有配置再叠加"，否则骨骼名、Unity 窗口参数等不由面板管理的字段会被重置。
- `h2Widget::collectGloveConfigFromUI()` 只收集**当前采集模式**那两块面板：三组面板的拇指滑杆
  共用同一个 `angleAdditionEuler[Thumb1]` 字段，全收集会变成"最后遍历到的那组说了算"。
- 相关用例：`tests/unit/h2GloveConfigSync_test.cpp`（跳变判定）、
  `tests/ui/h2GloveConfigSnapshot_ui_test.cpp`（界面值 → 配置字段的映射）。


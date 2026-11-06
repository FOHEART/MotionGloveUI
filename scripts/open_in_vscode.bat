:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前；双解释器脚本须 CRLF、无 BOM。
REM  open_in_vscode.bat -- 用 VSCode 打开工程（替代原根目录两份同名脚本）
REM  用法（Windows）：scripts\open_in_vscode.bat
REM  用法（Linux）  ：sh scripts/open_in_vscode.bat
setlocal EnableExtensions
REM 位于 scripts\ 层：工程根要向上一级
for %%I in ("%~dp0..") do set "ROOT_DIR=%%~fI"

where code >nul 2>nul
if errorlevel 1 (
    echo [ERROR] 未找到 code 命令。安装 VSCode 时请勾选 "Add to PATH"，
    echo         或把 Microsoft VS Code\bin 加入 PATH 后重试。
    exit /b 1
)

code "%ROOT_DIR%"
exit /b %errorlevel%

#__SH__
cd "$(dirname "$0")/.." || exit 1

if ! command -v code >/dev/null 2>&1; then
    echo "[ERROR] 未找到 code 命令。请先安装 VSCode 并在命令面板执行" >&2
    echo '        "Shell Command: Install '\''code'\'' command in PATH"。' >&2
    exit 1
fi

code "$PWD"

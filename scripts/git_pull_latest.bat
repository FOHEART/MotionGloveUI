:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前；双解释器脚本须 CRLF、无 BOM。
REM  git_pull_latest.bat -- 拉取最新代码（替代原根目录 [Windows]git_pull_latest.cmd / [Linux]git_pull_latest.sh）
REM  用法（Windows）：scripts\git_pull_latest.bat
REM  用法（Linux）  ：sh scripts/git_pull_latest.bat
setlocal EnableExtensions
for %%I in ("%~dp0..") do set "ROOT_DIR=%%~fI"
cd /d "%ROOT_DIR%"

echo [fetch_latest] Fetching latest from origin...
git fetch origin
if errorlevel 1 goto :fetch_failed

git pull origin main
if errorlevel 1 goto :pull_failed

echo.
echo [fetch_latest] Done. Current status:
git --no-pager log --oneline -5
exit /b 0

:fetch_failed
echo [ERROR] git fetch failed.
exit /b 1

:pull_failed
echo [ERROR] git pull failed.
exit /b 1

#__SH__
cd "$(dirname "$0")/.." || exit 1

echo "[fetch_latest] Fetching latest from origin..."
git fetch origin || { echo "[ERROR] git fetch failed."; exit 1; }

git pull origin main || { echo "[ERROR] git pull failed."; exit 1; }

echo ""
echo "[fetch_latest] Done. Current status:"
git --no-pager log --oneline -5

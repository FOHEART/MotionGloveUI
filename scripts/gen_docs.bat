:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前；双解释器脚本须 CRLF、无 BOM。
REM  gen_docs.bat -- Doxygen 文档生成（有告警即失败）
REM  doxygen 路径：优先环境变量 DOXYGEN，默认本机 D:\autotools\doxygen-1.11.0；Linux 用 PATH
setlocal
if not defined DOXYGEN set "DOXYGEN=D:\autotools\doxygen-1.11.0\doxygen.exe"
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "LOGFILE=%ROOT%\out\doxygen\doxygen_warnings.log"
set "RAWLOG=%ROOT%\docs\doxygen_warnings.log"

if not exist "%DOXYGEN%" (
    echo [ERROR] doxygen not found: %DOXYGEN%
    echo         可用 set DOXYGEN=路径 指定，或安装 doxygen 后重试。
    exit /b 2
)

pushd "%ROOT%"
REM WARN_LOGFILE = docs/doxygen_warnings.log 按运行时的当前目录解析，必须先到工程根
if not exist "out\doxygen" md "out\doxygen" 2>nul
"%DOXYGEN%" Doxyfile
set "DOX_RC=%errorlevel%"
if exist "%RAWLOG%" move /y "%RAWLOG%" "%LOGFILE%" >nul 2>nul
if not "%DOX_RC%"=="0" (
    echo.
    echo [FAILED] doxygen failed, exit code: %DOX_RC%
    if exist "%LOGFILE%" type "%LOGFILE%"
    popd
    exit /b 1
)
REM WARN_AS_ERROR 未在 Doxyfile 打开时，非空告警日志同样判为失败
if exist "%LOGFILE%" for %%S in ("%LOGFILE%") do if not %%~zS EQU 0 (
    echo.
    echo [FAILED] doxygen reported warnings, see %LOGFILE%
    type "%LOGFILE%"
    popd
    exit /b 1
)

echo.
echo [OK] html generated: %ROOT%\docs\html\index.html
start "" "%ROOT%\docs\html\index.html"
popd
exit /b 0

#__SH__
# POSIX 段
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

DOXYGEN_BIN=${DOXYGEN:-doxygen}
if ! command -v "$DOXYGEN_BIN" >/dev/null 2>&1; then
    echo "[ERROR] doxygen not found: $DOXYGEN_BIN" >&2
    echo "        请安装 doxygen（如 sudo apt install doxygen），或用 DOXYGEN=/path/to/doxygen 指定。" >&2
    exit 2
fi

mkdir -p out/doxygen
"$DOXYGEN_BIN" Doxyfile
rc=$?
[ -f docs/doxygen_warnings.log ] && mv -f docs/doxygen_warnings.log out/doxygen/doxygen_warnings.log
if [ "$rc" -ne 0 ] || [ -s out/doxygen/doxygen_warnings.log ]; then
    echo ""
    echo "[FAILED] doxygen failed or reported warnings, see out/doxygen/doxygen_warnings.log"
    [ -f out/doxygen/doxygen_warnings.log ] && cat out/doxygen/doxygen_warnings.log
    exit 1
fi

echo ""
echo "[OK] html generated: $ROOT/docs/html/index.html"

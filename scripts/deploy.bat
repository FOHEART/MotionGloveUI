:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前；双解释器脚本须 CRLF、无 BOM。
REM  deploy.bat -- 把 MotionGloveUI 的库产物与公共头文件汇总到 out\install
REM  （产物在构建树里，源码树不再生成 lib\；刻意不用 cmake --install，
REM   否则会连带执行 CPM 依赖的 install 规则，把上游头文件/静态库装进来）
REM  用法（Windows）：scripts\deploy.bat [Debug|Release] [构建树目录]
REM  用法（Linux）  ：sh scripts/deploy.bat [Debug|Release] [构建树目录]
setlocal EnableDelayedExpansion
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
cd /d "%ROOT%"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=all"
if /i "%CONFIG%"=="all" (
    call "%~f0" Debug
    if errorlevel 1 exit /b 1
    call "%~f0" Release
    exit /b !errorlevel!
)

set "BDIR=%~2"
if "%BDIR%"=="" set "BDIR=out\build\x64-%CONFIG%"
if not exist "%BDIR%\CMakeCache.txt" (
    echo [ERROR] 未找到构建树 %BDIR%，请先运行 scripts\build.bat。
    exit /b 1
)

set "INSTALL=%ROOT%\out\install\x64-%CONFIG%"

echo   -^> 汇总产物到 %INSTALL%
REM 先清空安装子目录，保证幂等重跑（例如头文件改名后不残留旧文件）
if exist "%INSTALL%\include" rd /s /q "%INSTALL%\include"
if exist "%INSTALL%\lib" rd /s /q "%INSTALL%\lib"
md "%INSTALL%\include" 2>nul
md "%INSTALL%\lib" 2>nul

copy /y "%ROOT%\include\*.h" "%INSTALL%\include\" >nul 2>nul

REM 库产物在构建树\<配置>\（CMakeLists 固定 RUNTIME/LIBRARY/ARCHIVE 到该目录）
set "LIBSRC=%BDIR%\%CONFIG%"
if not exist "%LIBSRC%\MotionGloveUI.dll" if not exist "%LIBSRC%\MotionGloveUI.lib" set "LIBSRC=%BDIR%"
copy /y "%LIBSRC%\MotionGloveUI.dll" "%INSTALL%\lib\" >nul 2>nul
copy /y "%LIBSRC%\MotionGloveUI.lib" "%INSTALL%\lib\" >nul 2>nul

set "HDR_COUNT=0"
set "LIB_COUNT=0"
for /f "delims=" %%F in ('dir /b /a-d "%INSTALL%\include\*.h" 2^>nul') do set /a HDR_COUNT+=1
for /f "delims=" %%F in ('dir /b /a-d "%INSTALL%\lib\*" 2^>nul') do set /a LIB_COUNT+=1
echo      %INSTALL%：!HDR_COUNT! 个头文件，!LIB_COUNT! 个库文件
if "!HDR_COUNT!"=="0" (
    echo [ERROR] 未汇总到头文件，请检查 include\ 目录。
    exit /b 1
)
if "!LIB_COUNT!"=="0" (
    echo [ERROR] 未汇总到 MotionGloveUI 库产物，请先完成构建。
    exit /b 1
)
exit /b 0

#__SH__
# POSIX 段
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

CONFIG=${1:-all}
if [ "$CONFIG" = "all" ]; then
    sh "$(dirname "$0")/deploy.bat" Debug || exit 1
    sh "$(dirname "$0")/deploy.bat" Release || exit 1
    exit 0
fi

BDIR=${2:-out/build/x64-$CONFIG}
if [ ! -f "$BDIR/CMakeCache.txt" ]; then
    echo "[ERROR] 未找到构建树 $BDIR，请先运行 sh scripts/build.bat。"
    exit 1
fi

INSTALL="$ROOT/out/install/x64-$CONFIG"

echo "  -> 汇总产物到 $INSTALL"
# 先清空安装子目录，保证幂等重跑；库产物在构建树里（源码树不再生成 lib\）
rm -rf "$INSTALL/include" "$INSTALL/lib"
mkdir -p "$INSTALL/include" "$INSTALL/lib"

cp "$ROOT"/include/*.h "$INSTALL/include/" 2>/dev/null

# 库产物在构建树/<配置>/（CMakeLists 固定 RUNTIME/LIBRARY/ARCHIVE 到该目录）
LIBSRC="$BDIR/$CONFIG"
[ -f "$LIBSRC/libMotionGloveUI.so" ] || [ -f "$LIBSRC/libMotionGloveUI.a" ] || LIBSRC="$BDIR"

count=0
for f in "$LIBSRC"/libMotionGloveUI.so* "$LIBSRC"/libMotionGloveUI.a "$LIBSRC"/MotionGloveUI.dll "$LIBSRC"/MotionGloveUI.lib; do
    [ -f "$f" ] || continue
    cp "$f" "$INSTALL/lib/" || exit 1
    count=$((count + 1))
done

hdr=$(find "$INSTALL/include" -maxdepth 1 -name '*.h' | wc -l | tr -d ' ')
echo "     $INSTALL：$hdr 个头文件，$count 个库文件"
if [ "$hdr" -eq 0 ]; then
    echo "[ERROR] 未汇总到头文件，请检查 include/ 目录。"
    exit 1
fi
if [ "$count" -eq 0 ]; then
    echo "[ERROR] 未汇总到 MotionGloveUI 库产物，请先完成构建。"
    exit 1
fi
if [ "$count" -gt 0 ]; then
    echo "     运行链接了 MotionGloveUI 动态库的程序前可执行：export LD_LIBRARY_PATH=\"$INSTALL/lib:\$LD_LIBRARY_PATH\""
fi
exit 0

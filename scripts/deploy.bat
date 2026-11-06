:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前；双解释器脚本须 CRLF、无 BOM。
REM  deploy.bat -- cmake --install + 汇总公共头文件与库产物到 out\install
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

echo   -^> 安装到 %INSTALL%
cmake --install "%BDIR%" --config %CONFIG% --prefix "%INSTALL%"
if errorlevel 1 (
    echo [ERROR] cmake --install failed, exit code: !errorlevel!
    exit /b 1
)

echo   -^> 汇总产物到 %INSTALL%
REM 只清空汇总子目录（幂等重跑），保留 cmake --install 的其它安装产物
if exist "%INSTALL%\include" rd /s /q "%INSTALL%\include"
if exist "%INSTALL%\lib" rd /s /q "%INSTALL%\lib"
md "%INSTALL%\include" 2>nul
md "%INSTALL%\lib" 2>nul

REM 公共头文件在 include\；库产物由 CMakeLists 输出到源码树 lib\<配置>
copy /y "%ROOT%\include\*.h" "%INSTALL%\include\" >nul 2>nul
set "LIBSRC=%ROOT%\lib\%CONFIG%"
if not exist "%LIBSRC%" set "LIBSRC=%BDIR%\%CONFIG%"
if not exist "%LIBSRC%" set "LIBSRC=%BDIR%"
copy /y "%LIBSRC%\*.lib" "%INSTALL%\lib\" >nul 2>nul
copy /y "%LIBSRC%\*.dll" "%INSTALL%\lib\" >nul 2>nul

set "HDR_COUNT=0"
set "LIB_COUNT=0"
for /f "delims=" %%F in ('dir /b /a-d "%INSTALL%\include\*.h" 2^>nul') do set /a HDR_COUNT+=1
for /f "delims=" %%F in ('dir /b /a-d "%INSTALL%\lib\*.lib" "%INSTALL%\lib\*.dll" 2^>nul') do set /a LIB_COUNT+=1
echo      %INSTALL%：!HDR_COUNT! 个头文件，!LIB_COUNT! 个库文件
if "!HDR_COUNT!"=="0" (
    echo [ERROR] 未汇总到头文件，请检查 include\ 目录。
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

echo "  -> 安装到 $INSTALL"
cmake --install "$BDIR" --prefix "$INSTALL" || { echo "[ERROR] install failed"; exit 1; }

echo "  -> 汇总产物到 $INSTALL"
rm -rf "$INSTALL/include" "$INSTALL/lib"
mkdir -p "$INSTALL/include" "$INSTALL/lib"

cp "$ROOT"/include/*.h "$INSTALL/include/" 2>/dev/null

# CMakeLists 把库产物输出到源码树的 lib/<配置>；单配置生成器也可能落在构建树根
LIBSRC="$ROOT/lib/$CONFIG"
[ -d "$LIBSRC" ] || LIBSRC="$BDIR"

count=0
for f in "$LIBSRC"/libMotionGloveUI.so* "$LIBSRC"/libMotionGloveUI.a "$LIBSRC"/*.dll "$LIBSRC"/*.lib; do
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
if [ "$count" -gt 0 ]; then
    echo "     运行链接了 MotionGloveUI 动态库的程序前可执行：export LD_LIBRARY_PATH=\"$INSTALL/lib:\$LD_LIBRARY_PATH\""
fi
exit 0

:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前；双解释器脚本须 CRLF、无 BOM。
REM  build.bat -- 配置 + 编译 + 安装 + 部署 MotionGloveUI
REM  用法（Windows）：scripts\build.bat [2022|2026] [Debug|Release]
REM  用法（Linux）  ：sh scripts/build.bat [Debug|Release]
REM  环境变量：QTDIR（Qt5 前缀）、XSTYPES_FETCH_VIA_SSH（ON/OFF，默认 OFF，HTTPS 拉私有依赖）
setlocal EnableDelayedExpansion
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
cd /d "%ROOT%"

set "VS_VER=2022"
set "CONFIG=all"
if not "%~1"=="" for %%A in (%*) do (
    if /i "%%~A"=="2022" set "VS_VER=2022"
    if /i "%%~A"=="2026" set "VS_VER=2026"
    if /i "%%~A"=="Debug" set "CONFIG=Debug"
    if /i "%%~A"=="Release" set "CONFIG=Release"
)

if "%VS_VER%"=="2026" (
    set "GENERATOR=Visual Studio 18 2026"
    set "VS_TAG=Visual Studio 2026"
) else (
    set "GENERATOR=Visual Studio 17 2022"
    set "VS_TAG=Visual Studio 2022"
)

REM Qt5 前缀：优先 QTDIR，其次本机默认安装路径；都没有就交给 find_package 自己找
set "QT_ARGS="
if defined QTDIR (
    set "QT_ARGS=-DCMAKE_PREFIX_PATH=!QTDIR!"
) else (
    if exist "D:\Qt5.15.2\5.15.2\msvc2019_64\bin\Qt5Core.dll" set "QT_ARGS=-DCMAKE_PREFIX_PATH=D:\Qt5.15.2\5.15.2\msvc2019_64"
)
set "DEP_ARGS=-DXSTYPES_FETCH_VIA_SSH=OFF"
if defined XSTYPES_FETCH_VIA_SSH set "DEP_ARGS=-DXSTYPES_FETCH_VIA_SSH=!XSTYPES_FETCH_VIA_SSH!"

echo [1/2] 构建（%VS_TAG%，配置：%CONFIG%）...
REM 用平铺的 goto 分派而不是把 call :label 塞进 if (...) 括号块，便于 cmd 排障
if /i "%CONFIG%"=="all" goto :build_all
call :build_config %CONFIG%
if errorlevel 1 goto :build_failed
goto :build_done

:build_all
call :build_config Debug
if errorlevel 1 goto :build_failed
call :build_config Release
if errorlevel 1 goto :build_failed

:build_done
echo [2/2] 完成：out\build\x64-Debug 与 out\build\x64-Release 见 out\ 下
exit /b 0

REM -- 配置 + 编译 + 部署指定配置 --------------------------------------------
:build_config
set "CFG=%~1"
set "BDIR=out\build\x64-%CFG%"
echo   -^> 配置 %BDIR%
cmake -S "%ROOT%" -B "%BDIR%" -G "%GENERATOR%" -A x64 ^
    -DCMAKE_CONFIGURATION_TYPES="Debug;Release" ^
    -DCMAKE_INSTALL_PREFIX="%ROOT%\out\install\x64-%CFG%" %QT_ARGS% %DEP_ARGS%
if errorlevel 1 (
    echo [ERROR] CMake configuration failed, exit code: !errorlevel!
    exit /b 1
)

echo   -^> 编译 %CFG%
cmake --build "%BDIR%" --config %CFG% --parallel
if errorlevel 1 (
    echo [ERROR] Build failed, exit code: !errorlevel!
    exit /b 1
)

call "%~dp0deploy.bat" %CFG% "%BDIR%"
exit /b !errorlevel!

:build_failed
echo.
echo [ERROR] 构建未完成，请检查上面的错误输出。
exit /b 1

#__SH__
# POSIX 段（Linux：CMake 默认生成器，单配置）
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

CONFIGS=""
for arg in "$@"; do
    case "$arg" in
        [Dd]ebug)   CONFIGS="$CONFIGS Debug" ;;
        [Rr]elease) CONFIGS="$CONFIGS Release" ;;
        2022|2026)  echo "[INFO] Linux 下忽略参数 $arg（使用 CMake 默认生成器）" ;;
        *)          echo "[WARN] 忽略未知参数: $arg" ;;
    esac
done
[ -z "$CONFIGS" ] && CONFIGS="Debug Release"

JOBS=$( (command -v nproc >/dev/null 2>&1 && nproc) || echo 4 )
DEP_ARG="-DXSTYPES_FETCH_VIA_SSH=${XSTYPES_FETCH_VIA_SSH:-OFF}"

build_one() {
    cfg=$1
    bdir="out/build/x64-$cfg"
    inst="$ROOT/out/install/x64-$cfg"

    echo "[1/2] 构建 $cfg（$bdir）..."
    if [ -n "${QTDIR:-}" ]; then
        cmake -S "$ROOT" -B "$bdir" -DCMAKE_BUILD_TYPE="$cfg" \
              -DCMAKE_INSTALL_PREFIX="$inst" -DCMAKE_PREFIX_PATH="$QTDIR" "$DEP_ARG" || return 1
    else
        cmake -S "$ROOT" -B "$bdir" -DCMAKE_BUILD_TYPE="$cfg" \
              -DCMAKE_INSTALL_PREFIX="$inst" "$DEP_ARG" || return 1
    fi
    cmake --build "$bdir" --target MotionGloveUI -j"$JOBS" || return 1

    sh "$(dirname "$0")/deploy.bat" "$cfg" "$bdir" || return 1
}

for cfg in $CONFIGS; do
    build_one "$cfg" || { echo ""; echo "[ERROR] 构建未完成，请检查上面的错误输出。"; exit 1; }
done

echo "[2/2] 完成：out/build/x64-* / out/install/x64-*"

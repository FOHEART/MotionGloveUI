@echo off
chcp 65001 >nul
REM  chcp 必须在任何中文之前！本文件是被 call 的深层子脚本，CRLF + chcp 顺序错了
REM  顶层入口全对也会解析错位，症状是碎片命令报错（'xxx' is not recognized）。
REM ============================================================================
REM  build_and_test.bat -- Windows 单元测试链：探测 VS + 配置 + 构建 + ctest
REM  用法（由 test.bat 转发）：scripts\lib\build_and_test.bat [Debug|Release] [ctest -R 正则]
REM  环境变量：QTDIR（Qt5 前缀）、XSTYPES_FETCH_VIA_SSH（ON/OFF，默认 OFF）
REM ============================================================================
setlocal EnableDelayedExpansion
for %%I in ("%~dp0..\..") do set "ROOT=%%~fI"
cd /d "%ROOT%"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"
set "FILTER=%~2"
set "BUILD=%ROOT%\out\build\tdd-tests"

REM vswhere 输出先落临时文件再读：-property 命令里若含括号会被 for /f 的命令子句提前闭合
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSW_PATH=%TEMP%\mgui_test_vs_path.txt"
set "VSW_VER=%TEMP%\mgui_test_vs_ver.txt"
if exist "%VSW_PATH%" del "%VSW_PATH%" >nul 2>nul
if exist "%VSW_VER%" del "%VSW_VER%" >nul 2>nul
if exist "%VSWHERE%" (
    "%VSWHERE%" -prerelease -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%VSW_PATH%" 2>nul
    "%VSWHERE%" -prerelease -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion > "%VSW_VER%" 2>nul
)
if not exist "%VSW_PATH%" > "%VSW_PATH%" echo.
if not exist "%VSW_VER%" > "%VSW_VER%" echo.
set "VSROOT="
for /f "usebackq delims=" %%I in ("%VSW_PATH%") do if not defined VSROOT set "VSROOT=%%I"
set "VS_VER="
for /f "usebackq delims=" %%V in ("%VSW_VER%") do if not defined VS_VER set "VS_VER=%%V"
set "VS_MAJOR="
for /f "usebackq tokens=1 delims=." %%M in ("%VSW_VER%") do if not defined VS_MAJOR set "VS_MAJOR=%%M"
del "%VSW_PATH%" "%VSW_VER%" >nul 2>nul

if not defined VSROOT (
    echo [ERROR] 未在本机找到带 C++ 工具链的 Visual Studio，请检查 vswhere 或安装 VS。
    exit /b 2
)
REM 生成器名必须由 installationVersion 主版本推导；不要用 catalog_productLineVersion（预览版可能为空）
if "%VS_MAJOR%"=="18" (
    set "GENERATOR=Visual Studio 18 2026"
) else if "%VS_MAJOR%"=="16" (
    set "GENERATOR=Visual Studio 16 2019"
) else (
    set "GENERATOR=Visual Studio 17 2022"
)
echo [INFO] VSROOT=%VSROOT%
echo [INFO] installationVersion=%VS_VER% -^> %GENERATOR%

set "VCVARS=%VSROOT%\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" (
    echo [ERROR] vcvars64.bat not found: %VCVARS%
    exit /b 2
)
call "%VCVARS%" >nul
if errorlevel 1 (
    echo [ERROR] failed to load Visual Studio environment
    exit /b 2
)

set "QT_ARGS="
if defined QTDIR (
    set "QT_ARGS=-DCMAKE_PREFIX_PATH=!QTDIR!"
    set "PATH=!QTDIR!\bin;!PATH!"
)
set "DEP_ARGS=-DXSTYPES_FETCH_VIA_SSH=OFF"
if defined XSTYPES_FETCH_VIA_SSH set "DEP_ARGS=-DXSTYPES_FETCH_VIA_SSH=!XSTYPES_FETCH_VIA_SSH!"

echo [1/3] configure: %BUILD%
cmake -S "%ROOT%" -B "%BUILD%" -G "%GENERATOR%" -A x64 ^
    -DCMAKE_CONFIGURATION_TYPES="Debug;Release" ^
    -DMOTIONGLOVE_UI_BUILD_TESTS=ON %QT_ARGS% %DEP_ARGS%
if errorlevel 1 (
    echo [ERROR] cmake configure failed
    exit /b 1
)

echo [2/3] build: 全部目标（含依赖侧测试）[%CONFIG%]
REM CPM 依赖同样注册了 ctest 用例，只编本项目两个目标会让它们 Not Run
cmake --build "%BUILD%" --config %CONFIG%
if errorlevel 1 (
    echo [ERROR] build failed
    exit /b 1
)

echo [3/3] ctest [%CONFIG%]
cd /d "%BUILD%"
if "%FILTER%"=="" (
    ctest -N -C %CONFIG% | findstr /r /c:"Total Tests: [1-9]" >nul
    if errorlevel 1 (
        echo [ERROR] ctest 没有发现任何用例（构建失败或测试未注册）
        exit /b 1
    )
    ctest --output-on-failure -C %CONFIG%
) else (
    ctest -N -C %CONFIG% -R "%FILTER%" | findstr /r /c:"Total Tests: [1-9]" >nul
    if errorlevel 1 (
        echo [ERROR] 过滤器 %FILTER% 没有匹配到任何用例（构建失败或用例名写错）
        exit /b 1
    )
    ctest --output-on-failure -C %CONFIG% -R "%FILTER%"
)
if errorlevel 1 (
    echo [FAILED] ctest failed
    exit /b 1
)
echo [OK] all tests passed
exit /b 0

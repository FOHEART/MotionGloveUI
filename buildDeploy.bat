@echo off
chcp 65001 >nul
REM  chcp 必须在任何中文（多字节）内容之前执行，且本文件须保存为 CRLF、无 BOM。
REM  buildDeploy.bat -- 双击入口：检测本机 VS 工具链，菜单/倒计时选择后串起全流程。
REM  用法：
REM    buildDeploy.bat                    检测 + 菜单（只检测到一套时倒计时直跑）
REM    buildDeploy.bat 2022 / 2026        跳过菜单
REM    buildDeploy.bat [2022|2026] [Debug|Release]
REM  流程：[0/4] 清理 out\ -^> [1/4] 构建部署 -^> [2/4] 测试 -^> [3/4] 文档 -^> [4/4] 打包
REM ============================================================================
@setlocal EnableDelayedExpansion
cd /d "%~dp0"

REM 参数里已带年份时跳过检测与菜单
set "VS_VER="
for %%A in (%*) do (
    if /i "%%~A"=="2022" set "VS_VER=2022"
    if /i "%%~A"=="2026" set "VS_VER=2026"
)
if defined VS_VER goto :run

set "COUNT=0"
call :detect_toolchain 2022 "[17.0,18.0)"
call :detect_toolchain 2026 "[18.0,19.0)"
if "%COUNT%"=="0" goto :none_found
if "%COUNT%"=="1" goto :auto_select

:menu
echo.
echo  本机检测到以下 Visual Studio 工具链：
for /l %%I in (1,1,%COUNT%) do echo    [%%I] Visual Studio !YEAR_%%I!      !INST_%%I!
echo.
set "SEL="
set /p "SEL=请选择用于本次编译的工具链 [1-%COUNT%]（回车=1）: "
if not defined SEL set "SEL=1"
if "%SEL%"=="" set "SEL=1"
set "VS_VER="
for /l %%I in (1,1,%COUNT%) do if "%%I"=="%SEL%" set "VS_VER=!YEAR_%%I!"
if not defined VS_VER (
    echo [ERROR] 无效选择：%SEL%，请输入 1-%COUNT% 之间的编号。
    goto :menu
)
echo.
echo  已选择 Visual Studio %VS_VER%，开始全流程...
echo.
goto :run

REM -- 只检测到一套工具链：不询问，倒计时 3 秒后自动开始（Ctrl+C 可取消）----
:auto_select
echo.
echo  仅检测到一个工具链：Visual Studio !YEAR_1!（!INST_1!）
echo  3 秒后自动开始全流程，按 Ctrl+C 可取消...
for /l %%I in (3,-1,1) do (
    <nul set /p "countdown=%%I "
    ping -n 2 127.0.0.1 >nul
)
echo.
set "VS_VER=!YEAR_1!"
goto :run

:run
REM 透传其余参数（Debug/Release），年份已单独解析，避免重复传递
set "REMAIN="
for %%A in (%*) do (
    if /i not "%%~A"=="2022" if /i not "%%~A"=="2026" set "REMAIN=!REMAIN! %%~A"
)

echo [0/4] 清理旧产物 out\ ...
if exist "%~dp0out" rmdir /s /q "%~dp0out"
if exist "%~dp0out" (
    echo [ERROR] out\ 清理失败，可能有文件正被占用，请关闭后重试。
    exit /b 1
)
echo.

echo [1/4] 编译 + 安装 + 部署（Visual Studio %VS_VER%，配置：!REMAIN!）...
call "%~dp0scripts\build.bat" %VS_VER%%REMAIN%
if errorlevel 1 goto :failed
echo.

echo [2/4] 单元测试（out\build\tdd-tests）...
call "%~dp0scripts\test.bat"%REMAIN%
if errorlevel 1 goto :failed
echo.

echo [3/4] 生成文档（Doxygen）...
call "%~dp0scripts\gen_docs.bat"
if errorlevel 1 goto :failed
echo.

echo [4/4] 打源码分发包...
call "%~dp0scripts\package.bat"
if errorlevel 1 goto :failed

echo.
echo  全流程完成：
echo    编译/部署：out\install\x64-Debug、out\install\x64-Release
echo    单元测试：out\build\tdd-tests（结果见上方 ctest 输出）
echo    文档    ：docs\html\index.html
echo    源码包  ：out\pack\MotionGloveUI_v*.zip
exit /b 0

:failed
echo.
echo [ERROR] 流程在当前步骤失败，请检查上方输出。
exit /b 1

REM -- 检测指定版本范围的 VS 工具链，命中则登记为第 COUNT 项 -----------------
REM vswhere 的 -version 范围参数带右括号，不能放进 for /f 的命令子句，
REM 因此先重定向到临时文件，再用 for /f 读文件。
:detect_toolchain
set "YEAR=%~1"
set "RANGE=%~2"
set "INST="
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSW_OUT=%TEMP%\mgui_vswhere_%YEAR%.txt"
if exist "%VSW_OUT%" del "%VSW_OUT%" >nul 2>nul
if exist "%VSWHERE%" "%VSWHERE%" -prerelease -version %RANGE% -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%VSW_OUT%" 2>nul
if not exist "%VSW_OUT%" > "%VSW_OUT%" echo.
for /f "usebackq delims=" %%I in ("%VSW_OUT%") do (
    if not defined INST set "INST=%%I"
)
del "%VSW_OUT%" 2>nul
if not defined INST (
    dir /b "%ProgramFiles%\Microsoft Visual Studio\%YEAR%\*\MSBuild\Current\Bin\MSBuild.exe" >nul 2>nul && set "INST=%ProgramFiles%\Microsoft Visual Studio\%YEAR%"
)
if not defined INST (
    dir /b "%ProgramFiles(x86)%\Microsoft Visual Studio\%YEAR%\*\MSBuild\Current\Bin\MSBuild.exe" >nul 2>nul && set "INST=%ProgramFiles(x86)%\Microsoft Visual Studio\%YEAR%"
)
if defined INST (
    set /a COUNT+=1
    set "YEAR_!COUNT!=%YEAR%"
    set "INST_!COUNT!=%INST%"
)
exit /b 0

:none_found
echo [ERROR] 未在本机检测到带 C++ 工具链的 Visual Studio（2022 / 2026）。
echo         请安装 Visual Studio 并勾选"使用 C++ 的桌面开发"工作负载后重试；
echo         或在命令行直接运行 scripts\build.bat 2022 / 2026 指定工具链。
exit /b 1

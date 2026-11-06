@echo off
chcp 65001 >nul
REM  wsl.bat -- 在 WSL 里跑同一份一键脚本的 POSIX 分支（本机预验证，不必等 CI 红一轮）
REM
REM  用法：scripts\wsl\wsl.bat [sync^|build^|test^|docs^|verify] [参数...]
REM    sync    只把仓库同步到 WSL 家目录副本（默认 ~/MotionGloveUI）
REM    build   副本内跑 scripts/build.bat（参数：Debug^|Release，默认 Release）
REM    test    副本内跑 scripts/test.bat（参数：Debug^|Release，再可选 ctest -R 正则）
REM    docs    副本内跑 scripts/gen_docs.bat（用 WSL 自带的 doxygen 复验门禁）
REM    verify  依次 build -^> test -^> docs，任一失败即停（不带参数时的默认动作）
REM
REM  环境变量：
REM    WSL_DISTRO  发行版名称（默认 Ubuntu-24.04，用 wsl -l -v 查看）
REM    WSL_DEST    副本目录（bash 侧默认 ~/MotionGloveUI）
REM    WSL_QTDIR   WSL 里的 Qt 前缀（默认用系统 Qt5，交给脚本 find_package）
REM
REM  两个必须记住的细节：
REM    1) wsl.exe 的 -d 参数不吃引号：写 -d "%NAME%" 会报 WSL_E_DISTRO_NOT_FOUND，
REM       必须写成 -d %NAME%（发行版名含空格时才需要引号，实际几乎不会出现）
REM    2) 本文件是 cmd 专属入口（在 WSL 里跑它没有意义），故不做双解释器首行
setlocal EnableExtensions EnableDelayedExpansion

set "TASK=%~1"
if "%TASK%"=="" set "TASK=verify"
set "REST=%*"
for /f "tokens=1,*" %%A in ("%REST%") do set "REST=%%B"

if not defined WSL_DISTRO set "WSL_DISTRO=Ubuntu-24.04"

for %%I in ("%~dp0..\..") do set "ROOT=%%~fI"
call :to_posix "%ROOT%"
if not defined POSIX (
    echo [ERROR] 无法把仓库路径转成 WSL 路径：%ROOT%
    exit /b 2
)

echo [wsl] 发行版=%WSL_DISTRO%  仓库=%POSIX%  任务=%TASK%
wsl -d %WSL_DISTRO% -- true >nul 2>&1
if errorlevel 1 (
    echo [ERROR] 无法进入发行版 %WSL_DISTRO%，用 wsl -l -v 查看名称，或设置 WSL_DISTRO
    exit /b 2
)

if /i "%TASK%"=="sync" goto :sync
if /i "%TASK%"=="build" goto :build
if /i "%TASK%"=="test" goto :test
if /i "%TASK%"=="docs" goto :docs
if /i "%TASK%"=="verify" goto :verify
echo [ERROR] 未知任务 "%TASK%"，可用：sync build test docs verify
exit /b 2

:sync
call :run sync
exit /b %errorlevel%

:build
call :run build
exit /b %errorlevel%

:test
call :run test
exit /b %errorlevel%

:docs
call :run docs
exit /b %errorlevel%

:verify
call :run build
if errorlevel 1 exit /b %errorlevel%
call :run test
if errorlevel 1 exit /b %errorlevel%
call :run docs
exit /b %errorlevel%

:run
REM  %~1 = 任务名；%REST% 透传给 bash 侧脚本（如 Release、ctest -R 正则）。
REM  MOTIONGLOVE_UI_WSL_SOURCE 必须显式传：脚本住在副本里时 $0 推出的仓库根是副本自己，
REM  不传源路径的话 rsync 会退化成"自我复制"，工作区新改动永远同步不过去。
wsl -d %WSL_DISTRO% -- env MOTIONGLOVE_UI_WSL_SOURCE=%POSIX% bash "%POSIX%/scripts/wsl/%~1.sh" %REST%
exit /b %errorlevel%

:to_posix
REM  把 D:\a\b 转成 /mnt/d/a/b（盘符转小写），结果写入变量 POSIX
setlocal EnableDelayedExpansion
set "p=%~1"
set "p=!p:\=/!"
set "d=!p:~0,1!"
for %%L in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do if /i "!d!"=="%%L" set "d=%%L"
set "out=/mnt/!d!!p:~2!"
endlocal & set "POSIX=%out%"
exit /b 0

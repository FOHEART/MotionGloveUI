:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前；双解释器脚本须 CRLF、无 BOM。
REM  release.bat -- 按版本号唯一来源（Doxyfile 的 PROJECT_NUMBER）打 tag 并推送
REM  用法：scripts\release.bat [--force]
REM  行为：解析版本 -^> 校验工作树干净 -^> 校验本地/远程无同名 tag -^> 创建 annotated tag 并推送
setlocal EnableDelayedExpansion
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
cd /d "%ROOT%"

set "FORCE="
if /i "%~1"=="--force" set "FORCE=1"

set "DOXYFILE=%ROOT%\Doxyfile"
if not exist "%DOXYFILE%" (
    echo [ERROR] not found: %DOXYFILE%
    exit /b 1
)

REM 解析锚点行：PROJECT_NUMBER = "x.y.z"（tokens=3 取带引号串，%%~V 去引号）
set "VER="
for /f "tokens=3" %%V in ('findstr /b /c:"PROJECT_NUMBER " "%DOXYFILE%"') do set "VER=%%~V"
if not defined VER set "VER=1.0.0"
set "TAG=v%VER%"

git diff --quiet --exit-code -- >nul 2>nul && git diff --cached --quiet --exit-code -- >nul 2>nul
if errorlevel 1 (
    if not defined FORCE (
        echo [ERROR] working tree not clean. Commit or stash first, or rerun with --force.
        exit /b 1
    )
    echo [WARN] --force: continuing with dirty working tree.
)

git show-ref --tags --verify --quiet "refs/tags/%TAG%" 2>nul
if not errorlevel 1 (
    echo [ERROR] tag %TAG% already exists locally.
    exit /b 1
)
git ls-remote --exit-code --tags origin "refs/tags/%TAG%" >nul 2>nul
if not errorlevel 1 (
    echo [ERROR] tag %TAG% already exists on origin.
    exit /b 1
)

echo [1/2] create tag %TAG% (from Doxyfile PROJECT_NUMBER) ...
git tag -a "%TAG%" -m "MotionGloveUI %TAG%"
if errorlevel 1 exit /b 1

echo [2/2] push tag %TAG% ...
git push origin "%TAG%"
if errorlevel 1 (
    echo [ERROR] push failed, deleting local tag %TAG%
    git tag -d "%TAG%" >nul 2>nul
    exit /b 1
)

echo.
echo [OK] tag %TAG% pushed. 需要发布产物时请自行配置 release workflow。
exit /b 0

#__SH__
# POSIX 段
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

FORCE=0
[ "$1" = "--force" ] && FORCE=1

DOXYFILE="$ROOT/Doxyfile"
[ -f "$DOXYFILE" ] || { echo "[ERROR] not found: $DOXYFILE"; exit 1; }

VER=$(sed -n 's/^PROJECT_NUMBER[^=]*= *"\([^"]*\)".*/\1/p' "$DOXYFILE" | head -n 1)
: "${VER:=1.0.0}"
TAG="v$VER"

if [ "$FORCE" -ne 1 ]; then
    if ! git diff --quiet --exit-code -- || ! git diff --cached --quiet --exit-code --; then
        echo "[ERROR] working tree not clean. Commit or stash first, or rerun with --force."
        exit 1
    fi
else
    echo "[WARN] --force: continuing with dirty working tree."
fi

if git show-ref --tags --verify --quiet "refs/tags/$TAG" 2>/dev/null; then
    echo "[ERROR] tag $TAG already exists locally."
    exit 1
fi
if git ls-remote --tags origin "refs/tags/$TAG" | grep -q .; then
    echo "[ERROR] tag $TAG already exists on origin."
    exit 1
fi

echo "[1/2] create tag $TAG (from Doxyfile PROJECT_NUMBER) ..."
git tag -a "$TAG" -m "MotionGloveUI $TAG" || exit 1

echo "[2/2] push tag $TAG ..."
if ! git push origin "$TAG"; then
    echo "[ERROR] push failed, deleting local tag $TAG"
    git tag -d "$TAG" >/dev/null 2>&1
    exit 1
fi

echo ""
echo "[OK] tag $TAG pushed. Configure a release workflow to publish artifacts."
exit 0

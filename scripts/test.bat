:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前；双解释器脚本须 CRLF、无 BOM。
REM  test.bat -- 构建并运行单元测试（独立构建树 out\build\tdd-tests）
REM  用法：scripts\test.bat [Debug|Release] [ctest -R 正则]
setlocal
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
call "%ROOT%\scripts\lib\build_and_test.bat" %*
exit /b %errorlevel%

#__SH__
# POSIX 段
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

CONFIG=${1:-Debug}
FILTER=${2:-}
BUILD="out/build/tdd-tests"
JOBS=$( (command -v nproc >/dev/null 2>&1 && nproc) || echo 4 )
DEP_ARG="-DXSTYPES_FETCH_VIA_SSH=${XSTYPES_FETCH_VIA_SSH:-OFF}"

echo "[1/3] configure: $BUILD"
if [ -n "${QTDIR:-}" ]; then
    cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE="$CONFIG" \
          -DMOTIONGLOVE_UI_BUILD_TESTS=ON -DCMAKE_PREFIX_PATH="$QTDIR" "$DEP_ARG" \
        || { echo "[ERROR] cmake configure failed"; exit 1; }
else
    cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE="$CONFIG" \
          -DMOTIONGLOVE_UI_BUILD_TESTS=ON "$DEP_ARG" \
        || { echo "[ERROR] cmake configure failed"; exit 1; }
fi

echo "[2/3] build: 全部目标（含依赖侧测试）[$CONFIG]"
# CPM 依赖也注册了 ctest 用例（glovell_unit_tests、Cli.SelfCheck 等），只编
# 本项目两个测试目标会让它们变成 NOT_BUILT / Not Run，ctest 以 8 退出。
cmake --build "$BUILD" -j"$JOBS" \
    || { echo "[ERROR] build failed"; exit 1; }

echo "[3/3] ctest [$CONFIG]"
cd "$BUILD" || exit 1
if [ -n "$FILTER" ]; then
    # ctest -R 没匹配到任何用例时退出码是 0，直接跑会把"一个用例都没跑到"误报成通过
    if ! ctest -N -R "$FILTER" 2>/dev/null | grep -qE 'Total Tests: [1-9]'; then
        echo "[ERROR] 过滤器 \"$FILTER\" 没有匹配到任何用例（构建失败或用例名写错）"
        exit 1
    fi
    ctest --output-on-failure -R "$FILTER"
else
    # ctest 没发现任何用例时同样会返回 0，先断言确实注册了用例
    if ! ctest -N 2>/dev/null | grep -qE 'Total Tests: [1-9]'; then
        echo "[ERROR] ctest 没有发现任何用例（构建失败或测试未注册）"
        exit 1
    fi
    ctest --output-on-failure
fi
rc=$?
if [ "$rc" -ne 0 ]; then
    echo "[FAILED] ctest exit code = $rc"
    exit "$rc"
fi
echo "[OK] all tests passed"

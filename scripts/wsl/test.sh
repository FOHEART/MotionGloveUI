#!/bin/bash
# scripts/wsl/test.sh -- 在 WSL 副本里跑单元/界面测试（scripts/test.bat 的 POSIX 分支）
# 用法：test.sh [Debug|Release] [ctest -R 正则]
set -eu
. "$(dirname -- "$0")/lib.sh"
CONFIG=${1:-Release}
if [ $# -gt 0 ]; then shift; fi
wsl_sync
wsl_run scripts/test.bat "$CONFIG" "$@"

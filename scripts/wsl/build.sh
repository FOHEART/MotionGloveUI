#!/bin/bash
# scripts/wsl/build.sh -- 在 WSL 副本里跑主构建（scripts/build.bat 的 POSIX 分支）
# 用法：build.sh [Debug|Release]
set -eu
. "$(dirname -- "$0")/lib.sh"
wsl_sync
wsl_run scripts/build.bat "${1:-Release}"

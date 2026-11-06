#!/bin/bash
# scripts/wsl/docs.sh -- 在 WSL 副本里跑文档门禁（scripts/gen_docs.bat 的 POSIX 分支）
# 用 WSL 自带的 doxygen（通常等于 Ubuntu 的 apt 版本）复验"写法是否跨版本"
set -eu
. "$(dirname -- "$0")/lib.sh"
echo "[wsl] doxygen：$(doxygen --version 2>/dev/null || echo '未安装（apt install doxygen）')"
wsl_sync
wsl_run scripts/gen_docs.bat

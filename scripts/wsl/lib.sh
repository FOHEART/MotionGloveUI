#!/bin/bash
# ============================================================================
# scripts/wsl/lib.sh -- WSL 侧公共函数（由 sync/build/test/docs 引入）
#
# 设计要点：
#   * 一律在「家目录副本」里构建，不直接跑 /mnt/d/...（9p 慢，且会与 Windows 的 out/ 抢构建树）
#   * 日志写 $HOME，不写 /tmp：systemd 发行版的 /tmp 是 tmpfs，WSL 实例回收即清空
#   * 同步用 rsync --delete，排除构建产物与文档输出，保证副本与工作区一致
# ============================================================================

# 误用 sh 启动时切回 bash（下面用到 PIPESTATUS）
if [ -z "${BASH_VERSION:-}" ]; then exec bash "$0" "$@"; fi

set -u

# 仓库根：本文件位于 scripts/wsl/ 下，$0 是调用它的脚本
wsl_repo_root() {
    ( CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd )
}

# 同步源：由 wsl.bat 用 MOTIONGLOVE_UI_WSL_SOURCE 显式传入 /mnt 路径。
# 不能只靠 $0 推：这组脚本会被 rsync 进家目录副本，在副本里直接跑时
# $0 推出的"仓库根"就是副本自己，同步会退化成自我复制，工作区的新改动
# 永远过不来（症状：同步文件数两次完全相同、新用例在 ctest 里不存在）。
wsl_source_root() {
    if [ -n "${MOTIONGLOVE_UI_WSL_SOURCE:-}" ]; then
        ( CDPATH= cd -- "$MOTIONGLOVE_UI_WSL_SOURCE" && pwd )
    else
        wsl_repo_root
    fi
}

WSL_DEST="${WSL_DEST:-$HOME/MotionGloveUI}"
WSL_LOG="${WSL_LOG:-$HOME/wsl_last.log}"

# 可选：WSL 侧自定义 Qt 前缀（默认交给脚本自己 find_package 系统 Qt5）
if [ -n "${WSL_QTDIR:-}" ]; then export QTDIR="$WSL_QTDIR"; fi

wsl_sync() {
    src=$(wsl_source_root) || { echo "[ERROR] 无法定位同步源"; return 1; }
    # 自同步判断必须跟目标副本比，而不是跟 $0 推出的仓库根比：
    # 从 /mnt 调用时仓库根就是同步源本身，比错了会把"该同步"误判成"在副本里"。
    if [ "$src" = "$WSL_DEST" ]; then
        echo "[wsl] 源与副本相同（在副本内直接运行），跳过同步"
        return 0
    fi
    mkdir -p "$WSL_DEST" || return 1
    rsync -a --delete \
        --exclude 'out' --exclude 'lib' --exclude '.git' \
        --exclude 'docs/html' --exclude 'docs/doxygen' \
        "$src/" "$WSL_DEST/" || { echo "[ERROR] rsync 失败：$src -> $WSL_DEST"; return 1; }
    # --delete 不会删「被排除」的路径，所以副本里的 out/ 会保留下来做增量构建；统计时排除它
    echo "[wsl] 同步完成：$(find "$WSL_DEST" -type f -not -path '*/out/*' | wc -l) 个文件 -> $WSL_DEST"
}

wsl_run() {
    rel=$1; shift
    echo "[wsl] 副本内执行：sh $rel $*"
    ( cd "$WSL_DEST" && sh "$rel" "$@" ) 2>&1 | tee "$WSL_LOG"
    rc=${PIPESTATUS[0]}
    echo "[wsl] 退出码 $rc（完整日志：$WSL_LOG）"
    if [ "$rc" -ne 0 ]; then
        echo "[wsl] 失败尾部输出："
        tail -n 20 "$WSL_LOG"
    fi
    return "$rc"
}

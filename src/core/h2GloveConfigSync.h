/**
 * @file  h2GloveConfigSync.h
 * @brief 手套上线 / 下线时该对套装配置做什么（读取回填 / 写盘保存）。
 * @ingroup h2core
 * @version 1.0.0
 *
 * @details
 * 为什么要有这个模块：配置同步原先没有任何"上线读、下线存"的时机，
 * 首次使用（配置目录里没有该序列号的文件）时连第一份默认配置都不会生成，
 * 于是所有保存路径都因为查不到配置而静默失效。
 *
 * 本模块只回答"这次状态跳变该执行哪个动作"，不碰 SDK、不碰控件，
 * 因此可以毫秒级单测；真正的 SDK 调用留在 h2Widget 里。
 *
 * 纯函数，无全局状态，可在多线程中并发调用；不依赖 Qt 与 SDK。
 */
#pragma once

namespace h2core
{

/**
 * @brief 手套在线状态跳变时应执行的配置动作。
 */
enum class GloveConfigSyncAction
{
    /// 状态没变（一直在线或一直离线），什么都不用做。
    None,
    /// 由离线转为在线：读配置回填界面；没有配置时创建一份默认配置。
    Load,
    /// 由在线转为离线：把界面上的当前值写回配置文件。
    Save
};

/**
 * @brief 由「上一次看到的在线状态」与「这一次看到的在线状态」推导配置动作。
 * @param[in] wasOnline 上一次采样时任一只手是否在线。
 * @param[in] isOnline  本次采样时任一只手是否在线。
 * @return 离线→在线返回 Load，在线→离线返回 Save，其余返回 None。
 * @note 只看跳变，不看具体是哪只手：左手或右手任一上线即视为在线。
 */
GloveConfigSyncAction decideGloveConfigSyncAction(bool wasOnline, bool isOnline);

} // namespace h2core

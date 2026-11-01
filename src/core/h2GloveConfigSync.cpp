/**
 * @file  h2GloveConfigSync.cpp
 * @brief 手套上线 / 下线时配置同步动作推导的实现。
 * @ingroup h2core
 * @version 1.0.0
 *
 * @details 只做状态跳变判定，函数级说明见头文件。
 */

#include "core/h2GloveConfigSync.h"

namespace h2core
{

GloveConfigSyncAction decideGloveConfigSyncAction(bool wasOnline, bool isOnline)
{
    if (wasOnline == isOnline)
    {
        return GloveConfigSyncAction::None;
    }
    return isOnline ? GloveConfigSyncAction::Load : GloveConfigSyncAction::Save;
}

} // namespace h2core

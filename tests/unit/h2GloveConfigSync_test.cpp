/**
 * @file  h2GloveConfigSync_test.cpp
 * @brief 手套上线 / 下线时配置同步动作推导的单元测试，框架为 GoogleTest。
 * @ingroup h2tests
 *
 * @details
 * 覆盖策略
 *  - 全部四种状态组合各一条用例，防止"只处理上线、忘了下线"这类单向实现；
 *  - 断言只比枚举值，不碰 SDK 与控件，避免用例依赖真实手套。
 *
 * 背景：没有这套判定之前，配置只在用户拖滑块松手时才写盘，而且首次运行
 * 配置目录里没有该序列号的文件，保存路径因为查不到配置而整体失效。
 */

#include <gtest/gtest.h>

#include "core/h2GloveConfigSync.h"

/**
 * @test   GloveConfigSync.OfflineToOnlineLoadsConfig
 * @brief  手套上线：应触发一次配置读取（缺失时由上层创建默认配置）。
 * @details 目的：上电是最需要"把配置显示到界面"的时刻；一旦这里返回 None，
 *          用户会看到界面停在默认值，却误以为设备读到了旧配置。
 * @expected 返回 GloveConfigSyncAction::Load
 * @priority P0
 */
/// @cond
TEST(GloveConfigSync, OfflineToOnlineLoadsConfig)
{
    EXPECT_EQ(h2core::decideGloveConfigSyncAction(false, true),
              h2core::GloveConfigSyncAction::Load);
}
/// @endcond

/**
 * @test   GloveConfigSync.OnlineToOfflineSavesConfig
 * @brief  手套下线：应触发一次配置写盘。
 * @details 目的：关机前不在线了，用户最后调的角度只能靠这次写盘保留；
 *          若返回 None，下次上电会读回旧值，表现为"改了不生效"。
 * @expected 返回 GloveConfigSyncAction::Save
 * @priority P0
 */
/// @cond
TEST(GloveConfigSync, OnlineToOfflineSavesConfig)
{
    EXPECT_EQ(h2core::decideGloveConfigSyncAction(true, false),
              h2core::GloveConfigSyncAction::Save);
}
/// @endcond

/**
 * @test   GloveConfigSync.StayingOnlineDoesNothing
 * @brief  一直在线：不重复读也不重复写。
 * @details 目的：监控定时器每秒都会问一次，若在稳态也返回动作，
 *          会变成每秒读一次盘 / 写一次盘，既卡界面又磨损文件。
 * @expected 返回 GloveConfigSyncAction::None
 * @priority P1
 */
/// @cond
TEST(GloveConfigSync, StayingOnlineDoesNothing)
{
    EXPECT_EQ(h2core::decideGloveConfigSyncAction(true, true),
              h2core::GloveConfigSyncAction::None);
}
/// @endcond

/**
 * @test   GloveConfigSync.StayingOfflineDoesNothing
 * @brief  一直离线：什么都不做。
 * @details 目的：软件刚打开、手套还没连上时不应创建配置文件——
 *          序列号可能还没解析出来，此时写盘会写出 SN=0 的垃圾文件。
 * @expected 返回 GloveConfigSyncAction::None
 * @priority P0
 */
/// @cond
TEST(GloveConfigSync, StayingOfflineDoesNothing)
{
    EXPECT_EQ(h2core::decideGloveConfigSyncAction(false, false),
              h2core::GloveConfigSyncAction::None);
}
/// @endcond

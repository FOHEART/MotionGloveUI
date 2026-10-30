/**
 * @file  h2RatioConversion_test.cpp
 * @brief OK 校准滑杆刻度与配置存储值之间换算的单元测试，框架为 GoogleTest。
 * @ingroup h2tests
 *
 * @details
 * 覆盖策略
 *  - 核心行为：滑杆刻度与存储偏移之间的双向换算；
 *  - 不变量：任意刻度值往返一次必须回到原值（防止正负方向错位）；
 *  - 边界条件：中性点、中性点下方一格。
 *
 * 该层是纯整数运算，不依赖 Qt 与 SDK，用于替代原先散落在
 * src/h2Widget_OKCali.cpp 里重复出现的 `val - 100` 与 `round(ratio + 100.0)`。
 */

#include <gtest/gtest.h>

#include "core/h2RatioConversion.h"

/**
 * @test   RatioConversion.SliderNeutralMapsToZeroOffset
 * @brief  换算：滑杆中性刻度 100 对应配置中的偏移 0。
 * @details 目的：中性点一旦错位，用户每次恢复默认都会把手部比例
 *          静默改成非零值。设计思路：以「等价于无修正」的中性点作为金标准，
 *          期望值由业务定义直接给出，不抄实现公式。
 * @expected sliderValueToRatioOffset(100) 为 0；ratioOffsetToSliderValue(0) 为 100
 * @priority P0
 */
/// @cond
TEST(RatioConversion, SliderNeutralMapsToZeroOffset)
{
    EXPECT_EQ(h2core::sliderValueToRatioOffset(100), 0);
    EXPECT_EQ(h2core::ratioOffsetToSliderValue(0), 100);
}
/// @endcond

/**
 * @test   RatioConversion.SliderRoundTripsThroughOffset
 * @brief  不变量：任意滑杆刻度经「写入配置」再「读回界面」后保持不变。
 * @details 目的：写入路径用整数减法、读回路径原先用 std::round(double)，
 *          两条路径若偏移量或取整方式不一致，就会出现「拖到 137、
 *          界面回弹成 136」这种漂移。设计思路：对 0 到 200 全量刻度做往返断言，
 *          不抽样，彻底排除个别点上的取整差异。
 * @expected 201 个刻度值逐一原样返回
 * @priority P0
 */
/// @cond
TEST(RatioConversion, SliderRoundTripsThroughOffset)
{
    for (int slider = 0; slider <= 200; ++slider)
    {
        const int offset = h2core::sliderValueToRatioOffset(slider);
        ASSERT_EQ(h2core::ratioOffsetToSliderValue(offset), slider)
            << "刻度 " << slider << " 往返后发生漂移";
    }
}
/// @endcond

/**
 * @test   RatioConversion.OffsetRoundTripsThroughSlider
 * @brief  不变量：配置里的偏移值经界面往返后保持不变（反方向）。
 * @details 目的：覆盖「从配置文件读到偏移值再回写」的路径，与上一条
 *          形成双向对照，两条都成立才能保证读写一致。
 *          设计思路：偏移量合法范围为 -100 到 100，全量遍历。
 * @expected 201 个偏移值逐一原样返回
 * @priority P1
 */
/// @cond
TEST(RatioConversion, OffsetRoundTripsThroughSlider)
{
    for (int offset = -100; offset <= 100; ++offset)
    {
        const int slider = h2core::ratioOffsetToSliderValue(offset);
        ASSERT_EQ(h2core::sliderValueToRatioOffset(slider), offset)
            << "偏移 " << offset << " 往返后发生漂移";
    }
}
/// @endcond

/**
 * @test   RatioConversion.SliderBelowNeutralYieldsNegativeOffset
 * @brief  边界：低于中性点的刻度换算为负偏移。
 * @details 目的：锁定符号约定——滑杆往小拖是「减小比例」，写入配置必须为负值。
 *          符号一旦反过来，用户的调整效果会与拖动方向相反。
 *          设计思路：取中性点下方一个最小刻度，验证符号而非绝对值，避免抄实现。
 * @expected sliderValueToRatioOffset(99) 为 -1，且结果为负数
 * @priority P1
 */
/// @cond
TEST(RatioConversion, SliderBelowNeutralYieldsNegativeOffset)
{
    const int offset = h2core::sliderValueToRatioOffset(99);

    EXPECT_EQ(offset, -1);
    EXPECT_LT(offset, 0);
}
/// @endcond

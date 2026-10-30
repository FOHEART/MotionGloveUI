/**
 * @file  h2TextWrap_test.cpp
 * @brief 定宽文本折行的单元测试，框架为 GoogleTest。
 * @ingroup h2tests
 *
 * @details
 * 覆盖策略
 *  - 核心行为：按固定字符数插入换行，段与段之间用 LF 连接；
 *  - 边界条件：长度恰为宽度整数倍、有余数、空串；
 *  - 异常场景：宽度为 0 或负值（调用方传入非法参数）。
 *
 * 被测函数原为 ui/h2CsvRecord.cpp 匿名命名空间里的 formatFileNameForDisplay，
 * 无法单独测试；提取到核心层后可在无 QWidget 环境下直接验证。
 */

#include <gtest/gtest.h>

#include <QString>

#include "core/h2TextWrap.h"

/**
 * @test   TextWrap.WrapsAtFixedWidth
 * @brief  折行：长度恰为宽度整数倍时，每行宽度一致且不多出空行。
 * @details 目的：整数倍是差一错误高发点——多循环一次的实现在末尾
 *          会追加一个只有换行符的空行，标签高度随之翻倍。
 *          设计思路：8 个字符按宽度 4 折行，断言行数与逐行内容。
 * @expected 结果为 "ABCD\nEFGH"，共 2 行
 * @priority P0
 */
/// @cond
TEST(TextWrap, WrapsAtFixedWidth)
{
    const QString wrapped = h2core::wrapFixedWidth(QStringLiteral("ABCDEFGH"), 4);

    EXPECT_EQ(wrapped, QStringLiteral("ABCD\nEFGH"));
    EXPECT_EQ(wrapped.count(QLatin1Char('\n')), 1);
}
/// @endcond

/**
 * @test   TextWrap.KeepsRemainderOnLastLine
 * @brief  折行：不足一整行的尾部原样保留在最后一行。
 * @details 目的：防止实现丢弃余数（丢字符）或用空格补齐（多出可见空白）。
 *          设计思路：10 个字符按宽度 4 折行，余数为 2。
 * @expected 结果为 "ABCD\nEFGH\nIJ"
 * @priority P0
 */
/// @cond
TEST(TextWrap, KeepsRemainderOnLastLine)
{
    const QString wrapped = h2core::wrapFixedWidth(QStringLiteral("ABCDEFGHIJ"), 4);

    EXPECT_EQ(wrapped, QStringLiteral("ABCD\nEFGH\nIJ"));
}
/// @endcond

/**
 * @test   TextWrap.EmptyTextStaysEmpty
 * @brief  边界：空输入返回空串，不产生任何换行符。
 * @details 目的：目录未选择时文件名为空，若返回单个换行符会让标签
 *          看起来「有内容」，干扰用户判断。设计思路：空集合边界值。
 * @expected 结果为空串
 * @priority P1
 */
/// @cond
TEST(TextWrap, EmptyTextStaysEmpty)
{
    const QString wrapped = h2core::wrapFixedWidth(QString(), 4);

    EXPECT_TRUE(wrapped.isEmpty());
}
/// @endcond

/**
 * @test   TextWrap.NonPositiveWidthReturnsTextUnchanged
 * @brief  异常：宽度为 0 或负值时原样返回，不崩溃、不无限循环。
 * @details 目的：宽度来自界面配置，可能被误设为 0；若实现用
 *          `for (i = 0; i < n; i += width)` 且不做保护，步长为 0 会死循环，
 *          界面直接卡死。设计思路：0 与负数两个非法等价类。
 * @expected 两种情况均原样返回输入文本
 * @priority P0
 */
/// @cond
TEST(TextWrap, NonPositiveWidthReturnsTextUnchanged)
{
    const QString text = QStringLiteral("ABCDEFGH");

    EXPECT_EQ(h2core::wrapFixedWidth(text, 0), text);
    EXPECT_EQ(h2core::wrapFixedWidth(text, -3), text);
}
/// @endcond

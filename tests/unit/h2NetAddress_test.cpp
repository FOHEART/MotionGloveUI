/**
 * @file  h2NetAddress_test.cpp
 * @brief 广播地址串拼接与定长缓冲区写入的单元测试，框架为 GoogleTest。
 * @ingroup h2tests
 *
 * @details
 * 覆盖策略
 *  - 核心行为：空项过滤、分隔符拼接、UTF-8 字节级定长写入；
 *  - 边界条件：缓冲区恰好容纳、少 1 字节、容量 1、容量 0；
 *  - 异常场景：空指针目标缓冲区、无分隔符的超长串。
 *
 * 该层只依赖 Qt5::Core（QString / QStringList / QByteArray），
 * 不含 QWidget 调用，可在无窗口、无 GPU 环境毫秒级执行。
 */

#include <gtest/gtest.h>

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QtGlobal>

#include <cstring>

#include "core/h2NetAddress.h"

namespace
{

/// @brief 测试用地址串：两项 IPv4 加端口，总长 35 字节。
const char* const kTwoEntrySpec = "192.168.1.10:5000,192.168.1.11:5000";

/// @brief 单项地址串，总长 17 字节。
const char* const kFirstEntry = "192.168.1.10:5000";

} // namespace

/**
 * @test   NetAddress.JoinDropsEmptyAndBlankEntries
 * @brief  拼接：空串与纯空白项被丢弃，只保留有效地址。
 * @details 目的：UI 表格里允许存在空行，若把空行拼进地址串会产生
 *          "1.2.3.4:5000,,1.2.3.5:5000" 这类非法配置，SDK 侧解析失败。
 *          设计思路：用等价类划分覆盖「空串 / 空格 / Tab / 有效项」四类输入。
 * @expected 结果为 "1.2.3.4:5000,1.2.3.5:5000"
 * @priority P0
 */
/// @cond
TEST(NetAddress, JoinDropsEmptyAndBlankEntries)
{
    QStringList items;                                        // Arrange
    items << QStringLiteral("1.2.3.4:5000")
          << QString()
          << QStringLiteral("   ")
          << QStringLiteral("\t")
          << QStringLiteral("1.2.3.5:5000");

    const QString joined = h2core::joinNonEmpty(items);        // Act

    EXPECT_EQ(joined, QStringLiteral("1.2.3.4:5000,1.2.3.5:5000")); // Assert
}
/// @endcond

/**
 * @test   NetAddress.JoinSingleEntryAddsNoSeparator
 * @brief  拼接：只有一项时不追加任何分隔符。
 * @details 目的：防止实现在末尾无条件补逗号，产生 "1.2.3.4:5000," 的
 *          尾随逗号，SDK 侧会把它当成一个空地址项。
 *          设计思路：这是「元素个数为 1」的边界值。
 * @expected 结果等于输入项本身，长度为 12
 * @priority P1
 */
/// @cond
TEST(NetAddress, JoinSingleEntryAddsNoSeparator)
{
    QStringList items;
    items << QStringLiteral("1.2.3.4:5000");

    const QString joined = h2core::joinNonEmpty(items);

    EXPECT_EQ(joined, QStringLiteral("1.2.3.4:5000"));
    EXPECT_EQ(joined.size(), 12);
}
/// @endcond

/**
 * @test   NetAddress.JoinEmptyListYieldsEmptyString
 * @brief  拼接：全部为空时返回空串，而不是落单的分隔符。
 * @details 目的：地址表全空时应写入空广播配置（等价于关闭广播），
 *          不能写出 "," 这种无意义内容。设计思路：空集合边界值。
 * @expected 返回空串且 isEmpty() 为真
 * @priority P1
 */
/// @cond
TEST(NetAddress, JoinEmptyListYieldsEmptyString)
{
    const QString joined = h2core::joinNonEmpty(QStringList());

    EXPECT_TRUE(joined.isEmpty());
    EXPECT_EQ(joined.size(), 0);
}
/// @endcond

/**
 * @test   NetAddress.WriteKeepsSpecWhenItFitsExactly
 * @brief  定长写入：UTF-8 字节数与「容量减 1」相等时完整写入并补 NUL。
 * @details 目的：验证容量计算没有差一错误（多留或少留 1 字节）。
 *          设计思路：取 35 字节的地址串，缓冲区给 36 字节，正好容纳。
 * @expected 返回 35；缓冲区内容与输入一致；末字节为 NUL
 * @priority P0
 */
/// @cond
TEST(NetAddress, WriteKeepsSpecWhenItFitsExactly)
{
    char buffer[36];
    std::memset(buffer, 0x7F, sizeof(buffer));                // Arrange：填脏数据

    const int written = h2core::writeAddressSpecFixed(        // Act
        buffer, static_cast<int>(sizeof(buffer)), QString::fromLatin1(kTwoEntrySpec));

    EXPECT_EQ(written, 35);                                   // Assert
    EXPECT_STREQ(buffer, kTwoEntrySpec);
    EXPECT_EQ(buffer[written], '\0');
}
/// @endcond

/**
 * @test   NetAddress.WriteDropsWholeEntryInsteadOfHalfAddress
 * @brief  定长写入：缓冲区差 1 字节时丢弃最后一项，绝不写出半截地址。
 * @details 目的：这是本条用例要防的真实缺陷——按字节硬裁剪会把
 *          "192.168.1.10:5000,192.168.1.11:5000" 截成
 *          "192.168.1.10:5000,192.168.1.11:500"（端口少一位），
 *          SDK 侧拿到的就是一个指向错误端口的合法地址，静默发错数据。
 *          设计思路：用「刚好少 1 字节」的边界值，再校验结果里不存在
 *          第二个冒号，证明第二项是整体丢弃而不是被切一半。
 * @expected 返回 17（只剩第一项）；缓冲区等于第一项；无尾随逗号
 * @priority P0
 */
/// @cond
TEST(NetAddress, WriteDropsWholeEntryInsteadOfHalfAddress)
{
    char buffer[35];
    std::memset(buffer, 0x7F, sizeof(buffer));

    const int written = h2core::writeAddressSpecFixed(
        buffer, static_cast<int>(sizeof(buffer)), QString::fromLatin1(kTwoEntrySpec));

    EXPECT_EQ(written, 17);
    EXPECT_STREQ(buffer, kFirstEntry);
    EXPECT_EQ(QString::fromLatin1(buffer).count(QLatin1Char(':')), 1)
        << "截断后仍出现第二个冒号，说明写出的是半截地址";
    EXPECT_FALSE(QString::fromLatin1(buffer).endsWith(QLatin1Char(',')))
        << "截断后留下了尾随逗号";
}
/// @endcond

/**
 * @test   NetAddress.WriteWithoutSeparatorYieldsEmptyString
 * @brief  定长写入：单项地址放不下且没有分隔符可回退时，写入空串。
 * @details 目的：宁可写出空配置（关闭广播）也不写出错误地址。
 *          设计思路：单元素集合加容量 6（仅能放 5 字节），无可回退的逗号。
 * @expected 返回 0；缓冲区首字节为 NUL
 * @priority P1
 */
/// @cond
TEST(NetAddress, WriteWithoutSeparatorYieldsEmptyString)
{
    char buffer[6];
    std::memset(buffer, 0x7F, sizeof(buffer));

    const int written = h2core::writeAddressSpecFixed(
        buffer, static_cast<int>(sizeof(buffer)), QStringLiteral("1.2.3.4:5000"));

    EXPECT_EQ(written, 0);
    EXPECT_EQ(buffer[0], '\0');
}
/// @endcond

/**
 * @test   NetAddress.WriteToCapacityOneYieldsEmptyString
 * @brief  定长写入：容量为 1 时只能写 NUL，返回 0。
 * @details 目的：容量 1 是可写 NUL 的最小合法缓冲区，off-by-one 错误的
 *          高发点——若实现写成 memcpy(capacity) 就会越界 1 字节。
 *          设计思路：边界值下界。
 * @expected 返回 0；buffer[0] 为 NUL
 * @priority P1
 */
/// @cond
TEST(NetAddress, WriteToCapacityOneYieldsEmptyString)
{
    char buffer[1];
    buffer[0] = 0x7F;

    const int written = h2core::writeAddressSpecFixed(
        buffer, 1, QString::fromLatin1(kTwoEntrySpec));

    EXPECT_EQ(written, 0);
    EXPECT_EQ(buffer[0], '\0');
}
/// @endcond

/**
 * @test   NetAddress.WriteToZeroCapacityWritesNothing
 * @brief  定长写入：容量为 0 时不触碰缓冲区，返回 0。
 * @details 目的：防止零容量下的负下标或越界写。设计思路：非法入参，
 *          按「拒绝并保持原值」策略处理。
 * @expected 返回 0；缓冲区字节保持不变
 * @priority P2
 */
/// @cond
TEST(NetAddress, WriteToZeroCapacityWritesNothing)
{
    char buffer[4];
    std::memset(buffer, 0x7F, sizeof(buffer));

    const int written = h2core::writeAddressSpecFixed(
        buffer, 0, QString::fromLatin1(kTwoEntrySpec));

    EXPECT_EQ(written, 0);
    EXPECT_EQ(buffer[0], 0x7F) << "零容量时不应写入任何字节";
}
/// @endcond

/**
 * @test   NetAddress.WriteToNullBufferIsHarmless
 * @brief  定长写入：目标缓冲区为空指针时安全返回，不崩溃。
 * @details 目的：SDK 结构体成员地址理论上不会为空，但接口层仍需自保，
 *          避免上游误传导致进程崩溃。设计思路：空指针异常输入。
 * @expected 返回 0，无崩溃
 * @priority P2
 */
/// @cond
TEST(NetAddress, WriteToNullBufferIsHarmless)
{
    EXPECT_EQ(h2core::writeAddressSpecFixed(nullptr, 64, QString::fromLatin1(kTwoEntrySpec)), 0);
}
/// @endcond

/**
 * @test   NetAddress.WriteZeroFillsRemainderOfBuffer
 * @brief  定长写入：写入后缓冲区剩余字节全部清零，不留脏数据。
 * @details 目的：写入目标往往是复用过的 SDK 结构体成员，若不清零，
 *          上一次更长的地址会以残留字节形式被当作配置读回。
 *          设计思路：先用 0x7F 填满缓冲区，写入短串后检查 NUL 之后全为 0。
 * @expected NUL 之后所有字节均为 0
 * @priority P1
 */
/// @cond
TEST(NetAddress, WriteZeroFillsRemainderOfBuffer)
{
    char buffer[64];
    std::memset(buffer, 0x7F, sizeof(buffer));

    const int written = h2core::writeAddressSpecFixed(
        buffer, static_cast<int>(sizeof(buffer)), QStringLiteral("1.2.3.4:5000"));

    ASSERT_EQ(written, 12);
    for (int i = written + 1; i < static_cast<int>(sizeof(buffer)); ++i)
    {
        ASSERT_EQ(buffer[i], 0) << "缓冲区第 " << i << " 字节未被清零";
    }
}
/// @endcond

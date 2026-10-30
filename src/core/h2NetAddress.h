/**
 * @file  h2NetAddress.h
 * @brief 广播地址串的拼接与定长缓冲区写入。
 * @ingroup h2core
 * @version 1.0.0
 *
 * @details
 * h2GloveStreamConfig::BroadcastAddr 是 SDK 结构体里的定长字符数组，
 * 内容形如 "192.168.1.10:5000,192.168.1.11:5000"。界面侧需要把表格里
 * 的多行地址拼成一个串再写入该数组，长度超出时必须截断。
 *
 * 截断策略：**丢弃整个地址项**，绝不写出半截地址。按字节硬裁会把
 * "192.168.1.11:5000" 切成 "192.168.1.11:500"，那仍然是一个语法合法
 * 的地址，SDK 会静默发往错误端口 —— 这比拒绝发送更难排查。
 *
 * 只依赖 Qt5::Core，无 QWidget 调用，可独立单元测试。
 */
#pragma once

#include <QChar>
#include <QString>
#include <QStringList>

namespace h2core
{

/**
 * @brief 去掉首尾空白后，用分隔符连接所有非空项。
 * @param[in] items     待连接的字符串列表，可为空；
 *                      其中空串与纯空白项会被丢弃（不会产生连续分隔符）。
 * @param[in] separator 项之间的分隔符，默认英文逗号 ','。
 * @return 连接结果；没有有效项时返回空串（而不是单个分隔符）。
 * @post 返回值中不出现连续分隔符，也不以分隔符开头或结尾。
 */
QString joinNonEmpty(const QStringList& items, QChar separator = QLatin1Char(','));

/**
 * @brief 把地址串写入 NUL 结尾的定长缓冲区，放不下时丢弃末尾整项。
 * @param[out] destination 目标缓冲区首地址，**可为空指针**（为空时直接返回 0）。
 *                         写入前会把前 @p capacity 字节整体清零，因此 NUL
 *                         之后的残留字节不会污染下一次读取。
 * @param[in]  capacity    缓冲区总字节数（含结尾 NUL），取值 [0, INT_MAX]；
 *                         取值 0 表示没有可写空间，此时不修改缓冲区。
 * @param[in]  spec        待写入的地址串，按 UTF-8 编码计长；
 *                         空串写入后缓冲区内容为空串。
 * @return 实际写入的字节数，**不含**结尾 NUL；
 *         完整写入时为 spec 的 UTF-8 字节数；
 *         发生截断时为截断点之前最后一个完整地址项的字节数；
 *         无法容纳任何完整项、或入参非法时为 0。
 * @warning 截断以 UTF-8 字节为单位判定，返回的字节数不会切断多字节字符，
 *          因为截断点必定落在分隔符上。
 */
int writeAddressSpecFixed(char* destination, int capacity, const QString& spec);

} // namespace h2core

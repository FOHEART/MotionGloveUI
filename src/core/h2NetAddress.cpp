/**
 * @file  h2NetAddress.cpp
 * @brief 实现广播地址串的拼接与定长缓冲区写入。
 * @ingroup h2core
 *
 * @details
 * 本文件实现 h2NetAddress.h 声明的两个函数，函数语义见头文件。
 *
 * 写入路径先整体清零目标缓冲区，再按「完整地址项」为单位截断：
 * 截断点必定落在分隔符上，因此不会切断多字节字符，也不会留下
 * "192.168.1.11:500" 这种语法合法但端口错误的半截地址。
 */

#include "h2NetAddress.h"

#include <QByteArray>
#include <QtGlobal>

#include <cstring>

namespace h2core
{

QString joinNonEmpty(const QStringList& items, QChar separator)
{
    QStringList kept;
    kept.reserve(items.size());

    for (const QString& item : items)
    {
        const QString trimmed = item.trimmed();
        if (!trimmed.isEmpty())
        {
            kept << trimmed;
        }
    }

    return kept.join(separator);
}

int writeAddressSpecFixed(char* destination, int capacity, const QString& spec)
{
    if (destination == nullptr || capacity <= 0)
    {
        return 0;
    }

    std::memset(destination, 0, static_cast<size_t>(capacity));

    const QByteArray utf8 = spec.toUtf8();
    if (utf8.isEmpty())
    {
        return 0;
    }

    // 末尾必须留 1 字节给 NUL
    const int limit = capacity - 1;
    if (limit <= 0)
    {
        return 0;
    }

    if (utf8.size() <= limit)
    {
        std::memcpy(destination, utf8.constData(), static_cast<size_t>(utf8.size()));
        return utf8.size();
    }

    // 放不下全部内容：回退到最后一个完整地址项的边界。
    // 宁可少发一个地址（用户能立刻发现），也不写出半截地址（静默发错端口）。
    const int cut = utf8.lastIndexOf(',', limit - 1);
    if (cut <= 0)
    {
        return 0;
    }

    std::memcpy(destination, utf8.constData(), static_cast<size_t>(cut));
    return cut;
}

} // namespace h2core

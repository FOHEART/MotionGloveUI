/**
 * @file  h2TextWrap.cpp
 * @brief 实现定宽文本折行。
 * @ingroup h2core
 *
 * @details
 * 本文件实现 h2TextWrap.h 声明的 wrapFixedWidth()，
 * 函数语义与取值范围见头文件，此处只记录实现约定。
 */

#include "h2TextWrap.h"

#include <QStringList>

namespace h2core
{

QString wrapFixedWidth(const QString& text, int maxCharsPerLine)
{
    if (text.isEmpty() || maxCharsPerLine <= 0)
    {
        return text;
    }

    QStringList lines;
    for (int i = 0; i < text.size(); i += maxCharsPerLine)
    {
        lines << text.mid(i, maxCharsPerLine);
    }
    return lines.join(QLatin1Char('\n'));
}

} // namespace h2core

/**
 * @file  h2TextWrap.h
 * @brief 定宽文本折行，用于在固定高度的标签里显示长文件名。
 * @ingroup h2core
 * @version 1.0.0
 *
 * @details
 * CSV 录制目录往往很长，标签高度固定为 5 行，因此需要按固定字符数折行。
 * 原实现是 ui/h2CsvRecord.cpp 匿名命名空间里的 formatFileNameForDisplay，
 * 因是内部链接而无法单测；现已提取到本模块，行为保持不变并补上用例锁定。
 *
 * 只依赖 Qt5::Core（QString），无 QWidget 调用。
 */
#pragma once

#include <QString>

namespace h2core
{

/**
 * @brief 把文本按固定字符数折行，行间以单个 LF('\n') 连接。
 * @param[in] text            待折行的文本，可为空；
 *                            按 UTF-16 码元计数，不做按词断行，不会丢字符。
 * @param[in] maxCharsPerLine 每行最大字符数，取值 [1, INT_MAX]；
 *                            传入 0 或负值视为「不折行」，原样返回 @p text
 *                            （不做除零/死循环保护以外的任何处理）。
 * @return 折行后的文本；输入为空时返回空串；宽度非法时返回 @p text 本身。
 * @post 返回值的长度 = 原长度 + 插入的换行符个数，且不会以换行符结尾。
 */
QString wrapFixedWidth(const QString& text, int maxCharsPerLine);

} // namespace h2core

/**
 * @file  h2RatioConversion.cpp
 * @brief 实现 OK 校准滑杆刻度与配置存储偏移之间的换算。
 * @ingroup h2core
 *
 * @details
 * 本文件实现 h2RatioConversion.h 声明的两个换算函数，
 * 函数语义与取值范围见头文件，此处只记录实现约定。
 */

#include "h2RatioConversion.h"

namespace h2core
{

int sliderValueToRatioOffset(int sliderValue)
{
    return sliderValue - kOkCaliSliderNeutral;
}

int ratioOffsetToSliderValue(int ratioOffset)
{
    return ratioOffset + kOkCaliSliderNeutral;
}

} // namespace h2core

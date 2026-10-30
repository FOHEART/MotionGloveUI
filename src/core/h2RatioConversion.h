/**
 * @file  h2RatioConversion.h
 * @brief OK 校准滑杆刻度与配置存储偏移之间的换算。
 * @ingroup h2core
 * @version 1.0.0
 *
 * @details
 * 界面滑杆的取值是 0 到 200，其中 100 为中性（等价于「不做修正」）；
 * 而 H2Calibration_Config 里保存的是相对中性点的偏移量，范围 -100 到 100。
 * 两套表示原先在 src/h2Widget_OKCali.cpp 中以 `val - 100` 与
 * `std::round(ratio + 100.0)` 的形式重复出现 6 次，换算方向一多就容易写反。
 * 本模块把它们收敛为唯一的换算入口。
 *
 * 纯整数运算，无全局状态，可在多线程中并发调用；不依赖 Qt 与 SDK。
 */
#pragma once

namespace h2core
{

/**
 * @brief 滑杆中性刻度值，等价于「不做任何比例修正」。
 * @details 用无名枚举而不是 `static constexpr int`：C++14 下类外定义的
 *          constexpr 变量一旦被 ODR-use（例如传入按引用取值的断言宏），
 *          会缺少符号定义而报 LNK2019。
 */
enum : int
{
    kOkCaliSliderNeutral = 100 ///< 滑杆刻度中性点，同时也是存储偏移量的零点。
};

/**
 * @brief 把滑杆刻度换算成配置结构体中的偏移量。
 * @param[in] sliderValue 滑杆刻度，通常取自 QSlider::value()，设计范围 [0, 200]；
 *                        超出该范围不做夹取，按线性关系直接换算。
 * @return 相对中性点的偏移量，等于 `sliderValue - 100`；
 *         传入 100 时返回 0，传入 0 时返回 -100。
 * @note 与 ratioOffsetToSliderValue() 互为逆运算，任意合法刻度往返一次保持不变。
 */
int sliderValueToRatioOffset(int sliderValue);

/**
 * @brief 把配置结构体中的偏移量换算回滑杆刻度。
 * @param[in] ratioOffset 配置中的偏移量，设计范围 [-100, 100]；
 *                        超出该范围不做夹取，按线性关系直接换算。
 * @return 滑杆刻度，等于 `ratioOffset + 100`；
 *         传入 0 时返回 100。
 * @note 与 sliderValueToRatioOffset() 互为逆运算。
 */
int ratioOffsetToSliderValue(int ratioOffset);

} // namespace h2core

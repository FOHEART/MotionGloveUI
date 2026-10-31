/**
 * @file  h2Widget_skeletonOpacity_ui_test.h
 * @brief 「骨骼」页签透明度滑块的 QtTest 界面用例声明。
 * @ingroup h2tests
 *
 * @details
 * 只验证对外契约：滑块当前值的读取接口、拖动后发出的信号，
 * 以及数值文案的刷新。渲染效果无法在无 GPU 的用例里断言，
 * 交给程序自带的内部截屏人工核对。
 *
 * fixture 约定：init() 新建并显示一个 h2Widget，cleanup() 销毁；
 * 两者缺一不可，否则数据行之间会累积滑块状态。
 */
#pragma once

#include <QObject>
#include <QScopedPointer>

#include "ui_test_support.h"

// QScopedPointer 的清理器要求类型完整，这里直接包含被测类头文件。
#include "h2Widget.h"

/**
 * @brief 骨骼透明度滑块的界面用例。
 */
class h2WidgetSkeletonOpacityUiTest : public QObject
{
    Q_OBJECT

public:
    /** @brief 析构；定义放在 .cpp 中，保证 QScopedPointer 析构时类型完整。 */
    ~h2WidgetSkeletonOpacityUiTest() override;

private slots:
    /** @brief 初始化归档目录并清空上一次产物。 */
    void initTestCase();

    /** @brief 每个用例前新建并显示一个 h2Widget。 */
    void init();

    /** @brief 每个用例后归档/补拍失败现场并销毁控件。 */
    void cleanup();

    /** @brief 写出图库与汇总。 */
    void cleanupTestCase();

    /** @brief 默认值：滑块为 100，即完全不透明，与新增功能前的画面一致。 */
    void defaultIsFullyOpaque();

    /** @brief 滑块取值范围必须是完整的 [0,100]，两端都可达。 */
    void sliderRangeCoversFullRange();

    /** @brief 拖动滑块发出 skeletonOpacityChanged，且携带新百分比。 */
    void movingSliderEmitsPercent();

    /** @brief 数值文案跟随滑块刷新（与信号同一路径，防止只改视图不改文案）。 */
    void valueTextFollowsSlider();

private:
    QScopedPointer<h2Widget> m_widget; ///< 当前用例的被测控件。
    ScreenshotArchive m_archive{QStringLiteral("h2WidgetSkeletonOpacityUiTest")}; ///< 截图归档器。
};

/**
 * @file  h2Widget_skeletonOpacity_ui_test.cpp
 * @brief 「骨骼」页签透明度滑块的 QtTest 界面用例实现。
 * @ingroup h2tests
 *
 * @details 用例意图写在每个槽函数上方的 Doxygen 块里；断言口径见函数体注释。
 */

#include "h2Widget_skeletonOpacity_ui_test.h"

#include "h2Widget.h"

#include <QLabel>
#include <QSignalSpy>
#include <QSlider>
#include <QWidget>

#include <QtTest/QtTest>

namespace
{

/// 透明度滑块的 objectName，见 ui/h2Widget.ui。
const char kOpacitySliderName[] = "SL_skeletonOpacity";

/// 透明度数值文案的 objectName，见 ui/h2Widget.ui。
const char kOpacityValueLabelName[] = "LBL_skeletonOpacityValue";

/// 取滑块；取不到时返回 nullptr，由调用方用 QVERIFY2 给出可读诊断。
QSlider* opacitySlider(h2Widget* widget)
{
    if (!widget)
    {
        return nullptr;
    }
    return widget->findChild<QSlider*>(QLatin1String(kOpacitySliderName));
}

/// 取数值文案标签；取不到时返回 nullptr。
QLabel* opacityValueLabel(h2Widget* widget)
{
    if (!widget)
    {
        return nullptr;
    }
    return widget->findChild<QLabel*>(QLatin1String(kOpacityValueLabelName));
}

} // namespace

h2WidgetSkeletonOpacityUiTest::~h2WidgetSkeletonOpacityUiTest() = default;

void h2WidgetSkeletonOpacityUiTest::initTestCase()
{
    m_archive.initTestCase();
}

void h2WidgetSkeletonOpacityUiTest::init()
{
    m_widget.reset(new h2Widget());
    m_widget->resize(960, 720);
    m_widget->show();
    QVERIFY(QTest::qWaitForWindowExposed(m_widget.data()));
}

void h2WidgetSkeletonOpacityUiTest::cleanup()
{
    if (QTest::currentTestFailed())
    {
        m_archive.captureOnFailure(m_widget.data());
    }
    m_widget.reset();
}

void h2WidgetSkeletonOpacityUiTest::cleanupTestCase()
{
    m_archive.finalize();
}

/**
 * @test   h2WidgetSkeletonOpacityUiTest::defaultIsFullyOpaque
 * @brief  默认状态：滑块为 100，读取接口返回 100。
 * @details 目的：新增控件不得改变既有画面。若默认值不是 100，用户升级后第一眼
 *          看到的骨骼就是半透明的，会被当成回归缺陷。
 *          设计思路：同时校验 .ui 里的滑块值与对外读取接口，两者漂移时立刻失败。
 * @expected 滑块 value() 为 100，skeletonOpacityPercent() 为 100
 * @priority P0
 */
void h2WidgetSkeletonOpacityUiTest::defaultIsFullyOpaque()
{
    QSlider* slider = opacitySlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_skeletonOpacity 的滑块");

    QCOMPARE(slider->value(), 100);
    QCOMPARE(m_widget->skeletonOpacityPercent(), 100);

    m_archive.capture(QStringLiteral("default-opaque"), m_widget.data(), slider);
}

/**
 * @test   h2WidgetSkeletonOpacityUiTest::sliderRangeCoversFullRange
 * @brief  滑块取值范围是完整的 [0,100]，两端都可到达。
 * @details 目的：需求是「从完全透明到完全不透明」连续可调；若最小值被写成 1
 *          或最大值被写成 99，用户永远看不到全透明/全不透明两端，且拖动到端点时
 *          与 3D 视图的实际 alpha 不一致。
 * @expected minimum 为 0，maximum 为 100
 * @priority P0
 */
void h2WidgetSkeletonOpacityUiTest::sliderRangeCoversFullRange()
{
    QSlider* slider = opacitySlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_skeletonOpacity 的滑块");

    QCOMPARE(slider->minimum(), 0);
    QCOMPARE(slider->maximum(), 100);
}

/**
 * @test   h2WidgetSkeletonOpacityUiTest::movingSliderEmitsPercent
 * @brief  拖动滑块发出一次 skeletonOpacityChanged，携带新百分比。
 * @details 目的：主界面靠这个信号驱动 3D 视图；信号不发、发两次或多发一次都会
 *          导致「拖了没反应」或「每帧重复下发」。设计思路：分别验证中间值 40 与
 *          端点 0，端点用来确认 0 不会被当成「无变化」而漏发。
 * @expected 40 → 一次信号值 40；0 → 一次信号值 0
 * @priority P0
 */
void h2WidgetSkeletonOpacityUiTest::movingSliderEmitsPercent()
{
    QSlider* slider = opacitySlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_skeletonOpacity 的滑块");

    QSignalSpy spy(m_widget.data(), &h2Widget::skeletonOpacityChanged);
    QVERIFY2(spy.isValid(), "skeletonOpacityChanged 未注册为可监听的信号");

    slider->setValue(40);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 40);

    slider->setValue(0);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toInt(), 0);

    m_archive.capture(QStringLiteral("slider-at-zero"), m_widget.data(), slider);
}

/**
 * @test   h2WidgetSkeletonOpacityUiTest::valueTextFollowsSlider
 * @brief  数值文案跟随滑块刷新为「<百分比>%」。
 * @details 目的：滑块没有刻度，用户只能靠这行文案判断当前值；文案不刷新时
 *          界面会一直显示旧值，看起来像滑块失灵。设计思路：中间值与端点各取一次，
 *          并断言百分比符号存在，避免只显示裸数字。
 * @expected 40 → "40%"；100 → "100%"
 * @priority P1
 */
void h2WidgetSkeletonOpacityUiTest::valueTextFollowsSlider()
{
    QSlider* slider = opacitySlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_skeletonOpacity 的滑块");

    QLabel* label = opacityValueLabel(m_widget.data());
    QVERIFY2(label != nullptr, "「骨骼」页签缺少名为 LBL_skeletonOpacityValue 的数值文案");

    slider->setValue(40);
    QCOMPARE(label->text(), QStringLiteral("40%"));

    slider->setValue(100);
    QCOMPARE(label->text(), QStringLiteral("100%"));

    m_archive.capture(QStringLiteral("value-text-100"), m_widget.data(), label);
}

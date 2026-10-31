/**
 * @file  h2Widget_jointOpacity_ui_test.cpp
 * @brief 「骨骼」页签关节点透明度滑块的 QtTest 界面用例实现。
 * @ingroup h2tests
 *
 * @details 用例意图写在每个槽函数上方的 Doxygen 块里；断言口径见函数体注释。
 */

#include "h2Widget_jointOpacity_ui_test.h"

#include "h2Widget.h"

#include <QLabel>
#include <QSignalSpy>
#include <QSlider>
#include <QWidget>

#include <QtTest/QtTest>

namespace
{

/// 关节点透明度滑块的 objectName，见 ui/h2Widget.ui。
const char kJointSliderName[] = "SL_jointOpacity";

/// 关节点透明度数值文案的 objectName，见 ui/h2Widget.ui。
const char kJointValueLabelName[] = "LBL_jointOpacityValue";

/// 骨骼段透明度滑块的 objectName，用于「两条通道互不影响」的对照断言。
const char kSkeletonSliderName[] = "SL_skeletonOpacity";

/// 取关节点滑块；取不到时返回 nullptr，由调用方用 QVERIFY2 给出可读诊断。
QSlider* jointSlider(h2Widget* widget)
{
    if (!widget)
    {
        return nullptr;
    }
    return widget->findChild<QSlider*>(QLatin1String(kJointSliderName));
}

/// 取关节点数值文案标签；取不到时返回 nullptr。
QLabel* jointValueLabel(h2Widget* widget)
{
    if (!widget)
    {
        return nullptr;
    }
    return widget->findChild<QLabel*>(QLatin1String(kJointValueLabelName));
}

} // namespace

h2WidgetJointOpacityUiTest::~h2WidgetJointOpacityUiTest() = default;

void h2WidgetJointOpacityUiTest::initTestCase()
{
    m_archive.initTestCase();
}

void h2WidgetJointOpacityUiTest::init()
{
    m_widget.reset(new h2Widget());
    m_widget->resize(960, 720);
    m_widget->show();
    QVERIFY(QTest::qWaitForWindowExposed(m_widget.data()));
}

void h2WidgetJointOpacityUiTest::cleanup()
{
    if (QTest::currentTestFailed())
    {
        m_archive.captureOnFailure(m_widget.data());
    }
    m_widget.reset();
}

void h2WidgetJointOpacityUiTest::cleanupTestCase()
{
    m_archive.finalize();
}

/**
 * @test   h2WidgetJointOpacityUiTest::defaultIsFullyOpaque
 * @brief  默认状态：关节点滑块为 100，读取接口返回 100。
 * @details 目的：新增控件不得改变既有画面；若默认不是 100，升级后关节点球会
 *          一上来就是半透明的，渲染结果与改动前不一致。
 *          设计思路：同时校验 .ui 里的滑块值与对外读取接口，两者漂移时立刻失败。
 * @expected 滑块 value() 为 100，jointOpacityPercent() 为 100
 * @priority P0
 */
void h2WidgetJointOpacityUiTest::defaultIsFullyOpaque()
{
    QSlider* slider = jointSlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_jointOpacity 的滑块");

    QCOMPARE(slider->value(), 100);
    QCOMPARE(m_widget->jointOpacityPercent(), 100);

    m_archive.capture(QStringLiteral("default-opaque"), m_widget.data(), slider);
}

/**
 * @test   h2WidgetJointOpacityUiTest::sliderRangeCoversFullRange
 * @brief  关节点滑块取值范围是完整的 [0,100]。
 * @details 目的：需求是「关节点可从完全透明调回完全不透明」；量程被截断时
 *          用户无法关掉红球，或无法恢复默认画面。设计思路：同时校验上下限。
 * @expected minimum 为 0，maximum 为 100
 * @priority P0
 */
void h2WidgetJointOpacityUiTest::sliderRangeCoversFullRange()
{
    QSlider* slider = jointSlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_jointOpacity 的滑块");

    QCOMPARE(slider->minimum(), 0);
    QCOMPARE(slider->maximum(), 100);
}

/**
 * @test   h2WidgetJointOpacityUiTest::movingSliderEmitsPercent
 * @brief  拖动关节点滑块发出一次 jointOpacityChanged，携带新百分比。
 * @details 目的：主界面靠这个信号驱动 3D 视图的关节点渲染；信号不发就「拖了没反应」，
 *          发两次会重复下发。设计思路：中间值 30 与端点 0 各一次，端点用于确认
 *          0 不会被当成「无变化」而漏发。
 * @expected 30 → 一次信号值 30；0 → 一次信号值 0
 * @priority P0
 */
void h2WidgetJointOpacityUiTest::movingSliderEmitsPercent()
{
    QSlider* slider = jointSlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_jointOpacity 的滑块");

    QSignalSpy spy(m_widget.data(), &h2Widget::jointOpacityChanged);
    QVERIFY2(spy.isValid(), "jointOpacityChanged 未注册为可监听的信号");

    slider->setValue(30);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 30);

    slider->setValue(0);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toInt(), 0);

    m_archive.capture(QStringLiteral("joint-at-zero"), m_widget.data(), slider);
}

/**
 * @test   h2WidgetJointOpacityUiTest::valueTextFollowsSlider
 * @brief  关节点数值文案跟随关节点滑块刷新为「<百分比>%」。
 * @details 目的：两个滑块各有一行文案，若共用同一个标签或接错信号，
 *          会看到「拖 A 改 B」的错乱。设计思路：先确认初始 100%，再改到 65%
 *          并断言文案，最后把骨骼滑块拨到 20% 确认关节点文案不受影响。
 * @expected 65 → "65%"；骨骼滑块改到 20% 后关节点文案仍为 "65%"
 * @priority P1
 */
void h2WidgetJointOpacityUiTest::valueTextFollowsSlider()
{
    QSlider* slider = jointSlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_jointOpacity 的滑块");

    QLabel* label = jointValueLabel(m_widget.data());
    QVERIFY2(label != nullptr, "「骨骼」页签缺少名为 LBL_jointOpacityValue 的数值文案");

    slider->setValue(65);
    QCOMPARE(label->text(), QStringLiteral("65%"));

    if (QSlider* skeletonSlider = m_widget->findChild<QSlider*>(QLatin1String(kSkeletonSliderName)))
    {
        skeletonSlider->setValue(20);
        QCOMPARE(label->text(), QStringLiteral("65%"));
    }

    m_archive.capture(QStringLiteral("joint-value-65"), m_widget.data(), label);
}

/**
 * @test   h2WidgetJointOpacityUiTest::doesNotAffectSkeletonChannel
 * @brief  拖关节点滑块不改变骨骼滑块的数值与骨骼读取接口。
 * @details 目的：需求明确要求两条通道分开管理。若把两个滑块接到同一个成员或
 *          同一份配置，用户想单独隐藏红球时骨骼也会跟着变淡，这是本需求最容易
 *          犯的错。设计思路：改动关节点滑块后，同时断言骨骼滑块 value() 与
 *          skeletonOpacityPercent() 仍为 100。
 * @expected 骨骼滑块仍为 100，skeletonOpacityPercent() 仍为 100
 * @priority P0
 */
void h2WidgetJointOpacityUiTest::doesNotAffectSkeletonChannel()
{
    QSlider* slider = jointSlider(m_widget.data());
    QVERIFY2(slider != nullptr, "「骨骼」页签缺少名为 SL_jointOpacity 的滑块");

    QSlider* skeletonSlider = m_widget->findChild<QSlider*>(QLatin1String(kSkeletonSliderName));
    QVERIFY2(skeletonSlider != nullptr, "「骨骼」页签缺少名为 SL_skeletonOpacity 的骨骼滑块");

    QCOMPARE(skeletonSlider->value(), 100);

    slider->setValue(0);

    QCOMPARE(skeletonSlider->value(), 100);
    QCOMPARE(m_widget->skeletonOpacityPercent(), 100);
    QCOMPARE(m_widget->jointOpacityPercent(), 0);

    m_archive.capture(QStringLiteral("joint-zero-skeleton-intact"), m_widget.data(), slider);
}

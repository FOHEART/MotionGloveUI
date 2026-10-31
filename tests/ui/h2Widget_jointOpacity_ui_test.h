/**
 * @file  h2Widget_jointOpacity_ui_test.h
 * @brief 「骨骼」页签关节点透明度滑块的 QtTest 界面用例声明。
 * @ingroup h2tests
 *
 * @details
 * 关节点（腕关节、各指节球、指尖红球）与骨骼段的透明度是两条独立通道，
 * 本用例只验证关节点这一条：读取接口、拖动后的信号、数值文案刷新，
 * 并显式验证「拖动本滑块不影响骨骼滑块」，防止两条通道被错误地接到同一份状态上。
 *
 * 渲染效果无法在无 GPU 的用例里断言，交给程序自带的内部截屏人工核对。
 *
 * fixture 约定：init() 新建并显示一个 h2Widget，cleanup() 销毁。
 */
#pragma once

#include <QObject>
#include <QScopedPointer>

#include "ui_test_support.h"

// QScopedPointer 的清理器要求类型完整，这里直接包含被测类头文件。
#include "h2Widget.h"

/**
 * @brief 关节点透明度滑块的界面用例。
 */
class h2WidgetJointOpacityUiTest : public QObject
{
    Q_OBJECT

public:
    /** @brief 析构；定义放在 .cpp 中，保证 QScopedPointer 析构时类型完整。 */
    ~h2WidgetJointOpacityUiTest() override;

private slots:
    /** @brief 初始化归档目录并清空上一次产物。 */
    void initTestCase();

    /** @brief 每个用例前新建并显示一个 h2Widget。 */
    void init();

    /** @brief 每个用例后归档/补拍失败现场并销毁控件。 */
    void cleanup();

    /** @brief 写出图库与汇总。 */
    void cleanupTestCase();

    /** @brief 默认值：关节点滑块为 100，与新增功能前的画面一致。 */
    void defaultIsFullyOpaque();

    /** @brief 滑块取值范围是完整的 [0,100]，两端都可达。 */
    void sliderRangeCoversFullRange();

    /** @brief 拖动关节点滑块发出 jointOpacityChanged，携带新百分比。 */
    void movingSliderEmitsPercent();

    /** @brief 关节点数值文案跟随自身滑块刷新。 */
    void valueTextFollowsSlider();

    /** @brief 两条通道互不影响：拖关节点滑块不会改变骨骼滑块与骨骼读取值。 */
    void doesNotAffectSkeletonChannel();

private:
    QScopedPointer<h2Widget> m_widget; ///< 当前用例的被测控件。
    ScreenshotArchive m_archive{QStringLiteral("h2WidgetJointOpacityUiTest")}; ///< 截图归档器。
};

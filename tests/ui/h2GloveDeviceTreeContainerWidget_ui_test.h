/**
 * @file  h2GloveDeviceTreeContainerWidget_ui_test.h
 * @brief 设备树容器控件（h2GloveDeviceTreeContainerWidget）的 QtTest 用例声明。
 * @ingroup h2tests
 *
 * @details 只验证公开契约：卡片增删计数、点击后的单选互斥、清空。
 *          不触发 startDeviceScan()，避免用例依赖真实硬件与 SDK 返回值。
 */
#pragma once

#include <QObject>
#include <QScopedPointer>

#include "ui_test_support.h"

// QScopedPointer 的清理器要求类型完整，这里直接包含被测类头文件。
#include "h2GloveDeviceTreeContainerWidget.h"

class h2GloveDeviceTreeWidget;

/**
 * @brief 设备树容器控件的界面用例。
 *
 * @details fixture 约定：init() 新建并显示空容器，cleanup() 销毁；
 *          卡片由各用例自行创建后交给容器托管。
 */
class h2GloveDeviceTreeContainerWidgetUiTest : public QObject
{
    Q_OBJECT

public:
    /** @brief 析构；定义放在 .cpp 中，保证 QScopedPointer 析构时类型完整。 */
    ~h2GloveDeviceTreeContainerWidgetUiTest() override;

private slots:
    /** @brief 初始化归档目录并清空上一次产物。 */
    void initTestCase();

    /** @brief 每个用例前新建并显示空容器。 */
    void init();

    /** @brief 每个用例后归档/补拍失败现场并销毁容器。 */
    void cleanup();

    /** @brief 写出图库与汇总。 */
    void cleanupTestCase();

    /** @brief 卡片计数只统计卡片本身，不把布局里的弹簧项算进去。 */
    void countIgnoresLayoutSpacer();

    /** @brief 点击某张卡片后，只有它处于选中态。 */
    void leftClickSelectsExactlyOneCard();

    /** @brief 移除卡片后计数递减；移除选中项后不再有选中卡片。 */
    void removeCardUpdatesCountAndSelection();

    /** @brief 清空后计数归零且不再残留卡片对象。 */
    void clearRemovesAllCards();

private:
    /**
     * @brief 创建一张带序列号与名称的卡片并交给容器托管。
     * @param[in] sn   序列号原始值，用于区分不同卡片。
     * @param[in] name 设备名，写入卡片并随选中信号发出。
     * @return 托管后的卡片指针，**不会为 nullptr**。
     */
    h2GloveDeviceTreeWidget* addCard(unsigned int sn, const QString& name);

    QScopedPointer<h2GloveDeviceTreeContainerWidget> m_container; ///< 当前用例的容器实例。
    ScreenshotArchive m_archive{QStringLiteral("h2GloveDeviceTreeContainerWidgetUiTest")}; ///< 截图归档器。
};

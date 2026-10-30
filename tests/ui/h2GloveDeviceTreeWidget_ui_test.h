/**
 * @file  h2GloveDeviceTreeWidget_ui_test.h
 * @brief 设备卡片控件（h2GloveDeviceTreeWidget）的 QtTest 界面用例声明。
 * @ingroup h2tests
 *
 * @details 只在导出接口上做验证：构造、状态设置与鼠标点击后的信号。
 *          每个用例结束后按需归档整窗截图，失败时在 cleanup() 里补拍现场。
 */
#pragma once

#include <QObject>
#include <QScopedPointer>

#include "ui_test_support.h"

// QScopedPointer 的清理器要求类型完整，这里直接包含被测类头文件。
#include "h2GloveDeviceTreeWidget.h"

/**
 * @brief 设备卡片控件的界面用例。
 *
 * @details fixture 约定：init() 新建卡片并显示，cleanup() 销毁。
 *          两者缺一不可，否则多个数据行之间会累积状态、截图互相污染。
 */
class h2GloveDeviceTreeWidgetUiTest : public QObject
{
    Q_OBJECT

public:
    /** @brief 析构；定义放在 .cpp 中，保证 QScopedPointer 析构时类型完整。 */
    ~h2GloveDeviceTreeWidgetUiTest() override;

private slots:
    /** @brief 初始化归档目录并清空上一次产物。 */
    void initTestCase();

    /** @brief 每个用例前新建并显示一张设备卡片。 */
    void init();

    /** @brief 每个用例后归档/补拍失败现场并销毁卡片。 */
    void cleanup();

    /** @brief 写出图库与汇总。 */
    void cleanupTestCase();

    /** @brief 数据行：在线状态、电量、选中态的等价类。 */
    void deviceCardState_data();
    /** @brief 状态设置后各项读取值与设置值一致。 */
    void deviceCardState();

    /** @brief 左键点击卡片发出 suitSelectedLive，且携带正确序列号与名称。 */
    void leftClickEmitsSuitSelectedLive();

    /** @brief 右键点击不发出选中信号（避免右键菜单误触发选中）。 */
    void rightClickIsIgnored();

private:
    QScopedPointer<h2GloveDeviceTreeWidget> m_card; ///< 当前用例的卡片实例。
    ScreenshotArchive m_archive{QStringLiteral("h2GloveDeviceTreeWidgetUiTest")}; ///< 截图归档器。
};

/**
 * @file  h2Widget_mocapModePanel_ui_test.h
 * @brief 「手套采集模式」三组手指角度调节面板的显示/隐藏界面的用例声明。
 * @ingroup h2tests
 *
 * @details
 * 三组面板（自由模式 / 分指锁定 / 混合模式）是互斥的：同一时刻只能显示一组。
 * 本用例只验证「谁能被看到」这一层契约——软件刚打开时自由模式那组必须已经可见，
 * 点别的模式后必须跟着切换。面板内部的数值渲染不在此断言。
 *
 * fixture 约定：init() 新建并显示一个 h2Widget，cleanup() 销毁；
 * 两者缺一不可，否则用例之间会累积可见性状态。
 */
#pragma once

#include <QObject>
#include <QScopedPointer>

#include "ui_test_support.h"

// QScopedPointer 的清理器要求类型完整，这里直接包含被测类头文件。
#include "h2Widget.h"

/**
 * @brief 采集模式面板显示/隐藏的界面用例。
 */
class h2WidgetMocapModePanelUiTest : public QObject
{
    Q_OBJECT

public:
    /** @brief 析构；定义放在 .cpp 中，保证 QScopedPointer 析构时类型完整。 */
    ~h2WidgetMocapModePanelUiTest() override;

private slots:
    /** @brief 初始化归档目录并清空上一次产物。 */
    void initTestCase();

    /** @brief 每个用例前新建并显示一个 h2Widget。 */
    void init();

    /** @brief 每个用例后归档/补拍失败现场并销毁控件。 */
    void cleanup();

    /** @brief 写出图库与汇总。 */
    void cleanupTestCase();

    /** @brief 刚构造完就显示自由模式那组，其余两组隐藏。 */
    void freeModePanelVisibleOnStartup();

    /** @brief 未绑定设备时切换到分指锁定，可见性也要跟着切换。 */
    void switchingModeWithoutDeviceStillSwitchesPanels();

private:
    QScopedPointer<h2Widget> m_widget; ///< 当前用例的被测控件。
    ScreenshotArchive m_archive{QStringLiteral("h2WidgetMocapModePanelUiTest")}; ///< 截图归档器。
};

/**
 * @file  h2GloveConfigSnapshot_ui_test.h
 * @brief 把右侧各面板当前界面值收集成套装配置的界面用例声明。
 * @ingroup h2tests
 *
 * @details
 * 手套下线时要一次性把界面上的所有设置写盘，这要求 h2Widget 能"反向收集"：
 * 从各模式面板的滑杆读值填进 h2GloveMgr。本用例不接设备、不写盘，
 * 只验证收集结果本身——左右手、三个模式各自的字段映射是否正确。
 *
 * fixture 约定：init() 新建并显示一个 h2Widget，cleanup() 销毁。
 */
#pragma once

#include <QObject>
#include <QScopedPointer>

#include "ui_test_support.h"

#include "h2Widget.h"

/**
 * @brief 界面值 → 套装配置结构体的收集用例。
 */
class h2GloveConfigSnapshotUiTest : public QObject
{
    Q_OBJECT

public:
    /** @brief 析构；定义放在 .cpp 中，保证 QScopedPointer 析构时类型完整。 */
    ~h2GloveConfigSnapshotUiTest() override;

private slots:
    /** @brief 初始化归档目录。 */
    void initTestCase();

    /** @brief 每个用例前新建并显示一个 h2Widget。 */
    void init();

    /** @brief 每个用例后归档失败现场并销毁控件。 */
    void cleanup();

    /** @brief 写出图库与汇总。 */
    void cleanupTestCase();

    /** @brief 自由模式的拇指角度按左右手分别落到对应关节上。 */
    void freeModeThumbAnglesFollowTheirHand();

    /** @brief 分指锁定的「分指张开 / 握拳」角度落到对应手的字段上。 */
    void lockYawStretchAndFistFollowTheirHand();

    /** @brief 混合模式的滑杆落到增强模式专用字段上。 */
    void enhancedStretchLandsInEnhancedField();

private:
    QScopedPointer<h2Widget> m_widget; ///< 当前用例的被测控件。
    ScreenshotArchive m_archive{QStringLiteral("h2GloveConfigSnapshotUiTest")}; ///< 截图归档器。
};

/**
 * @file  h2StreamConfigWidget_ui_test.h
 * @brief 「转发格式」页签骨骼段数分组的 QtTest 界面用例声明。
 * @ingroup h2tests
 *
 * @details
 * 只验证对外契约：分组下三个互斥选项的存在性、初始状态来自 SDK 当前配置，
 * 以及改选后能写回 SDK 配置（SDK 内部的落盘由 SDK 自己保证）。
 *
 * fixture 约定：init() 新建并显示一个 h2StreamConfigWidget，cleanup() 销毁；
 * 用例里若需要"按特定配置构造"，先把配置写进 SDK 再重建控件。
 */
#pragma once

#include <QObject>
#include <QScopedPointer>

#include "ui_test_support.h"

#include "h2sdk.h"

// QScopedPointer 的清理器要求类型完整，这里直接包含被测类头文件。
#include "h2StreamConfigWidget.h"

/**
 * @brief 骨骼段数（32 / 42 / OpenXR）分组的界面用例。
 */
class h2StreamConfigWidgetUiTest : public QObject
{
    Q_OBJECT

public:
    /** @brief 析构；定义放在 .cpp 中，保证 QScopedPointer 析构时类型完整。 */
    ~h2StreamConfigWidgetUiTest() override;

private slots:
    /** @brief 初始化归档目录、备份 SDK 当前配置并清空上一次产物。 */
    void initTestCase();

    /** @brief 每个用例前新建并显示一个 h2StreamConfigWidget。 */
    void init();

    /** @brief 每个用例后归档/补拍失败现场并销毁控件。 */
    void cleanup();

    /** @brief 还原 SDK 配置并写出图库与汇总。 */
    void cleanupTestCase();

    /** @brief 旧的「双手32段骨骼模型」提示标签必须已从界面移除。 */
    void hintLabelRemoved();

    /** @brief 没有配置文件时默认勾选 42 段。 */
    void skeletonFormatDefaultsTo42WithoutConfig();

    /** @brief 新增分组位于「位移」与「旋转」之间，且三个选项互斥。 */
    void skeletonFormatGroupHasThreeExclusiveOptions();

    /** @brief 打开界面时按 SDK 当前配置回填勾选状态。 */
    void skeletonFormatReflectsConfig();

    /** @brief 改选后写回 SDK 配置（三条路径都要覆盖到）。 */
    void skeletonFormatWritesBack();

private:
    /** @brief 销毁并按当前 SDK 配置重建被测控件。 */
    void rebuildWidget();

    QScopedPointer<h2StreamConfigWidget> m_widget; ///< 当前用例的被测控件。
    ScreenshotArchive m_archive{QStringLiteral("h2StreamConfigWidgetUiTest")}; ///< 截图归档器。
    h2GloveStreamConfig m_originalConfig; ///< 用例开始前的 SDK 配置，cleanupTestCase 还原。
};

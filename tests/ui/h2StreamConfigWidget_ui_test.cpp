/**
 * @file  h2StreamConfigWidget_ui_test.cpp
 * @brief 「转发格式」页签骨骼段数分组的 QtTest 界面用例实现。
 * @ingroup h2tests
 *
 * @details 用例意图写在每个槽函数上方的 Doxygen 块里；断言口径见函数体注释。
 */

#include "h2StreamConfigWidget_ui_test.h"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QRadioButton>
#include <QWidget>

#include <QtTest/QtTest>

namespace
{

/// 骨骼段数分组的 objectName，见 ui/steamvr/h2StreamConfigWidget.ui。
const char kFormatGroupName[] = "GBX_skeletonFormat";

/// 三个互斥选项的 objectName，见 ui/steamvr/h2StreamConfigWidget.ui。
const char kFormat32Name[] = "RB_skeleton32";
const char kFormat42Name[] = "RB_skeleton42";
const char kFormatOpenXRName[] = "RB_skeletonOpenXR";

/// 被移除的旧提示标签的 objectName（现在只用来断言"确实不存在了"）。
const char kRemovedHintLabelName[] = "LBL_skeleton_hint";

/// 相邻分组，用来断言新分组的插入位置。
const char kDisplacementGroupName[] = "GBX_loc";
const char kRotationGroupName[] = "GBX_rot";

/// 取具名子控件；取不到时返回 nullptr，由调用方用 QVERIFY2 给出可读诊断。
template <typename T>
T* child(h2StreamConfigWidget* widget, const char* name)
{
    if (!widget)
    {
        return nullptr;
    }
    return widget->findChild<T*>(QLatin1String(name));
}

/// 把枚举映射到对应的单选按钮 objectName；未知值按 32 段处理（与控件回填逻辑一致）。
const char* radioNameOf(SkeletonFormat_TypeDef format)
{
    switch (format)
    {
        case SkeletonFormat_42:
            return kFormat42Name;
        case SkeletonFormat_OpenXR:
            return kFormatOpenXRName;
        default:
            return kFormat32Name;
    }
}

} // namespace

h2StreamConfigWidgetUiTest::~h2StreamConfigWidgetUiTest() = default;

void h2StreamConfigWidgetUiTest::initTestCase()
{
    m_archive.initTestCase();
    H2_SDK_getGloveStreamConfig(&m_originalConfig);
}

void h2StreamConfigWidgetUiTest::init()
{
    rebuildWidget();
    QVERIFY2(m_widget != nullptr, "无法构造 h2StreamConfigWidget");
    QVERIFY(QTest::qWaitForWindowExposed(m_widget.data()));
}

void h2StreamConfigWidgetUiTest::cleanup()
{
    if (QTest::currentTestFailed())
    {
        m_archive.captureOnFailure(m_widget.data());
    }
    m_widget.reset();
}

void h2StreamConfigWidgetUiTest::cleanupTestCase()
{
    // 用例里会改动全局流配置，跑完还原成进入测试前的样子，避免影响后续用例类
    H2_SDK_setGloveStreamConfig(m_originalConfig);
    m_archive.finalize();
}

void h2StreamConfigWidgetUiTest::rebuildWidget()
{
    m_widget.reset(new h2StreamConfigWidget());
    m_widget->resize(760, 780);
    m_widget->show();
}

/**
 * @test   h2StreamConfigWidgetUiTest::hintLabelRemoved
 * @brief  「双手32段骨骼模型。」这条提示文案已从界面上拿掉。
 * @details 目的：32 段不再是唯一格式，写死"32段"的提示会与新分组自相矛盾。
 *          设计思路：直接按 objectName 找旧标签，找不到才算通过——
 *          只断言"没有选中 32 段"是不够的，标签可能仍然留在界面上。
 * @expected 找不到 LBL_skeleton_hint
 * @priority P0
 */
void h2StreamConfigWidgetUiTest::hintLabelRemoved()
{
    QLabel* hintLabel = child<QLabel>(m_widget.data(), kRemovedHintLabelName);
    QVERIFY2(hintLabel == nullptr, "旧的 LBL_skeleton_hint 提示标签仍然存在");

    m_archive.capture(QStringLiteral("01-hint-label-removed"), m_widget.data());
}

/**
 * @test   h2StreamConfigWidgetUiTest::skeletonFormatGroupHasThreeExclusiveOptions
 * @brief  新增的骨骼段数分组在「位移」与「旋转」之间，三个选项互斥。
 * @details 目的：用户要求 32 / 42 / OpenXR 只能三选一，
 *          且分组位置就排在"位移"和"旋转"两个大类中间。
 *          设计思路：先用布局里的索引断言位置（新分组索引 = 位移 + 1，旋转 = 位移 + 2），
 *          再依次点击三个选项，断言任意时刻只有被点的那一个处于选中状态。
 * @expected 分组存在、位置正确、三选项互斥
 * @priority P0
 */
void h2StreamConfigWidgetUiTest::skeletonFormatGroupHasThreeExclusiveOptions()
{
    QGroupBox* formatGroup = child<QGroupBox>(m_widget.data(), kFormatGroupName);
    QVERIFY2(formatGroup != nullptr, "缺少骨骼段数分组 GBX_skeletonFormat");

    QGroupBox* displacementGroup = child<QGroupBox>(m_widget.data(), kDisplacementGroupName);
    QGroupBox* rotationGroup = child<QGroupBox>(m_widget.data(), kRotationGroupName);
    QVERIFY2(displacementGroup != nullptr, "缺少位移分组 GBX_loc");
    QVERIFY2(rotationGroup != nullptr, "缺少旋转分组 GBX_rot");

    QWidget* container = formatGroup->parentWidget();
    QVERIFY2(container != nullptr, "骨骼段数分组没有父控件");
    QVERIFY2(displacementGroup->parentWidget() == container, "位移分组与骨骼段数分组不在同一容器");
    QVERIFY2(rotationGroup->parentWidget() == container, "旋转分组与骨骼段数分组不在同一容器");

    QLayout* layout = container->layout();
    QVERIFY2(layout != nullptr, "分组容器缺少布局");
    const int displacementIndex = layout->indexOf(displacementGroup);
    const int formatIndex = layout->indexOf(formatGroup);
    const int rotationIndex = layout->indexOf(rotationGroup);
    QCOMPARE(formatIndex, displacementIndex + 1);
    QCOMPARE(rotationIndex, displacementIndex + 2);

    QRadioButton* radio32 = child<QRadioButton>(m_widget.data(), kFormat32Name);
    QRadioButton* radio42 = child<QRadioButton>(m_widget.data(), kFormat42Name);
    QRadioButton* radioOpenXR = child<QRadioButton>(m_widget.data(), kFormatOpenXRName);
    QVERIFY2(radio32 != nullptr, "缺少 32 段单选按钮 RB_skeleton32");
    QVERIFY2(radio42 != nullptr, "缺少 42 段单选按钮 RB_skeleton42");
    QVERIFY2(radioOpenXR != nullptr, "缺少 OpenXR 单选按钮 RB_skeletonOpenXR");

    QRadioButton* radios[] = {radio32, radio42, radioOpenXR};
    for (QRadioButton* clicked : radios)
    {
        QTest::mouseClick(clicked, Qt::LeftButton);
        QVERIFY2(clicked->isChecked(), "点击后单选按钮未选中");
        for (QRadioButton* other : radios)
        {
            if (other != clicked)
            {
                QVERIFY2(!other->isChecked(), "两个骨骼段数选项同时被选中");
            }
        }
    }

    m_archive.capture(QStringLiteral("02-format-group-exclusive"), m_widget.data(), formatGroup);
}

/**
 * @test   h2StreamConfigWidgetUiTest::skeletonFormatReflectsConfig
 * @brief  打开界面时按 SDK 当前配置勾选对应选项，而不是永远停在 .ui 里的默认值。
 * @details 目的：.ui 里 RB_skeleton32 默认就是勾上的，如果读取链路断开，
 *          "配置=32 时显示 32"这种断言会假通过。
 *          设计思路：先把 SDK 配置改成一个"当前不是它"的非默认值，再重建控件，
 *          断言只有对应选项被勾选。
 * @expected 勾选状态与 SDK 配置一致
 * @priority P0
 */
void h2StreamConfigWidgetUiTest::skeletonFormatReflectsConfig()
{
    h2GloveStreamConfig cfg;
    H2_SDK_getGloveStreamConfig(&cfg);

    const SkeletonFormat_TypeDef target =
        (cfg.skeletonFormat == SkeletonFormat_42) ? SkeletonFormat_OpenXR : SkeletonFormat_42;
    cfg.skeletonFormat = target;
    H2_SDK_setGloveStreamConfig(cfg);

    rebuildWidget();
    QVERIFY2(m_widget != nullptr, "无法按指定配置构造 h2StreamConfigWidget");
    QVERIFY(QTest::qWaitForWindowExposed(m_widget.data()));

    const char* expectedName = radioNameOf(target);
    QRadioButton* expected = child<QRadioButton>(m_widget.data(), expectedName);
    QVERIFY2(expected != nullptr, qPrintable(QStringLiteral("缺少单选按钮 %1").arg(QLatin1String(expectedName))));
    QVERIFY2(expected->isChecked(),
             qPrintable(QStringLiteral("配置为 %1 时未勾选对应选项").arg(static_cast<int>(target))));

    QRadioButton* radios[] = {
        child<QRadioButton>(m_widget.data(), kFormat32Name),
        child<QRadioButton>(m_widget.data(), kFormat42Name),
        child<QRadioButton>(m_widget.data(), kFormatOpenXRName),
    };
    for (QRadioButton* radio : radios)
    {
        QVERIFY2(radio != nullptr, "缺少骨骼段数单选按钮");
        if (radio != expected)
        {
            QVERIFY2(!radio->isChecked(), "非当前配置的选项也被勾选了");
        }
    }

    m_archive.capture(QStringLiteral("03-format-reflects-config"), m_widget.data(), expected);
}

/**
 * @test   h2StreamConfigWidgetUiTest::skeletonFormatWritesBack
 * @brief  改选三个选项后，writeH2StreamConfig() 都会把新格式写回 SDK 配置。
 * @details 目的：设置对话框点「确定」时调用 writeH2StreamConfig()，
 *          漏掉 skeletonFormat 就会让界面选择永远落不到配置文件里。
 *          设计思路：依次点 42 / OpenXR / 32 三个选项，每次都调用写回接口，
 *          再用 H2_SDK_getGloveStreamConfig() 读回来比对枚举值——
 *          三个方向都走一遍，避免只测到一条 if 分支。
 * @expected 读回的 skeletonFormat 与界面所选一致
 * @priority P0
 */
void h2StreamConfigWidgetUiTest::skeletonFormatWritesBack()
{
    const struct
    {
        const char* radioName;
        SkeletonFormat_TypeDef expected;
    } cases[] = {
        {kFormat42Name, SkeletonFormat_42},
        {kFormatOpenXRName, SkeletonFormat_OpenXR},
        {kFormat32Name, SkeletonFormat_32},
    };

    for (const auto& testCase : cases)
    {
        QRadioButton* radio = child<QRadioButton>(m_widget.data(), testCase.radioName);
        QVERIFY2(radio != nullptr, qPrintable(QStringLiteral("缺少单选按钮 %1").arg(QLatin1String(testCase.radioName))));

        QTest::mouseClick(radio, Qt::LeftButton);
        QVERIFY2(radio->isChecked(), "点击后单选按钮未选中");

        m_widget->writeH2StreamConfig();

        h2GloveStreamConfig written;
        H2_SDK_getGloveStreamConfig(&written);
        QCOMPARE(static_cast<int>(written.skeletonFormat), static_cast<int>(testCase.expected));
    }

    m_archive.capture(QStringLiteral("04-format-writes-back"), m_widget.data());
}

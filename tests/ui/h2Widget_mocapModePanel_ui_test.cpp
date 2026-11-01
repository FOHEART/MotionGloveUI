/**
 * @file  h2Widget_mocapModePanel_ui_test.cpp
 * @brief 「手套采集模式」三组面板显示/隐藏的界面的用例实现。
 * @ingroup h2tests
 *
 * @details 用例意图写在每个槽函数上方的 Doxygen 块里；断言口径见函数体注释。
 */

#include "h2Widget_mocapModePanel_ui_test.h"

#include "h2Widget.h"

#include <QRadioButton>
#include <QWidget>

#include <QtTest/QtTest>

namespace
{

/// 三组面板的 objectName，见 ui/h2Widget.cpp 的构造代码。
const char kFreeModeLeftName[] = "WIG_fingerAdjustFreeMode_LH";
const char kFreeModeRightName[] = "WIG_fingerAdjustFreeMode_RH";
const char kLockYawLeftName[] = "WIG_fingerAdjustLockYaw_LH";
const char kLockYawRightName[] = "WIG_fingerAdjustLockYaw_RH";
const char kEnhancedLeftName[] = "WIG_fingerAdjustEnhanced_LH";
const char kEnhancedRightName[] = "WIG_fingerAdjustEnhanced_RH";

/// 采集模式单选按钮的 objectName，见 ui/h2Widget.ui。
const char kLockModeRadioName[] = "RB_lockMode";
const char kFreeModeRadioName[] = "RB_freeMode";

/// 取具名子控件；取不到时返回 nullptr，由调用方用 QVERIFY2 给出可读诊断。
QWidget* child(h2Widget* widget, const char* name)
{
    if (!widget)
    {
        return nullptr;
    }
    return widget->findChild<QWidget*>(QLatin1String(name));
}

} // namespace

h2WidgetMocapModePanelUiTest::~h2WidgetMocapModePanelUiTest() = default;

void h2WidgetMocapModePanelUiTest::initTestCase()
{
    m_archive.initTestCase();
}

void h2WidgetMocapModePanelUiTest::init()
{
    m_widget.reset(new h2Widget());
    m_widget->resize(960, 720);
    m_widget->show();
    QVERIFY(QTest::qWaitForWindowExposed(m_widget.data()));
}

void h2WidgetMocapModePanelUiTest::cleanup()
{
    if (QTest::currentTestFailed())
    {
        m_archive.captureOnFailure(m_widget.data());
    }
    m_widget.reset();
}

void h2WidgetMocapModePanelUiTest::cleanupTestCase()
{
    m_archive.finalize();
}

/**
 * @test   h2WidgetMocapModePanelUiTest::freeModePanelVisibleOnStartup
 * @brief  软件刚打开（未绑定任何设备）时，自由模式那两块面板就已经可见。
 * @details 目的：三组面板在构造时被统一隐藏，如果没有代码把它们显示回来，
 *          用户打开软件看到的是空白区域，必须先点一次「自由模式」才会出现内容。
 *          设计思路：直接构造 h2Widget（m_sn 仍为 0），断言自由模式左右两块
 *          `!isHidden()`，同时断言另外两组仍隐藏，避免"全都显示"这种假通过。
 * @expected 自由模式 LH/RH 可见；分指锁定与混合模式四块全部隐藏
 * @priority P0
 */
void h2WidgetMocapModePanelUiTest::freeModePanelVisibleOnStartup()
{
    QWidget* freeLeft = child(m_widget.data(), kFreeModeLeftName);
    QWidget* freeRight = child(m_widget.data(), kFreeModeRightName);
    QVERIFY2(freeLeft != nullptr, "缺少自由模式左手段 WIG_fingerAdjustFreeMode_LH");
    QVERIFY2(freeRight != nullptr, "缺少自由模式右手 WIG_fingerAdjustFreeMode_RH");

    QVERIFY2(!freeLeft->isHidden(), "软件刚打开时自由模式左手段没有显示");
    QVERIFY2(!freeRight->isHidden(), "软件刚打开时自由模式右手没有显示");

    const char* others[] = {kLockYawLeftName, kLockYawRightName, kEnhancedLeftName,
                            kEnhancedRightName};
    for (const char* name : others)
    {
        QWidget* widget = child(m_widget.data(), name);
        QVERIFY2(widget != nullptr, name);
        QVERIFY2(widget->isHidden(), name);
    }

    m_archive.capture(QStringLiteral("startup-free-mode"), m_widget.data(), freeLeft);
}

/**
 * @test   h2WidgetMocapModePanelUiTest::switchingModeWithoutDeviceStillSwitchesPanels
 * @brief  未绑定设备（m_sn == 0）时点击「分指锁定」，面板也要切换过去。
 * @details 目的：早期实现里 onGloveCaptureModeChanged() 开头就是 `if (m_sn == 0) return;`，
 *          没接手套时点模式按钮毫无反应，用户会以为程序卡死。
 *          设计思路：直接点单选按钮（与用户操作同一条信号路径），
 *          断言分指锁定可见、自由模式隐藏。
 * @expected 点击后分指锁定 LH/RH 可见，自由模式 LH/RH 隐藏
 * @priority P1
 */
void h2WidgetMocapModePanelUiTest::switchingModeWithoutDeviceStillSwitchesPanels()
{
    QRadioButton* lockRadio =
        m_widget->findChild<QRadioButton*>(QLatin1String(kLockModeRadioName));
    QVERIFY2(lockRadio != nullptr, "缺少 RB_lockMode 单选按钮");

    lockRadio->click();

    QWidget* lockLeft = child(m_widget.data(), kLockYawLeftName);
    QWidget* lockRight = child(m_widget.data(), kLockYawRightName);
    QWidget* freeLeft = child(m_widget.data(), kFreeModeLeftName);
    QVERIFY2(lockLeft != nullptr && lockRight != nullptr && freeLeft != nullptr, "缺少面板控件");

    QVERIFY2(!lockLeft->isHidden(), "点击分指锁定后其左手段没有显示");
    QVERIFY2(!lockRight->isHidden(), "点击分指锁定后其右手没有显示");
    QVERIFY2(freeLeft->isHidden(), "切换到分指锁定后自由模式仍可见");

    m_archive.capture(QStringLiteral("switch-to-lock-yaw"), m_widget.data(), lockLeft);
}

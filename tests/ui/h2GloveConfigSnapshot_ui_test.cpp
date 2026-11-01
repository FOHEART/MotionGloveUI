/**
 * @file  h2GloveConfigSnapshot_ui_test.cpp
 * @brief 界面值 → 套装配置结构体收集用例的实现。
 * @ingroup h2tests
 *
 * @details 用例意图写在每个槽函数上方的 Doxygen 块里；断言口径见函数体注释。
 * 浮点断言统一走本文件里的 isNear()：QtTest 的 QCOMPARE 对 float 只做精确比较，
 * 直接比会因 float 字面量精度差异偶发失败。
 */

#include "h2GloveConfigSnapshot_ui_test.h"

#include "h2SuitMgr.h"
#include "h2Widget.h"

#include <QRadioButton>
#include <QSlider>
#include <QWidget>

#include <QtTest/QtTest>

namespace
{

/// 六个模式面板的 objectName，见 ui/h2Widget.cpp 的 initUI()。
const char kFreeModeLeft[] = "WIG_fingerAdjustFreeMode_LH";
const char kFreeModeRight[] = "WIG_fingerAdjustFreeMode_RH";
const char kLockYawLeft[] = "WIG_fingerAdjustLockYaw_LH";
const char kLockYawRight[] = "WIG_fingerAdjustLockYaw_RH";
const char kEnhancedLeft[] = "WIG_fingerAdjustEnhanced_LH";
const char kEnhancedRight[] = "WIG_fingerAdjustEnhanced_RH";

/// 面板内滑杆的 objectName，见各面板的 .ui 文件。
const char kThumbAngleX[] = "HS_thumbFigureAngleX";
const char kThumbAngleY[] = "HS_thumbFigureAngleY";
const char kThumbAngleZ[] = "HS_thumbFigureAngleZ";
const char kStretch[] = "HS_figureAngleStretch";
const char kFist[] = "HS_figureAngleFist";

/// 采集模式单选按钮的 objectName，见 ui/h2Widget.ui。
const char kLockModeRadio[] = "RB_lockMode";
const char kMixModeRadio[] = "RB_mixMode";

/// 浮点比较容差：滑杆是整数刻度，收集后应能原样还原。
const float kTolerance = 0.001f;

/// 判断两个浮点是否足够接近。
bool isNear(float actual, float expected)
{
    const float diff = actual - expected;
    return diff <= kTolerance && diff >= -kTolerance;
}

/// 取面板内的具名滑杆；任一层缺失时返回 nullptr，由调用方给出可读诊断。
QSlider* sliderIn(QWidget* root, const char* panelName, const char* sliderName)
{
    if (!root)
    {
        return nullptr;
    }
    QWidget* panel = root->findChild<QWidget*>(QLatin1String(panelName));
    if (!panel)
    {
        return nullptr;
    }
    return panel->findChild<QSlider*>(QLatin1String(sliderName));
}

/// 设置滑杆值；滑杆不存在时返回 false。
bool setSlider(QWidget* root, const char* panelName, const char* sliderName, int value)
{
    QSlider* slider = sliderIn(root, panelName, sliderName);
    if (!slider)
    {
        return false;
    }
    slider->setValue(value);
    return true;
}

} // namespace

h2GloveConfigSnapshotUiTest::~h2GloveConfigSnapshotUiTest() = default;

void h2GloveConfigSnapshotUiTest::initTestCase()
{
    m_archive.initTestCase();
}

void h2GloveConfigSnapshotUiTest::init()
{
    m_widget.reset(new h2Widget());
    m_widget->resize(960, 720);
    m_widget->show();
    QVERIFY(QTest::qWaitForWindowExposed(m_widget.data()));
}

void h2GloveConfigSnapshotUiTest::cleanup()
{
    if (QTest::currentTestFailed())
    {
        m_archive.captureOnFailure(m_widget.data());
    }
    m_widget.reset();
}

void h2GloveConfigSnapshotUiTest::cleanupTestCase()
{
    m_archive.finalize();
}

/**
 * @test   h2GloveConfigSnapshotUiTest::freeModeThumbAnglesFollowTheirHand
 * @brief  自由模式的拇指角度必须按左右手落到各自的关节索引上。
 * @details 目的：收集函数若把左右手搞反或只写一只手，手套下线时的保存就会
 *          覆盖掉另一只手的配置。设计思路：左右两块面板设不同数值，
 *          断言两份数据互不干扰，且 X/Y/Z 三个分量各就各位。
 * @expected 左手 {5,12,-30} 落到 KHS32_LeftHandThumb1，右手 {-4,-6,18} 落到 KHS32_RightHandThumb1
 * @priority P0
 */
void h2GloveConfigSnapshotUiTest::freeModeThumbAnglesFollowTheirHand()
{
    QVERIFY2(setSlider(m_widget.data(), kFreeModeLeft, kThumbAngleX, 5), "缺少左手拇指 X 滑杆");
    QVERIFY2(setSlider(m_widget.data(), kFreeModeLeft, kThumbAngleY, 12), "缺少左手拇指 Y 滑杆");
    QVERIFY2(setSlider(m_widget.data(), kFreeModeLeft, kThumbAngleZ, -30), "缺少左手拇指 Z 滑杆");
    QVERIFY2(setSlider(m_widget.data(), kFreeModeRight, kThumbAngleX, -4), "缺少右手拇指 X 滑杆");
    QVERIFY2(setSlider(m_widget.data(), kFreeModeRight, kThumbAngleY, -6), "缺少右手拇指 Y 滑杆");
    QVERIFY2(setSlider(m_widget.data(), kFreeModeRight, kThumbAngleZ, 18), "缺少右手拇指 Z 滑杆");

    const h2GloveMgr suit = m_widget->collectGloveConfigFromUI();

    QVERIFY2(isNear(suit.angleAdditionEuler[KHS32_LeftHandThumb1][0], 5.0f), "左手拇指 X 未收集");
    QVERIFY2(isNear(suit.angleAdditionEuler[KHS32_LeftHandThumb1][1], 12.0f), "左手拇指 Y 未收集");
    QVERIFY2(isNear(suit.angleAdditionEuler[KHS32_LeftHandThumb1][2], -30.0f), "左手拇指 Z 未收集");
    QVERIFY2(isNear(suit.angleAdditionEuler[KHS32_RightHandThumb1][0], -4.0f), "右手拇指 X 未收集");
    QVERIFY2(isNear(suit.angleAdditionEuler[KHS32_RightHandThumb1][1], -6.0f), "右手拇指 Y 未收集");
    QVERIFY2(isNear(suit.angleAdditionEuler[KHS32_RightHandThumb1][2], 18.0f), "右手拇指 Z 未收集");

    m_archive.capture(QStringLiteral("free-mode-collect"), m_widget.data(),
                      m_widget->findChild<QWidget*>(QLatin1String(kFreeModeLeft)));
}

/**
 * @test   h2GloveConfigSnapshotUiTest::lockYawStretchAndFistFollowTheirHand
 * @brief  分指锁定的「分指张开 / 握拳」角度按左右手分别收集。
 * @details 目的：这两个字段是分指锁定模式独有的，收集时最容易漏掉或写错左右。
 *          设计思路：先点「分指锁定」把模式切过去（收集只认当前模式那组面板，
 *          三组面板的拇指滑杆共用同一个字段），左右手给不同值，四个字段逐一断言。
 * @expected 左手 stretch=12、fist=-8；右手 stretch=20、fist=15
 * @priority P0
 */
void h2GloveConfigSnapshotUiTest::lockYawStretchAndFistFollowTheirHand()
{
    QRadioButton* lockRadio =
        m_widget->findChild<QRadioButton*>(QLatin1String(kLockModeRadio));
    QVERIFY2(lockRadio != nullptr, "缺少 RB_lockMode 单选按钮");
    lockRadio->click();

    QVERIFY2(setSlider(m_widget.data(), kLockYawLeft, kStretch, 12), "缺少左手分指张开滑杆");
    QVERIFY2(setSlider(m_widget.data(), kLockYawLeft, kFist, -8), "缺少左手握拳滑杆");
    QVERIFY2(setSlider(m_widget.data(), kLockYawRight, kStretch, 20), "缺少右手分指张开滑杆");
    QVERIFY2(setSlider(m_widget.data(), kLockYawRight, kFist, 15), "缺少右手握拳滑杆");

    const h2GloveMgr suit = m_widget->collectGloveConfigFromUI();

    QVERIFY2(isNear(suit.LeftFingerAngleStretch, 12.0f), "左手分指张开未收集");
    QVERIFY2(isNear(suit.LeftFingerAngleFist, -8.0f), "左手握拳角度未收集");
    QVERIFY2(isNear(suit.RightFingerAngleStretch, 20.0f), "右手分指张开未收集");
    QVERIFY2(isNear(suit.RightFingerAngleFist, 15.0f), "右手握拳角度未收集");

    m_archive.capture(QStringLiteral("lock-yaw-collect"), m_widget.data(),
                      m_widget->findChild<QWidget*>(QLatin1String(kLockYawLeft)));
}

/**
 * @test   h2GloveConfigSnapshotUiTest::enhancedStretchLandsInEnhancedField
 * @brief  混合模式的滑杆写进 LeftFingerEnhancedModeFistAngle / RightFingerEnhancedModeFistAngle。
 * @details 目的：混合模式的滑杆与分指锁定的同名（HS_figureAngleStretch），
 *          但落盘字段是增强模式专用的，写错字段会让两种模式互相覆盖。
 *          设计思路：先点「混合模式」切过去，再断言增强专用字段拿到新值、
 *          分指锁定字段仍是默认的 5（未被本轮收集碰过）。
 * @expected 左手 9、右手 21 落到增强模式字段；分指锁定字段保持默认 5
 * @priority P0
 */
void h2GloveConfigSnapshotUiTest::enhancedStretchLandsInEnhancedField()
{
    QRadioButton* mixRadio = m_widget->findChild<QRadioButton*>(QLatin1String(kMixModeRadio));
    QVERIFY2(mixRadio != nullptr, "缺少 RB_mixMode 单选按钮");
    mixRadio->click();

    QVERIFY2(setSlider(m_widget.data(), kEnhancedLeft, kStretch, 9), "缺少左手增强模式滑杆");
    QVERIFY2(setSlider(m_widget.data(), kEnhancedRight, kStretch, 21), "缺少右手增强模式滑杆");

    const h2GloveMgr suit = m_widget->collectGloveConfigFromUI();

    QVERIFY2(isNear(suit.LeftFingerEnhancedModeFistAngle, 9.0f), "左手增强模式角度未收集");
    QVERIFY2(isNear(suit.RightFingerEnhancedModeFistAngle, 21.0f), "右手增强模式角度未收集");
    QVERIFY2(isNear(suit.LeftFingerAngleStretch, 5.0f), "增强模式滑杆不应写进分指锁定字段");
    QVERIFY2(isNear(suit.RightFingerAngleStretch, 5.0f), "增强模式滑杆不应写进分指锁定字段");

    m_archive.capture(QStringLiteral("enhanced-collect"), m_widget.data(),
                      m_widget->findChild<QWidget*>(QLatin1String(kEnhancedLeft)));
}

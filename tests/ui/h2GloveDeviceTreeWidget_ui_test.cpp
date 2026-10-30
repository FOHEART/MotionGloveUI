/**
 * @file  h2GloveDeviceTreeWidget_ui_test.cpp
 * @brief 设备卡片控件的 QtTest 界面用例实现。
 * @ingroup h2tests
 *
 * @details 用例意图写在每个槽函数上方的 Doxygen 块里；
 *          断言口径见函数体注释。
 */

#include "h2GloveDeviceTreeWidget_ui_test.h"

#include "h2GloveDeviceTreeWidget.h"

#include <QPoint>
#include <QWidget>

#include <QtTest/QtTest>

#include <string>

h2GloveDeviceTreeWidgetUiTest::~h2GloveDeviceTreeWidgetUiTest() = default;

void h2GloveDeviceTreeWidgetUiTest::initTestCase()
{
    m_archive.initTestCase();
}

void h2GloveDeviceTreeWidgetUiTest::init()
{
    m_card.reset(new h2GloveDeviceTreeWidget());
    m_card->resize(320, 96);
    m_card->show();
    QVERIFY(QTest::qWaitForWindowExposed(m_card.data()));
}

void h2GloveDeviceTreeWidgetUiTest::cleanup()
{
    if (QTest::currentTestFailed())
    {
        m_archive.captureOnFailure(m_card.data());
    }
    m_card.reset();
}

void h2GloveDeviceTreeWidgetUiTest::cleanupTestCase()
{
    m_archive.finalize();
}

void h2GloveDeviceTreeWidgetUiTest::deviceCardState_data()
{
    QTest::addColumn<bool>("leftOnline");
    QTest::addColumn<bool>("rightOnline");
    QTest::addColumn<int>("leftBattery");
    QTest::addColumn<int>("rightBattery");
    QTest::addColumn<bool>("selected");

    QTest::newRow("both-online-full") << true << true << 100 << 100 << false;
    QTest::newRow("left-online-low-battery") << true << false << 5 << 0 << false;
    QTest::newRow("both-offline") << false << false << 0 << 0 << false;
    QTest::newRow("selected-card") << true << true << 50 << 50 << true;
}

/**
 * @test   h2GloveDeviceTreeWidgetUiTest::deviceCardState
 * @brief  状态设置：在线状态、电量、选中态写入后，读取值与写入值一致。
 * @details 目的：卡片是纯展示控件，唯一契约就是「设进去什么就读出什么」；
 *          一旦某个 setter 漏写成员或写错字段（例如把左右手电量互换），
 *          界面会长期显示错误数据而没有任何报错。
 *          设计思路：用等价类划分覆盖「双在线满电 / 单在线低电 / 双离线 / 选中」，
 *          每组都做一次整窗截图，作为视觉回归的人工核对材料。
 * @expected 四个 getter 全部返回 set 进去的值
 * @priority P0
 */
void h2GloveDeviceTreeWidgetUiTest::deviceCardState()
{
    QFETCH(bool, leftOnline);
    QFETCH(bool, rightOnline);
    QFETCH(int, leftBattery);
    QFETCH(int, rightBattery);
    QFETCH(bool, selected);

    // Arrange
    m_card->setDeviceName(QStringLiteral("MotionGlove 3E"));
    m_card->setSerialNumber(0x3E0001u);
    m_card->setLeftOnline(leftOnline);
    m_card->setRightOnline(rightOnline);
    m_card->setLeftBattery(leftBattery);
    m_card->setRightBattery(rightBattery);
    m_card->setSelected(selected);

    // Act 之外的读取
    QCOMPARE(m_card->deviceName(), QStringLiteral("MotionGlove 3E"));
    QCOMPARE(m_card->serialNumber(), 0x3E0001u);
    QCOMPARE(m_card->isLeftOnline(), leftOnline);
    QCOMPARE(m_card->isRightOnline(), rightOnline);
    QCOMPARE(m_card->leftBattery(), leftBattery);
    QCOMPARE(m_card->rightBattery(), rightBattery);
    QCOMPARE(m_card->isSelected(), selected);

    // Assert 后的证据归档
    m_archive.capture(QString::fromLatin1(QTest::currentDataTag()), m_card.data());
}

/**
 * @test   h2GloveDeviceTreeWidgetUiTest::leftClickEmitsSuitSelectedLive
 * @brief  鼠标交互：左键点击卡片发出一次 suitSelectedLive，携带序列号与设备名。
 * @details 目的：选中设备是整条链路（列表 → 主界面 → SDK）的起点，
 *          信号一次都不发或发两次都会导致界面无响应或重复下发配置。
 *          设计思路：用 lambda 计数替代 QSignalSpy —— 信号第二参数是
 *          std::string，未注册的元类型会让 QSignalSpy 直接报错；
 *          同时校验信号里的序列号与名称，防止「点 A 传 B」的错位。
 *          点击目标取卡片内部的 `widget` 子控件，事件过滤器就装在那里。
 * @expected 信号计数为 1，序列号为 0x3E0001，名称为 "MotionGlove 3E"
 * @priority P0
 */
void h2GloveDeviceTreeWidgetUiTest::leftClickEmitsSuitSelectedLive()
{
    m_card->setSerialNumber(0x3E0001u);
    m_card->setDeviceName(QStringLiteral("MotionGlove 3E"));

    int calls = 0;
    unsigned int gotSn = 0;
    QString gotName;
    const QMetaObject::Connection conn = connect(
        m_card.data(), &h2GloveDeviceTreeWidget::suitSelectedLive,
        [&calls, &gotSn, &gotName](const unsigned int& sn, const std::string& name)
        {
            ++calls;
            gotSn = sn;
            gotName = QString::fromStdString(name);
        });

    QWidget* hitArea = m_card->findChild<QWidget*>(QStringLiteral("h2GloveFrame"));
    QVERIFY2(hitArea != nullptr, "卡片内部缺少名为 h2GloveFrame 的点击区域，事件过滤器将失效");

    QTest::mouseClick(hitArea, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));

    QCOMPARE(calls, 1);
    QCOMPARE(gotSn, 0x3E0001u);
    QCOMPARE(gotName, QStringLiteral("MotionGlove 3E"));

    m_archive.capture(QStringLiteral("left-click"), m_card.data());
    disconnect(conn);
}

/**
 * @test   h2GloveDeviceTreeWidgetUiTest::rightClickIsIgnored
 * @brief  鼠标交互：右键点击不发出选中信号。
 * @details 目的：右键通常留给上下文菜单，若也发选中信号，
 *          用户想删设备时会先切走当前选中项，体验与数据都可能被动改变。
 *          设计思路：与左键用例成对，作为「只有左键有效」的对照。
 * @expected 信号计数保持 0
 * @priority P1
 */
void h2GloveDeviceTreeWidgetUiTest::rightClickIsIgnored()
{
    int calls = 0;
    const QMetaObject::Connection conn = connect(
        m_card.data(), &h2GloveDeviceTreeWidget::suitSelectedLive,
        [&calls](const unsigned int&, const std::string&) { ++calls; });

    QWidget* hitArea = m_card->findChild<QWidget*>(QStringLiteral("h2GloveFrame"));
    QVERIFY(hitArea != nullptr);

    QTest::mouseClick(hitArea, Qt::RightButton, Qt::NoModifier, QPoint(5, 5));

    QCOMPARE(calls, 0);

    m_archive.capture(QStringLiteral("right-click"), m_card.data());
    disconnect(conn);
}

/**
 * @file  h2GloveDeviceTreeContainerWidget_ui_test.cpp
 * @brief 设备树容器控件的 QtTest 用例实现。
 * @ingroup h2tests
 *
 * @details 用例意图写在每个槽函数上方的 Doxygen 块里。
 */

#include "h2GloveDeviceTreeContainerWidget_ui_test.h"

#include "h2GloveDeviceTreeContainerWidget.h"
#include "h2GloveDeviceTreeWidget.h"

#include <QPoint>
#include <QWidget>

#include <QCoreApplication>
#include <QEvent>

#include <QtTest/QtTest>

h2GloveDeviceTreeContainerWidgetUiTest::~h2GloveDeviceTreeContainerWidgetUiTest() = default;

void h2GloveDeviceTreeContainerWidgetUiTest::initTestCase()
{
    m_archive.initTestCase();
}

void h2GloveDeviceTreeContainerWidgetUiTest::init()
{
    m_container.reset(new h2GloveDeviceTreeContainerWidget());
    m_container->resize(320, 360);
    m_container->show();
    QVERIFY(QTest::qWaitForWindowExposed(m_container.data()));
}

void h2GloveDeviceTreeContainerWidgetUiTest::cleanup()
{
    if (QTest::currentTestFailed())
    {
        m_archive.captureOnFailure(m_container.data());
    }
    m_container.reset();
}

void h2GloveDeviceTreeContainerWidgetUiTest::cleanupTestCase()
{
    m_archive.finalize();
}

h2GloveDeviceTreeWidget* h2GloveDeviceTreeContainerWidgetUiTest::addCard(unsigned int sn, const QString& name)
{
    h2GloveDeviceTreeWidget* card = new h2GloveDeviceTreeWidget();
    card->setSerialNumber(sn);
    card->setDeviceName(name);
    card->resize(300, 96);
    m_container->addDeviceTreeWidget(card);
    return card;
}

/**
 * @test   h2GloveDeviceTreeContainerWidgetUiTest::countIgnoresLayoutSpacer
 * @brief  计数契约：加入 N 张卡片后 deviceTreeWidgetCount() 返回 N。
 * @details 目的：该接口的语义是「托管的卡片数量」，调用方用它判断列表是否为空
 *          （例如决定显示空列表提示）。若把布局末尾用于顶部的弹簧项也计入，
 *          空列表会返回 1，界面永远不会显示「暂无设备」。
 *          设计思路：先断言空容器为 0，再依次加到 2，逐步逼近边界。
 * @expected 空容器为 0；加入 1 张为 1；加入 2 张为 2
 * @priority P0
 */
void h2GloveDeviceTreeContainerWidgetUiTest::countIgnoresLayoutSpacer()
{
    QCOMPARE(m_container->deviceTreeWidgetCount(), 0);

    addCard(0x3E0001u, QStringLiteral("Glove A"));
    QCOMPARE(m_container->deviceTreeWidgetCount(), 1);
    m_archive.capture(QStringLiteral("count-1"), m_container.data());

    addCard(0x3E0002u, QStringLiteral("Glove B"));
    QCOMPARE(m_container->deviceTreeWidgetCount(), 2);
    m_archive.capture(QStringLiteral("count-2"), m_container.data());
}

/**
 * @test   h2GloveDeviceTreeContainerWidgetUiTest::leftClickSelectsExactlyOneCard
 * @brief  交互：点击某张卡片后，全容器内只有它处于选中态。
 * @details 目的：设备列表是单选语义——同时选中两张会让「当前设备」
 *          产生歧义，后续所有配置下发都可能发到错误的手套上。
 *          设计思路：先点 B 再点 A，两个方向都验证，避免实现只在
 *          「首次选中」时生效；每步都截图，便于人工核对高亮样式。
 * @expected 点 B 后 B 选中且 A 未选中；点 A 后 A 选中且 B 未选中
 * @priority P0
 */
void h2GloveDeviceTreeContainerWidgetUiTest::leftClickSelectsExactlyOneCard()
{
    h2GloveDeviceTreeWidget* cardA = addCard(0x3E0001u, QStringLiteral("Glove A"));
    h2GloveDeviceTreeWidget* cardB = addCard(0x3E0002u, QStringLiteral("Glove B"));

    QWidget* hitB = cardB->findChild<QWidget*>(QStringLiteral("h2GloveFrame"));
    QVERIFY2(hitB != nullptr, "卡片内部缺少名为 h2GloveFrame 的点击区域");

    QTest::mouseClick(hitB, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));

    QVERIFY2(cardB->isSelected(), "点击卡片 B 后 B 未进入选中态");
    QVERIFY2(!cardA->isSelected(), "点击卡片 B 后 A 仍处于选中态，单选互斥失效");
    m_archive.capture(QStringLiteral("select-b"), m_container.data());

    QWidget* hitA = cardA->findChild<QWidget*>(QStringLiteral("h2GloveFrame"));
    QVERIFY2(hitA != nullptr, "卡片内部缺少名为 h2GloveFrame 的点击区域");

    QTest::mouseClick(hitA, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));

    QVERIFY2(cardA->isSelected(), "点击卡片 A 后 A 未进入选中态");
    QVERIFY2(!cardB->isSelected(), "点击卡片 A 后 B 仍处于选中态，单选互斥失效");
    m_archive.capture(QStringLiteral("select-a"), m_container.data());
}

/**
 * @test   h2GloveDeviceTreeContainerWidgetUiTest::removeCardUpdatesCountAndSelection
 * @brief  移除：删除卡片后计数递减，且不会残留指向已销毁卡片的选中状态。
 * @details 目的：设备下线时卡片被移除，若选中指针没清干净，
 *          后续刷新会对野指针调用 setSelected() 而崩溃。
 *          设计思路：先选中 B 再移除 B，随后刷新 A 的选中态应当正常
 *          （说明内部状态已回到「无选中」）；再点 A 应能重新选中。
 * @expected 移除后计数为 1；A 未被选中；点 A 后 A 选中
 * @priority P0
 */
void h2GloveDeviceTreeContainerWidgetUiTest::removeCardUpdatesCountAndSelection()
{
    h2GloveDeviceTreeWidget* cardA = addCard(0x3E0001u, QStringLiteral("Glove A"));
    h2GloveDeviceTreeWidget* cardB = addCard(0x3E0002u, QStringLiteral("Glove B"));

    QWidget* hitB = cardB->findChild<QWidget*>(QStringLiteral("h2GloveFrame"));
    QVERIFY(hitB != nullptr);
    QTest::mouseClick(hitB, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(cardB->isSelected());

    QVERIFY(m_container->removeDeviceTreeWidget(cardB));
    QCOMPARE(m_container->deviceTreeWidgetCount(), 1);
    QVERIFY2(!cardA->isSelected(), "移除选中卡片后，其他卡片不应继承选中态");

    QWidget* hitA = cardA->findChild<QWidget*>(QStringLiteral("h2GloveFrame"));
    QVERIFY(hitA != nullptr);
    QTest::mouseClick(hitA, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY2(cardA->isSelected(), "移除选中项后再点其他卡片应当能正常选中");

    m_archive.capture(QStringLiteral("after-remove"), m_container.data());
}

/**
 * @test   h2GloveDeviceTreeContainerWidgetUiTest::clearRemovesAllCards
 * @brief  清空：clearDeviceTreeWidgets() 后计数归零且不残留卡片对象。
 * @details 目的：切换设备列表或重新扫描时会先清空；若卡片对象没被真正删除，
 *          内存会随扫描次数线性增长，且旧卡片仍可能响应点击。
 *          设计思路：加 3 张后清空，既查计数也查子对象数量，
 *          两者不一致就说明有对象脱离布局但仍存活。
 * @expected 计数为 0，且 findChildren 找不到任何卡片
 * @priority P1
 */
void h2GloveDeviceTreeContainerWidgetUiTest::clearRemovesAllCards()
{
    addCard(0x3E0001u, QStringLiteral("Glove A"));
    addCard(0x3E0002u, QStringLiteral("Glove B"));
    addCard(0x3E0003u, QStringLiteral("Glove C"));
    QCOMPARE(m_container->deviceTreeWidgetCount(), 3);

    m_container->clearDeviceTreeWidgets();

    QCOMPARE(m_container->deviceTreeWidgetCount(), 0);

    // clearDeviceTreeWidgets() 用 deleteLater() 延迟销毁（避免在信号处理中直接析构），
    // 必须先把 DeferredDelete 事件排空，否则 findChildren 仍能拿到待销毁的卡片，
    // 断言会因为「对象还在」而假红。
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

    QCOMPARE(m_container->findChildren<h2GloveDeviceTreeWidget*>().size(), 0);
    m_archive.capture(QStringLiteral("after-clear"), m_container.data());
}

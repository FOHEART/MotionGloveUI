/**
 * @file  h2SettingsDialog.cpp
 * @brief 设置对话框的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；控件样式统一由 CarbonUIClass 提供；依赖 uic 生成的 ui_*.h。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

#include "../include/h2SettingsDialog.h"
#include "ui_h2SettingsDialog.h"
#include "h2NetBroadcastWidget.h"
#include "../include/h2StreamConfigWidget.h"
#include "CarbonUIQTabWidget.h"
#include "CarbonUIQPushButton.h"
#include "groupboxcollapser.h"
#include <QtCore/QMetaObject>
#include <QtCore/QMetaMethod>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>

h2SettingsDialog::h2SettingsDialog(QWidget *parent)
	: CarbonUIDialog(tr("设置 / Settings"), parent)
{
	// 设置弹窗统一使用 CarbonUIDialog 外壳：原 .ui 的页签与按钮区装进外壳 body。
	auto* body = findChild<QWidget*>(QStringLiteral("CarbonUIDialogBody"));
	auto* content = new QWidget(body);
	ui = new Ui::h2SettingsDialog();
	ui->setupUi(content);
	if (auto* bodyLayout = qobject_cast<QVBoxLayout*>(body->layout()))
	{
		bodyLayout->setContentsMargins(0, 0, 0, 0);
		bodyLayout->setSpacing(0);
		bodyLayout->addWidget(content, 1);
	}
	// 主控件铺满页面：去掉 .ui 根布局的上下左右空白。
	if (auto* gridLayout = qobject_cast<QGridLayout*>(content->layout()))
	{
		gridLayout->setContentsMargins(0, 0, 0, 0);
		gridLayout->setSpacing(0);
	}
	// 滚动条统一使用 CarbonUI 样式（CarbonUIQScrollArea::applyStyle 同款：
	// 4px 宽、透明槽、#cdcdcd 圆角滑块、无箭头/翻页），覆盖页签内所有 QScrollBar。
	content->setStyleSheet(QStringLiteral(
		"QScrollBar:vertical { width: 4px; background: transparent; margin: 0px; }"
		"QScrollBar::handle:vertical { background: #cdcdcd; min-height: 20px; border-radius: 4px; }"
		"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
		"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }"
		"QScrollBar:horizontal { height: 4px; background: transparent; margin: 0px; }"
		"QScrollBar::handle:horizontal { background: #cdcdcd; min-width: 20px; border-radius: 4px; }"
		"QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
		"QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: none; }"));
	resize(560, 720);

	CarbonUIQTabWidget::applyStyle(ui->tabWidget);
	CarbonUIQPushButton::applyStyle(ui->PB_OK, CarbonUIQPushButton::Shape_RoundAll);
	CarbonUIQPushButton::applyStyle(ui->PB_Cancle, CarbonUIQPushButton::Shape_RoundAll);


	{
		m_netBroadcastWidget = new h2NetBroadcastWidget(this);
		QWidget *tabWidget = new QWidget();
		QVBoxLayout *vLayout = new QVBoxLayout(tabWidget);
		vLayout->setContentsMargins(0, 0, 0, 0);
		vLayout->setSpacing(0);
		vLayout->addWidget(m_netBroadcastWidget);
		ui->tabWidget->addTab(tabWidget, tr("Data Forwarding / 数据转发"));

	}
	{
		m_streamFormatWidget = new h2StreamConfigWidget(this);
		QWidget *tabWidget = new QWidget();
		QVBoxLayout *vLayout = new QVBoxLayout(tabWidget);
		vLayout->setContentsMargins(0, 0, 0, 0);
		vLayout->setSpacing(0);
		vLayout->addWidget(m_streamFormatWidget);
		ui->tabWidget->addTab(tabWidget, tr("Data Format / 转发格式"));
	}
	
	// Setup language change connections for child widgets
	// Setup language change connections for child widgets
	h2NetBroadcastWidget *broadcastWidget = qobject_cast<h2NetBroadcastWidget*>(m_netBroadcastWidget);
	if (broadcastWidget)
	{
		broadcastWidget->setupLanguageChangeConnection();
	}
	
	// Setup language change connection for this dialog
	const QMetaObject *metaObj = QCoreApplication::instance()->metaObject();
	for (int i = 0; i < QCoreApplication::instance()->children().size(); ++i)
	{
		QObject *obj = QCoreApplication::instance()->children()[i];
		if (obj && obj->inherits("LanguageManager"))
		{
			connect(obj, SIGNAL(languageChanged(QString)), this, SLOT(retranslateUI()));
			break;
		}
	}
	
	// Apply current language translations on initialization
	retranslateUI();
}

h2SettingsDialog::~h2SettingsDialog()
{
	delete ui;
	ui = nullptr;
}

void h2SettingsDialog::closeEvent(QCloseEvent *event)
{
	if (m_netBroadcastWidget)
	{
		delete m_netBroadcastWidget;
		m_netBroadcastWidget = 0;
	}

	if (m_streamFormatWidget)
	{
		delete m_streamFormatWidget;
		m_streamFormatWidget = 0;

	}
	event->accept(); // 接受关闭事件
}

void h2SettingsDialog::on_PB_Cancle_clicked()
{
	close();
}

void h2SettingsDialog::on_PB_OK_clicked()
{
	((h2NetBroadcastWidget*)m_netBroadcastWidget)->writeH2StreamConfig();
	((h2StreamConfigWidget*)m_streamFormatWidget)->writeH2StreamConfig();
	close();
}

void h2SettingsDialog::retranslateUI()
{
	// CarbonUIDialog 自带标题栏，用 setTitleText 更新标题
	if (this) {
		setTitleText(tr("设置 / Settings"));
	}
	
	// Update button texts with current language
	if (ui) {
		ui->PB_OK->setText(tr("确定 / OK"));
		ui->PB_Cancle->setText(tr("取消 / Cancel"));
	}
	
	// Update tab labels with current language
	if (ui && ui->tabWidget) {
		ui->tabWidget->setTabText(0, tr("Data Forwarding / 数据转发"));
		ui->tabWidget->setTabText(1, tr("Data Format / 转发格式"));
	}
	
	// Call retranslateUI on child widgets to update their texts
	if (m_netBroadcastWidget) {
		// Cast to h2NetBroadcastWidget and call retranslateUI()
		h2NetBroadcastWidget *broadcastWidget = static_cast<h2NetBroadcastWidget*>(m_netBroadcastWidget);
		if (broadcastWidget) {
			broadcastWidget->retranslateUI();
		}
	}
	
	// For h2StreamConfigWidget, try to call retranslateUI if it exists
	if (m_streamFormatWidget) {
		// Use QMetaObject to check if retranslateUI method exists
		const QMetaObject *metaObj = m_streamFormatWidget->metaObject();
		int methodIndex = metaObj->indexOfMethod("retranslateUI()");
		if (methodIndex >= 0) {
			QMetaMethod method = metaObj->method(methodIndex);
			method.invoke(m_streamFormatWidget, Qt::DirectConnection);
		}
	}
}

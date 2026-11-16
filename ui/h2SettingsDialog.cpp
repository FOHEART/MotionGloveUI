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

h2SettingsDialog::h2SettingsDialog(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::h2SettingsDialog();
	ui->setupUi(this);

	setStyleSheet("background-color: #3b3b3b; ");

	CarbonUIQTabWidget::applyStyle(ui->tabWidget);
	CarbonUIQPushButton::applyStyle(ui->PB_OK, CarbonUIQPushButton::Shape_RoundAll);
	CarbonUIQPushButton::applyStyle(ui->PB_Cancle, CarbonUIQPushButton::Shape_RoundAll);


	{
		m_netBroadcastWidget = new h2NetBroadcastWidget(this);
		QWidget *tabWidget = new QWidget(); 
		QVBoxLayout *vLayout = new QVBoxLayout(tabWidget); 
		vLayout->addWidget(m_netBroadcastWidget);
		ui->tabWidget->addTab(tabWidget, tr("Data Forwarding / 数据转发"));

	}
	{
		m_streamFormatWidget = new h2StreamConfigWidget(this);
		QWidget *tabWidget = new QWidget(); 
		QVBoxLayout *vLayout = new QVBoxLayout(tabWidget); 
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
	// Update window title with current language
	if (this) {
		setWindowTitle(tr("设置 / Settings"));
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

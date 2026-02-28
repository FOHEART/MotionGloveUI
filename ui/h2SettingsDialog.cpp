#include "../include/h2SettingsDialog.h"
#include "ui_h2SettingsDialog.h"
#include "h2NetBroadcastWidget.h"
#include "../include/h2StreamConfigWidget.h"
#include "CarbonUIClass.h"
#include "groupboxcollapser.h"

h2SettingsDialog::h2SettingsDialog(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::h2SettingsDialog();
	ui->setupUi(this);

	setStyleSheet("background-color: #3b3b3b; ");

	CarbonUIClass::setupTabWidgetStyle(ui->tabWidget);
	CarbonUIClass::applyButtonStyle(ui->PB_OK, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyButtonStyle(ui->PB_Cancle, CarbonUIClass::Shape_RoundAll);


	{
		m_netBroadcastWidget = new h2NetBroadcastWidget(this);
		QWidget *tabWidget = new QWidget(); 
		QVBoxLayout *vLayout = new QVBoxLayout(tabWidget); 
		vLayout->addWidget(m_netBroadcastWidget);
		ui->tabWidget->addTab(tabWidget, QString::fromLocal8Bit("数据转发"));

	}
	{
		m_streamFormatWidget = new h2StreamConfigWidget(this);
		QWidget *tabWidget = new QWidget(); 
		QVBoxLayout *vLayout = new QVBoxLayout(tabWidget); 
		vLayout->addWidget(m_streamFormatWidget);
		ui->tabWidget->addTab(tabWidget, QString::fromLocal8Bit("转发格式"));
	}
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

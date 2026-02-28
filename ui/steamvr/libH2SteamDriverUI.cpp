
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "../../include/libH2SteamDriverUI.h"
#include "ui_libH2SteamDriverUI.h"
#include <QtWidgets/QMessageBox>
#include <QtGui/QDesktopServices>
#ifdef _WIN32
#include "libH2SteamDriver.h"

libH2SteamDriverUI::libH2SteamDriverUI(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::libH2SteamDriverUI();
	ui->setupUi(this);
	lbl_green_styleSheet = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(0) + "," +
		QString::number(255) + "," +
		QString::number(0) + ");}";

	lbl_red_styleSheet = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(255) + "," +
		QString::number(0) + "," +
		QString::number(0) + ");}";

	checkInstallation();
	QObject::connect(ui->CBX_steamVRDriverEnable, SIGNAL(toggled(bool)), this, SLOT(onCheck_CBX_steamVRDriverEnable(bool)));
#ifdef _WIN32
if (libH2SteamDriver_getSteamVR_GloveDriverEnabled())
	{
		ui->CBX_steamVRDriverEnable->setChecked(true);
	}
	else
	{
		ui->CBX_steamVRDriverEnable->setChecked(false);
	}
#else

#endif
	


}

libH2SteamDriverUI::~libH2SteamDriverUI()
{
	delete ui;
	ui = nullptr;
}

void libH2SteamDriverUI::checkInstallation(void)
{
	ui->LE_steamDir->setEnabled(false);
	ui->LE_steamVRDir->setEnabled(false);
	ui->LBL_steamVRDriverLocalDir->setEnabled(false);

	if (libH2SteamDriver_is_SteamInstalled())
	{
		ui->LBL_steamInstalled->setStyleSheet(lbl_green_styleSheet);
		ui->LBL_steamInstalled->setText(QString::fromUtf8("已安装"));
		ui->PB_installSteamFromWeb->setVisible(false);
	}
	else
	{
		ui->LBL_steamInstalled->setStyleSheet(lbl_red_styleSheet);
		ui->LBL_steamInstalled->setText(QString::fromUtf8("未安装"));
		ui->PB_installSteamFromWeb->setVisible(true);
	}

	/////////
	if (libH2SteamDriver_is_SteamVRInstalled())
	{
		ui->LBL_steamVRInstalled->setStyleSheet(lbl_green_styleSheet);
		ui->LBL_steamVRInstalled->setText(QString::fromUtf8("已安装"));

	}
	else
	{
		ui->LBL_steamVRInstalled->setStyleSheet(lbl_red_styleSheet);
		ui->LBL_steamVRInstalled->setText(QString::fromUtf8("未安装"));

		ui->GBX_SteamVRDriver->setEnabled(false);
	}
	/////////
	if (libH2SteamDriver_is_SteamVRDriverInstalled())
	{
		ui->LBL_steamVRDriverInstalled->setStyleSheet(lbl_green_styleSheet);
		ui->LBL_steamVRDriverInstalled->setText(QString::fromUtf8("已安装"));

		ui->PB_installSteamVRDriverLocal->setEnabled(false);
		ui->PB_uninstallSteamVRDriverLocal->setEnabled(true);

	}
	else
	{
		ui->LBL_steamVRDriverInstalled->setStyleSheet(lbl_red_styleSheet);
		ui->LBL_steamVRDriverInstalled->setText(QString::fromUtf8("未安装"));
	}
	///

	char steamInstallDir[260] = { 0 };
	if (0 == libH2SteamDriver_get_SteamInstallDir(steamInstallDir))
	{
		ui->LE_steamDir->setText(QString::fromLocal8Bit(steamInstallDir));
	}

	//////
	char steamVRInstallDir[260] = { 0 };
	if (0 == libH2SteamDriver_get_SteamVRInstallDir(steamVRInstallDir))
	{
		ui->LE_steamVRDir->setText(QString::fromLocal8Bit(steamVRInstallDir));
	}
	//////////////////////////////////////////////////////////////////////////
	char steamVRDriverLocalDir[260] = { 0 };
	if (0 == libH2SteamDriver_get_SteamVRLocalDriverDir(steamVRDriverLocalDir))
	{
		ui->LBL_steamVRDriverLocalDir->setText(QString::fromLocal8Bit(steamVRDriverLocalDir));
	}

#if 1
	{
		char s[16][64] = {0};
		unsigned int trackerCount = 0;

		libH2SteamDriver_getBindedTracker_LeftHand(&s[0][0],&trackerCount);
		
		for (int i = 0; i < trackerCount; i++)
		{
			ui->PTE_bindedTrackers_LH->insertPlainText(QString::fromLocal8Bit(&s[i][0]) + "\r\n");
		}
		
	}
	{
		char s[16][64] = { 0 };
	 unsigned int trackerCount = 0;

		libH2SteamDriver_getBindedTracker_RightHand(&s[0][0], &trackerCount);

		for (int i = 0; i < trackerCount; i++)
		{
			ui->PTE_bindedTrackers_RH->insertPlainText(QString::fromLocal8Bit(&s[i][0]) + "\r\n");
		}
	}
#endif
}

void libH2SteamDriverUI::on_PB_installSteamVRDriverLocal_clicked()
{
	libH2SteamDriver_install_SteamVRDriver();

	if (libH2SteamDriver_is_SteamVRDriverInstalled())
	{
		ui->LBL_steamVRDriverInstalled->setStyleSheet(lbl_green_styleSheet);
		ui->LBL_steamVRDriverInstalled->setText(QString::fromUtf8("已安装"));

		// 创建 QMessageBox 实例
		QMessageBox msgBox;

		// 设置消息框的图标类型（这里选择 Information）
		msgBox.setIcon(QMessageBox::Information);

		// 设置消息框的标题
		msgBox.setWindowTitle(QObject::tr("安装成功"));

		// 设置消息框的内容文本
		msgBox.setText(QObject::tr("等待SteamVR客户端重新启动..."));

		// 添加一个“确定”按钮
		msgBox.addButton(QMessageBox::Ok);
		msgBox.button(QMessageBox::Ok)->setText(QString::fromUtf8("确定"));

		// 显示消息框并进入事件循环
		msgBox.exec();

		ui->PB_installSteamVRDriverLocal->setEnabled(false);
		ui->PB_uninstallSteamVRDriverLocal->setEnabled(true);

	}
	else
	{
		ui->LBL_steamVRDriverInstalled->setStyleSheet(lbl_red_styleSheet);
		ui->LBL_steamVRDriverInstalled->setText(QString::fromUtf8("未安装"));
	}
}
void libH2SteamDriverUI::on_PB_installSteamFromWeb_clicked()
{
	QDesktopServices::openUrl(QUrl("https://store.steampowered.com/about/"));

}
void libH2SteamDriverUI::on_PB_uninstallSteamVRDriverLocal_clicked()
{
	libH2SteamDriver_uninstall_SteamVRDriver();

	if (!libH2SteamDriver_is_SteamVRDriverInstalled())
	{
		ui->LBL_steamVRDriverInstalled->setStyleSheet(lbl_red_styleSheet);
		ui->LBL_steamVRDriverInstalled->setText(QString::fromUtf8("未安装"));

		// 创建 QMessageBox 实例
		QMessageBox msgBox;

		// 设置消息框的图标类型（这里选择 Information）
		msgBox.setIcon(QMessageBox::Information);

		// 设置消息框的标题
		msgBox.setWindowTitle(QString::fromUtf8("卸载成功"));

		// 设置消息框的内容文本
		msgBox.setText(QString::fromUtf8("等待SteamVR客户端重新启动..."));

		// 添加一个“确定”按钮
		msgBox.addButton(QMessageBox::Ok);
		msgBox.button(QMessageBox::Ok)->setText(QString::fromUtf8("确定"));

		// 显示消息框并进入事件循环
		msgBox.exec();

		ui->PB_installSteamVRDriverLocal->setEnabled(true);
		ui->PB_uninstallSteamVRDriverLocal->setEnabled(false);

	}
	else
	{
		ui->LBL_steamVRDriverInstalled->setStyleSheet(lbl_green_styleSheet);
		ui->LBL_steamVRDriverInstalled->setText(QString::fromUtf8("已安装"));
	}
}

void libH2SteamDriverUI::onCheck_CBX_steamVRDriverEnable(bool bCheck)
{
	bool isEnabled = libH2SteamDriver_getSteamVR_GloveDriverEnabled();
	if(isEnabled!=bCheck)
	libH2SteamDriver_setSteamVR_GloveDriverEnabled(bCheck);

}

#else

#endif

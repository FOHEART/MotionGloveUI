#pragma execution_character_set("utf-8")

#ifdef _WIN32
#include "h2SteamVRMonitorWidget.h"
#include "libH2SteamDriver.h"


h2SteamVRMonitorWidget::h2SteamVRMonitorWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
}

h2SteamVRMonitorWidget::~h2SteamVRMonitorWidget()
{
}

void h2SteamVRMonitorWidget::init()
{

	if (h2MonitorTimer == NULL)
	{
		h2MonitorTimer = new QTimer(this);
		QObject::connect(h2MonitorTimer, SIGNAL(timeout()), this, SLOT(onH2MonitorTimer()));
		h2MonitorTimer->start(1000 / h2MonitorTimerHz);
	}
}

void h2SteamVRMonitorWidget::onH2MonitorTimer()
{
	QString pbStylesheetGreen = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(0) + "," +
		QString::number(255) + "," +
		QString::number(0) + ");}";
	QString pbStylesheetRed = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(255) + "," +
		QString::number(0) + "," +
		QString::number(0) + ");}";
		if (libH2SteamDriver_is_VRCHAT_Running())
		{
			ui.LBL_VRChat->setStyleSheet(pbStylesheetGreen);
			ui.LBL_VRChat->setText(QObject::tr("[运行中]VRChat"));

			ui.PB_start_VRChat->setEnabled(false);
		}
		else
		{
			ui.LBL_VRChat->setStyleSheet(pbStylesheetRed);
			ui.LBL_VRChat->setText(QObject::tr("[未运行]VRChat"));
			ui.PB_start_VRChat->setEnabled(true);

		}

		if (libH2SteamDriver_is_SteamVR_Running())
		{
			ui.LBL_SteamVR->setStyleSheet(pbStylesheetGreen);
			ui.LBL_SteamVR->setText(QObject::tr("[运行中]SteamVR"));

			ui.PB_start_SteamVR->setEnabled(false);
			ui.PB_stop_SteamVR->setEnabled(true);

		}
		else
		{
			ui.LBL_SteamVR->setStyleSheet(pbStylesheetRed);
			ui.LBL_SteamVR->setText(QObject::tr("[未运行]SteamVR"));

			ui.PB_start_SteamVR->setEnabled(true);
			ui.PB_stop_SteamVR->setEnabled(false);

		}
	
}

void h2SteamVRMonitorWidget::on_PB_start_SteamVR_clicked()
{
	libH2SteamDriver_start_SteamVR();
}

void h2SteamVRMonitorWidget::on_PB_start_RebootSteamVR_clicked()
{
	libH2SteamDriver_reboot_SteamVR();
}

void h2SteamVRMonitorWidget::on_PB_stop_SteamVR_clicked()
{
	libH2SteamDriver_stop_SteamVR();
}

void h2SteamVRMonitorWidget::on_PB_start_VRChat_clicked()
{

}
#else

#endif


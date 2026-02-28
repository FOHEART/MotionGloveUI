#pragma once
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>

#ifdef _WIN32
#include "ui_h2SteamVRMonitorWidget.h"

class  h2SteamVRMonitorWidget : public QWidget
{
	Q_OBJECT

public:
	h2SteamVRMonitorWidget(QWidget *parent = Q_NULLPTR);
	~h2SteamVRMonitorWidget();
	void init();

private slots:
	void onH2MonitorTimer();
	
	void on_PB_start_SteamVR_clicked();
	void on_PB_start_RebootSteamVR_clicked();
	void on_PB_stop_SteamVR_clicked();

	void on_PB_start_VRChat_clicked();
private:
	Ui::h2SteamVRMonitorWidget ui;

	QTimer* h2MonitorTimer = nullptr;
	unsigned int h2MonitorTimerHz = 1;
};

#else

#endif

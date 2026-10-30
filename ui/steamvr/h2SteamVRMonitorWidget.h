/**
 * @file  h2SteamVRMonitorWidget.h
 * @brief SteamVR 监控面板控件的声明（仅 Windows 提供）。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

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

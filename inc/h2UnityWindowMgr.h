#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>

class h2UnityWindowMgr : public QObject
{
	Q_OBJECT

public:
	h2UnityWindowMgr(QObject *parent);
	~h2UnityWindowMgr();

	void startMonitorUnityWindow(std::string windowName,unsigned int hz = 1);

private:
	/*
	主程序已经启动Unity程序 但是Unity程序还没有启动完成
	*/
	bool m_startRequest = false;

	QTimer* pm_MonitorTimer = nullptr;
	unsigned int pm_MonitorTimerHz = 1;
	unsigned int m_startRequest_ms = 0;

	unsigned int m_waitMaxTime_ms = 5000;

	std::string m_windowName;

	void stopDetectTimer();

private slots:
	void onMonitorTimer();

signals:
	void signalFoundUnityWindow();
};

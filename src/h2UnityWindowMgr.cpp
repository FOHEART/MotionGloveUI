#include "../inc/h2UnityWindowMgr.h"
#ifdef _WIN32
#include <windows.h>

#else

#endif

h2UnityWindowMgr::h2UnityWindowMgr(QObject* parent)
	: QObject(parent)
{
}

h2UnityWindowMgr::~h2UnityWindowMgr()
{
}

void h2UnityWindowMgr::startMonitorUnityWindow(std::string windowName, unsigned int hz /*= 1*/)
{
#ifdef _WIN32
pm_MonitorTimerHz = hz;
	m_windowName = windowName;

	m_startRequest = true;
	m_startRequest_ms = GetTickCount();
	pm_MonitorTimer = new QTimer(this);
	QObject::connect(pm_MonitorTimer, SIGNAL(timeout()), this, SLOT(onMonitorTimer()));
	pm_MonitorTimer->start(1000 / pm_MonitorTimerHz);
#else

#endif
	
}

void h2UnityWindowMgr::stopDetectTimer()
{
	if (pm_MonitorTimer == 0)
		return;
	QObject::disconnect(pm_MonitorTimer, SIGNAL(timeout()), this, SLOT(onMonitorTimer()));
	//³¬Ê±
	m_startRequest = false;
	pm_MonitorTimer->stop();
	pm_MonitorTimer->deleteLater();
	pm_MonitorTimer = nullptr;
}

void h2UnityWindowMgr::onMonitorTimer()
{
#ifdef _WIN32
// Convert std::string to std::wstring
	std::wstring wstr(m_windowName.begin(), m_windowName.end());
	// Return LPCWSTR
	WId winId = (WId)FindWindow(NULL, wstr.c_str());
	if (winId != 0)
	{
		printf("emit signalFoundUnityWindow\r\n");
		emit signalFoundUnityWindow();
		stopDetectTimer();
	}

	unsigned int current_ms = GetTickCount();
	if ((current_ms - m_startRequest_ms) > m_waitMaxTime_ms)
	{
		printf("FoundUnityWindow timeout\r\n");
		stopDetectTimer();
	}
	else
	{
		//µÈ´ý
	}
#else

#endif
	
}


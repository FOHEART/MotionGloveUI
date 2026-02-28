//#include "stdafx.h"
#include "../inc/h2UnityEmbededDialog.h"
#include <QtGui/QWindow>

void h2UnityEmbededDialog::launchUnityApp()
{
	if (unityProcess)
	{
		delete unityProcess;
		unityProcess = nullptr;
	}

	unityProcess = new QProcess(this);
	// Use the non-deprecated overload: start(program, arguments)
	unityProcess->start(unityExePath, QStringList());

	// Wait a little to let Unity create its main window, then embed it
	QTimer::singleShot(2000, this, &h2UnityEmbededDialog::embedUnityWindow);
}

void h2UnityEmbededDialog::embedUnityWindow()
{
#ifdef _WIN32
WId winId = (WId)FindWindow(NULL, L"foheart_hmax_u3d_port");
	QWindow* window = QWindow::fromWinId(winId);
	QWidget* widget = QWidget::createWindowContainer(window);
	widget->show();
#else

#endif
	
 //childWidget = QWidget::createWindowContainer(window);
 //childWidget->show();

return;
	
}
#ifdef _WIN32
HWND h2UnityEmbededDialog::findWindowByPid(DWORD pid)
{
	HWND hWnd = GetTopWindow(nullptr);
	while (hWnd)
	{
		DWORD windowPid = 0;
		GetWindowThreadProcessId(hWnd, &windowPid);
		if (windowPid == pid)
		{
			// Optionally check window title or class name for verification
			return hWnd;
		}
		hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
	}
	return nullptr;
}
#else

#endif

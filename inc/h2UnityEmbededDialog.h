#pragma once

#include <QtWidgets/QDialog>
#include <QtCore/QProcess>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QTimer>
#ifdef _WIN32
#include <Windows.h>  // For HWND, SetParent, etc.

#else

#endif
// A QDialog class that creates a child widget to hold the Unity window
class  h2UnityEmbededDialog : public QDialog
{
	Q_OBJECT

public:
	explicit h2UnityEmbededDialog(const QString& unityExePath, QWidget* parent = nullptr)
		: QDialog(parent)
		, unityProcess(nullptr)
		, childWidget(nullptr)
		, unityExePath(unityExePath)
	{
		setWindowTitle("Unity in QDialog");

		QVBoxLayout* layout = new QVBoxLayout(this);

		QPushButton* launchButton = new QPushButton("Launch Unity", this);
		layout->addWidget(launchButton);

		// Child widget to embed the Unity window; initially hidden until embedding
		childWidget = new QWidget(this);
		childWidget->setMinimumSize(1200, 900);
		layout->addWidget(childWidget);

		QObject::connect(launchButton, &QPushButton::clicked, this, &h2UnityEmbededDialog::launchUnityApp);
	}

	~h2UnityEmbededDialog()
	{
		// Ensure the Unity process is terminated if it's still running
		if (unityProcess && unityProcess->state() == QProcess::Running)
		{
			unityProcess->kill();
			unityProcess->waitForFinished();
		}
	}

private slots:
	void launchUnityApp();


	void embedUnityWindow();


private:
	QProcess* unityProcess;
	QWidget* childWidget;
	QString unityExePath;
#ifdef _WIN32
// Helper to find the first top-level window belonging to a given process ID
	HWND findWindowByPid(DWORD pid);
#else

#endif
	

};


// Usage example from a main window or another dialog:
// ...
// UnityDialog* dialog = new UnityDialog("C:/Path/To/UnityApp.exe", this);
// dialog->exec();
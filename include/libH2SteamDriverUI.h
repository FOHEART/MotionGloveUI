#pragma once
#include "libh2ui_global.h"

#include <QtWidgets/QWidget>
#ifdef _WIN32

// #include "ui_libH2SteamDriverUI.h" // moved to .cpp
namespace Ui { class libH2SteamDriverUI; }

class LIBH2UI_API  libH2SteamDriverUI : public QWidget
{
	Q_OBJECT

public:
	libH2SteamDriverUI(QWidget *parent = Q_NULLPTR);
	~libH2SteamDriverUI();

	void checkInstallation(void);

private:
	Ui::libH2SteamDriverUI* ui = nullptr;

	QString lbl_green_styleSheet;
	QString lbl_red_styleSheet;

	public slots:
	void on_PB_installSteamVRDriverLocal_clicked();
	void on_PB_uninstallSteamVRDriverLocal_clicked();
	void on_PB_installSteamFromWeb_clicked();

	private slots:
	void onCheck_CBX_steamVRDriverEnable(bool bCheck);
};

#endif

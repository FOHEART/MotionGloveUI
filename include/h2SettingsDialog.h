#pragma once

#include "libh2ui_global.h"

#include <QtWidgets/QDialog>
#include <QtGui/QCloseEvent>

namespace Ui { class h2SettingsDialog; }
class h2NetBroadcastWidget;
class h2StreamConfigWidget;

class LIBH2UI_API h2SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	h2SettingsDialog(QWidget *parent = Q_NULLPTR);
	~h2SettingsDialog();

protected:
	void closeEvent(QCloseEvent *event) override;
private:
	Ui::h2SettingsDialog* ui = nullptr;
	QWidget *m_netBroadcastWidget = 0;
	QWidget *m_streamFormatWidget = 0;

	private slots:
		void on_PB_Cancle_clicked();
		void on_PB_OK_clicked();
		void retranslateUI();
};

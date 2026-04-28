#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2NetBroadcastWidget.h"

class h2NetBroadcastWidget : public QWidget
{
	Q_OBJECT

public:
	h2NetBroadcastWidget(QWidget *parent = Q_NULLPTR);
	~h2NetBroadcastWidget();

	void writeH2StreamConfig();

	// Retranslate UI strings when language changes
	void retranslateUI();
	
	// Initialize language change signal connection
	// Call this after widget creation to enable automatic language updates
	void setupLanguageChangeConnection();

private:
	Ui::h2NetBroadcastWidget ui;
	
	// Helper method to update framerate display with correct language
	void updateFramerateDisplay();
	void fillLocalIPAddress();
	void fillUDPTxFramerate();
	void loadLocalIPAddressFromConfigFile();
	void addIPPortRow(const QString &ip, const QString &port);
	void setAllLabelsWhite();

private slots:
	void on_addAddrBtn_clicked();
	//void on_IPPortListWidget_customContextMenuRequested(const QPoint &pos);
	void on_deleteRowButton_clicked();
	void onLanguageChanged(const QString& langCode);
};

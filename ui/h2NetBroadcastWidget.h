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

private:
	Ui::h2NetBroadcastWidget ui;
	void fillLocalIPAddress();
	void fillUDPTxFramerate();
	void loadLocalIPAddressFromConfigFile();
	void addIPPortRow(const QString &ip, const QString &port);
	void setAllLabelsWhite();

private slots:
	void on_addAddrBtn_clicked();
	//void on_IPPortListWidget_customContextMenuRequested(const QPoint &pos);
	void on_deleteRowButton_clicked();
};

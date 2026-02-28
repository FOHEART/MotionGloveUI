#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>

#include "ui_h2UnityWindowConfig.h"

class h2UnityWindowConfig : public QDialog
{
	Q_OBJECT

public:
	h2UnityWindowConfig(QWidget *parent = Q_NULLPTR);
	~h2UnityWindowConfig();
	unsigned int getGloveSN() const { return m_gloveSN; }
	void setGloveSN(unsigned int val);
private:
	Ui::h2UnityWindowConfig ui;

	unsigned int m_gloveSN = 0;
	bool m_autoOpenUnityPort = false;
	bool m_popoutUnityWindow = false;

	unsigned int m_unityDisplayQuality = 0;
private slots:
	void on_PB_OK_clicked();
	void on_PB_Cancel_clicked();

	/*
	Unity¸ßÇåäÖÈ¾´°¿Ú´°¿ÚÅäÖÃ
	*/
	void onCBX_autoOpenUnityWindow(bool bCheck);

	void onCBX_popoutUnityWindow(bool bCheck);


	void onRB_unityQualityToggled(bool bCheck);

private:
	//bool isUdpPortAvailable(quint16 port);
	
};

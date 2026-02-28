#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include <map>
#include "ui_h2GestureDisplay.h"

class h2GestureDisplay : public QWidget
{
	Q_OBJECT

public:
	h2GestureDisplay(QWidget *parent = Q_NULLPTR);
	~h2GestureDisplay();
	unsigned int getGloveSN() const { return m_gloveSN; }
	void setGloveSN(unsigned int val) { m_gloveSN = val; }

	bool getIsLeftHand() const { return m_bIsLeftHand; }
	void setIsLeftHand(bool val);
	bool getIsRightHand() const { return m_bIsRightHand; }
	void setIsRightHand(bool val);

	bool getIsEnable() const { return m_bIsEnable; }
	void setIsEnable(bool val);

	/*
	将配置文件中的设置读出来显示到界面上
	*/
	void readGloveConfigFileToUI(void);

private slots:
	void onH2UpdateGestureTimer();

private:
	Ui::h2GestureDisplay ui;
	unsigned int m_gloveSN = 0;
	bool m_bIsLeftHand = false;
	bool m_bIsRightHand = false;
	bool m_bIsEnable = false;
	QTimer* h2GestureTimer = nullptr;
	unsigned int h2GestureTimerHz = 5;

	std::map<std::string, QPixmap> m_gesturePixmapMap_LH;
	std::map<std::string, QPixmap> m_gesturePixmapMap_RH;

private:
	std::string GetLocalDllDirectory();
	void addGestureDisplayPicture(bool isLeftHand, bool isRightHand);
};

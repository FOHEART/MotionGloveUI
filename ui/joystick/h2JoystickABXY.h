#pragma once
#include <QtWidgets/QWidget>
#include "ui_h2JoystickABXY.h"

class  h2JoystickABXY : public QWidget
{
	Q_OBJECT

public:
	h2JoystickABXY(QWidget *parent = Q_NULLPTR);
	~h2JoystickABXY();

	bool getIsADown() const { return isADown; }
	void setIsADown(bool val) { isADown = val; }
	bool getIsBDown() const { return isBDown; }
	void setIsBDown(bool val) { isBDown = val; }
	bool getIsXDown() const { return isXDown; }
	void setIsXDown(bool val) { isXDown = val; }
	bool getIsYDown() const { return isYDown; }
	void setIsYDown(bool val) { isYDown = val; }

	void updateUI(void);
private:
	Ui::h2JoystickABXY ui;

	bool isADown = false;
	bool isBDown = false;
	bool isXDown = false;
	bool isYDown = false;

	QString upStyleSheet;
	QString downStyleSheet;
};

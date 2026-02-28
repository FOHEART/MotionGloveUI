#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2FingerClose.h"

class  h2FingerClose : public QWidget
{
	Q_OBJECT

public:
	h2FingerClose(QWidget *parent = Q_NULLPTR);
	~h2FingerClose();

	void setIsLeftHand(void);
	void setIsRightHand(void);

	void setIsFinger2Close(bool s);
	void setIsFinger3Close(bool s);
	void setIsFinger4Close(bool s);
	void setIsFinger5Close(bool s);

	void setIsTrigger(bool s);
	void setIsGrab(bool s);
	void setIsPinch(bool s);
	void setIsMenu(bool s);


	void updateUI(void);
private:
	Ui::h2FingerClose ui;
	bool isLeftHand=false;
	bool isRightHand = false;

	bool isFinger2Close = false;
	bool isFinger3Close = false;
	bool isFinger4Close = false;
	bool isFinger5Close = false;

	bool isTrigger = false;
	bool isGrab = false;
	bool isPinch = false;
	bool isMenu = false;

	QString upStyleSheet;
	QString downStyleSheet;
};

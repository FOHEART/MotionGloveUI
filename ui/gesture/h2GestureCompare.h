#pragma once
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>

#include "ui_h2GestureCompare.h"

class   h2GestureCompare : public QWidget
{
	Q_OBJECT

public:
	h2GestureCompare(QWidget *parent = Q_NULLPTR);
	~h2GestureCompare();

	void setName(std::string name);
	void setMinMaxVal(float *v);
	bool getIsLeftHand() const { return isLeftHand; }
	void setIsLeftHand(bool val) { isLeftHand = val; }
	bool getIsRightHand() const { return isRightHand; }
	void setIsRightHand(bool val) { isRightHand = val; }

private:
	Ui::h2GestureCompare ui;
	float minMaxVal[5][2];
	//×óÊÖÄ´Ö¸ÔÚÓÒ±ß
	QLabel *handLabelMin[5];
	QLabel *handLabelMax[5];

	bool isLeftHand=false;
	bool isRightHand = false;
};

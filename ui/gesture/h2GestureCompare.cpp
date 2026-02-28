//#include "stdafx.h"
#include "h2GestureCompare.h"

h2GestureCompare::h2GestureCompare(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	
}

h2GestureCompare::~h2GestureCompare()
{
}

void h2GestureCompare::setName(std::string name)
{
	ui.LBL_gestureName->setText(QString::fromStdString(name));
}

void h2GestureCompare::setMinMaxVal(float *v)
{
	for (int i = 0; i < 5; i++)
	{
		minMaxVal[i][0] = v[i*2+0];
		minMaxVal[i][1] = v[i*2+1];
	}

	if (isLeftHand)
	{
		handLabelMin[0] = ui.LBL_pinkyMin;
		handLabelMin[1] = ui.LBL_ringMin;
		handLabelMin[2] = ui.LBL_middleMin;
		handLabelMin[3] = ui.LBL_indexMin;
		handLabelMin[4] = ui.LBL_thumbMin;


		handLabelMax[0] = ui.LBL_pinkyMax;
		handLabelMax[1] = ui.LBL_ringMax;
		handLabelMax[2] = ui.LBL_middleMax;
		handLabelMax[3] = ui.LBL_indexMax;
		handLabelMax[4] = ui.LBL_thumbMax;
	}

	if (isRightHand)
	{
		handLabelMin[0] = ui.LBL_thumbMin ;
		handLabelMin[1] = ui.LBL_indexMin ;
		handLabelMin[2] = ui.LBL_middleMin;
		handLabelMin[3] = ui.LBL_ringMin;
		handLabelMin[4] = ui.LBL_pinkyMin;


		handLabelMax[0] = ui.LBL_thumbMax ;
		handLabelMax[1] = ui.LBL_indexMax ;
		handLabelMax[2] = ui.LBL_middleMax;
		handLabelMax[3] = ui.LBL_ringMax;
		handLabelMax[4] = ui.LBL_pinkyMax;
	}

	handLabelMin[0]->setText(QString::number(minMaxVal[0][0]));
	handLabelMax[0]->setText(QString::number(minMaxVal[0][1]));

	handLabelMin[1]->setText(QString::number(minMaxVal[1][0]));
	handLabelMax[1]->setText(QString::number(minMaxVal[1][1]));

	handLabelMin[2]->setText(QString::number(minMaxVal[2][0]));
	handLabelMax[2]->setText(QString::number(minMaxVal[2][1]));

	handLabelMin[3]->setText(QString::number(minMaxVal[3][0]));
	handLabelMax[3]->setText(QString::number(minMaxVal[3][1]));

	handLabelMin[4]->setText(QString::number(minMaxVal[4][0]));
	handLabelMax[4]->setText(QString::number(minMaxVal[4][1]));
}

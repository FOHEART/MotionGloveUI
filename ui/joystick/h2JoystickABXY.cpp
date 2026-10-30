/**
 * @file  h2JoystickABXY.cpp
 * @brief 手柄 ABXY 按键状态显示控件的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 界面布局由同名 .ui 文件生成。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

#include "h2JoystickABXY.h"

h2JoystickABXY::h2JoystickABXY(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	upStyleSheet = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(0x12) + "," +
		QString::number(0x96) + "," +
		QString::number(0xdb) + ");}";
	downStyleSheet = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(0xd8) + "," +
		QString::number(0x1e) + "," +
		QString::number(0x06) + ");}";

	ui.LBL_x->setStyleSheet(upStyleSheet);
	ui.LBL_y->setStyleSheet(upStyleSheet);
	ui.LBL_a->setStyleSheet(upStyleSheet);
	ui.LBL_b->setStyleSheet(upStyleSheet);
}

h2JoystickABXY::~h2JoystickABXY()
{
}

void h2JoystickABXY::updateUI(void)
{
	if (isADown)
	{
		ui.LBL_a->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_a->setStyleSheet(upStyleSheet);
	}
//////////////////////////////////////////////////////////////////////////
	if (isBDown)
	{
		ui.LBL_b->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_b->setStyleSheet(upStyleSheet);
	}
	/////////////
	if (isXDown)
	{
		ui.LBL_x->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_x->setStyleSheet(upStyleSheet);
	}
	//////////
	if (isYDown)
	{
		ui.LBL_y->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_y->setStyleSheet(upStyleSheet);
	}
}

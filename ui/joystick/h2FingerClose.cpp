/**
 * @file  h2FingerClose.cpp
 * @brief 握拳/张手状态显示控件的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 界面布局由同名 .ui 文件生成。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

//#include "stdafx.h"
#include "h2FingerClose.h"

h2FingerClose::h2FingerClose(QWidget *parent)
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

	ui.LBL_1->setStyleSheet(upStyleSheet);
	ui.LBL_2->setStyleSheet(upStyleSheet);
	ui.LBL_3->setStyleSheet(upStyleSheet);
	ui.LBL_4->setStyleSheet(upStyleSheet);
}

h2FingerClose::~h2FingerClose()
{
}

void h2FingerClose::setIsLeftHand(void)
{
	isLeftHand = true;
}

void h2FingerClose::setIsRightHand(void)
{
	isRightHand = true;
}

void h2FingerClose::setIsFinger2Close(bool s)
{
	isFinger2Close = s;
}

void h2FingerClose::setIsFinger3Close(bool s)
{
	isFinger3Close = s;
}

void h2FingerClose::setIsFinger4Close(bool s)
{
	isFinger4Close = s;
}

void h2FingerClose::setIsFinger5Close(bool s)
{
	isFinger5Close = s;
}

void h2FingerClose::setIsTrigger(bool s)
{
	isTrigger = s;
}

void h2FingerClose::setIsGrab(bool s)
{
	isGrab = s;
}

void h2FingerClose::setIsPinch(bool s)
{
	isPinch = s;
}

void h2FingerClose::setIsMenu(bool s)
{
	isMenu = s;
}

void h2FingerClose::updateUI(void)
{
	if (isFinger2Close)
	{
		ui.LBL_1->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_1->setStyleSheet(upStyleSheet);
	}

	//--------
	if (isFinger3Close)
	{
		ui.LBL_2->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_2->setStyleSheet(upStyleSheet);
	}
	//--------
	if (isFinger4Close)
	{
		ui.LBL_3->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_3->setStyleSheet(upStyleSheet);
	}
	//--------
	if (isFinger5Close)
	{
		ui.LBL_4->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_4->setStyleSheet(upStyleSheet);
	}
	//
	if (isTrigger)
	{
		ui.LBL_trigger->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_trigger->setStyleSheet(upStyleSheet);
	}
	//
	if (isGrab)
	{
		ui.LBL_grab->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_grab->setStyleSheet(upStyleSheet);
	}
	//
	if (isPinch)
	{
		ui.LBL_pinch->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_pinch->setStyleSheet(upStyleSheet);
	}
	//
	if (isMenu)
	{
		ui.LBL_menu->setStyleSheet(downStyleSheet);
	}
	else
	{
		ui.LBL_menu->setStyleSheet(upStyleSheet);
	}
}

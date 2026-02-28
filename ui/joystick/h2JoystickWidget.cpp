//#include "stdafx.h"
#pragma execution_character_set("utf-8")

#include "h2JoystickWidget.h"
#include "ui_h2JoystickWidget.h"
#include <QtCore/QTimer>
#include <iostream>
#include <sstream>
#include <iomanip> // 包含 std::setprecision 的头文件
#include "h2sdk.h"

h2JoystickWidget::h2JoystickWidget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::h2JoystickWidget();
	ui->setupUi(this);

	pbStylesheetGreen = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(0) + "," +
		QString::number(255) + "," +
		QString::number(0) + ");}";
	pbStylesheetRed = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(255) + "," +
		QString::number(0) + "," +
		QString::number(0) + ");}";
	if (m_isIndexSimuDebug)
	{
		ui->PB_indexSimuDebug_LH->setVisible(true);
	}
	else
	{
		ui->PB_indexSimuDebug_LH->setVisible(false);
	}
	QObject::connect(ui->HS_JSDeadZone_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_JSDeadZone_LH_Changed(int)));
	QObject::connect(ui->HS_JSDeadZone_LH, SIGNAL(sliderReleased()), this, SLOT(onHS_JSDeadZone_LH_Released()));


}

h2JoystickWidget::~h2JoystickWidget()
{
	delete ui;
}

void h2JoystickWidget::setIsLeftHand(bool val)
{
	m_bIsLeftHand = val;
	ui->GBX_joyKey->setTitle(QObject::tr(""));

	h2FingerClose* lc = ui->WIG_close_LH;
	lc->setIsLeftHand();
}

void h2JoystickWidget::setIsRightHand(bool val)
{
	m_bIsRightHand = val;
	ui->GBX_joyKey->setTitle(QObject::tr(""));

	h2FingerClose* lc = ui->WIG_close_LH;
	lc->setIsRightHand();
}

void h2JoystickWidget::setIsEnable(bool val)
{
	val = true;
	m_bIsEnable = val;

	ui->GBX_joyKey->setEnabled(val);
	if(val)
	{
		ui->WIG_jsLeftHand->setEnabled(true);
		ui->WIG_abxyLeftHand->setEnabled(true);
		ui->LBL_JS_X_LH->setEnabled(true);
		ui->LBL_JS_Y_LH->setEnabled(true);
		ui->HS_JSDeadZone_LH->setEnabled(true);
		ui->LBL_JSDeadZone_LH->setEnabled(true);
		//ui->WIG_close_LH->setEnabled(true);

	}
	else
	{
		ui->WIG_jsLeftHand->setEnabled(false);
		ui->WIG_abxyLeftHand->setEnabled(false);
		ui->LBL_JS_X_LH->setEnabled(false);
		ui->LBL_JS_Y_LH->setEnabled(false);
		ui->HS_JSDeadZone_LH->setEnabled(false);
		ui->LBL_JSDeadZone_LH->setEnabled(false);
		//ui->WIG_close_LH->setEnabled(false);
	}


	if (val)
	{
		startUpdateUITimer();
	}
	else
	{
		stopUpdateUITimer();
	}
}

void h2JoystickWidget::setIsJoyStickExist(bool val)
{
	m_bJoyStickExist = val;

	if(val)
	{
		ui->WIG_jsLeftHand->setEnabled(true);
		ui->WIG_abxyLeftHand->setEnabled(true);
		ui->LBL_JS_X_LH->setEnabled(true);
		ui->LBL_JS_Y_LH->setEnabled(true);
		ui->HS_JSDeadZone_LH->setEnabled(true);
		ui->LBL_JSDeadZone_LH->setEnabled(true);
		ui->WIG_close_LH->setEnabled(true);

	}
	else
	{
		ui->WIG_jsLeftHand->setEnabled(false);
		ui->WIG_abxyLeftHand->setEnabled(false);
		ui->LBL_JS_X_LH->setEnabled(false);
		ui->LBL_JS_Y_LH->setEnabled(false);
		ui->HS_JSDeadZone_LH->setEnabled(false);
		ui->LBL_JSDeadZone_LH->setEnabled(false);
		ui->WIG_close_LH->setEnabled(true);

	}
}

void h2JoystickWidget::readGloveConfigFileToUI(void)
{
	unsigned int m_sn = m_gloveSN;
	/*加载配置文件中的配置*/
	h2GloveMgr newX2Suit;
	//m_x2SuitMgr = ((x2SuitMgr *)(MCAPP::Ins()->g_x2SuitMgr));
	bool isFind = H2_SDK_getCurrentX2SuitBySn(m_sn, newX2Suit);
	if (!isFind)
		return;

	{
		//配置文件中的死区 读到界面上
		int lhdz = std::round(newX2Suit.LeftDeadZonePercent);
		int rhdz = std::round(newX2Suit.RightDeadZonePercent);
		if (m_bIsLeftHand)
		{
			ui->HS_JSDeadZone_LH->setValue(lhdz);

		}
		else if (m_bIsRightHand)
		{
			ui->HS_JSDeadZone_LH->setValue(rhdz);

		}

	}
	//设置死区 将界面上的值 设置到SDK之中
	{
		int deadZone_LH = ui->HS_JSDeadZone_LH->value();
		ui->LBL_JSDeadZone_LH->setText(QString::number(deadZone_LH) + "%");
		if (m_bIsLeftHand)
		{
			H2_SDK_setJoystick_leftHand_deadzone(m_sn, deadZone_LH);

		}
		else if (m_bIsRightHand)
		{
			H2_SDK_setJoystick_rightHand_deadzone(m_sn, deadZone_LH);
		}


	}

}

void h2JoystickWidget::onH2JoystickTimer()
{
	if (!m_bIsEnable)
	{
		return;
	}
	unsigned int sn = m_gloveSN;
	unsigned int isHandOnline = false;

	if (m_bIsLeftHand)
	{
		isHandOnline = H2_SDK_isLeftHand_Connected(sn);
	}
	else if (m_bIsRightHand)
	{
		isHandOnline = H2_SDK_isRightHand_Connected(sn);
	}
	h2JoyStickPadWidget* lj = ui->WIG_jsLeftHand;
	if (isHandOnline)
	{
		float jsadjust_x = 0;
		float jsadjust_y = 0;
		if (m_bIsLeftHand)
		{
			H2_SDK_getJoystick_leftHand_xy(sn, &jsadjust_x, &jsadjust_y);

		}
		else if (m_bIsRightHand)
		{
			H2_SDK_getJoystick_rightHand_xy(sn, &jsadjust_x, &jsadjust_y);
			jsadjust_x = -jsadjust_x;
			jsadjust_y = -jsadjust_y;
		}

		lj->setXY(jsadjust_x, jsadjust_y);

		{
			std::ostringstream oss;
			// std::fixed 表示使用定点格式，不使用科学计数法
			// std::setprecision(4) 表示小数点后 4 位
			oss << std::fixed << std::setprecision(4) << jsadjust_x;
			ui->LBL_JS_X_LH->setText(QString::fromStdString(oss.str()));
		}
		{
			std::ostringstream oss;
			// std::fixed 表示使用定点格式，不使用科学计数法
			// std::setprecision(4) 表示小数点后 4 位
			oss << std::fixed << std::setprecision(4) << jsadjust_y;
			ui->LBL_JS_Y_LH->setText(QString::fromStdString(oss.str()));
		}
	}
	else
	{
		lj->setXY(0, 0);
	}

	{
		bool leftHandConnected = false;
		bool rightHandConnected = false;
		H2_SDK_getOpenGloveStatus(sn,
			&leftHandConnected,
			&rightHandConnected
		);

		if (leftHandConnected || rightHandConnected)
		{
			ui->LBL_openGloveStatus_LH->setStyleSheet(pbStylesheetGreen);
			ui->LBL_openGloveStatus_LH->setText("已连接");
		}
		else
		{
			ui->LBL_openGloveStatus_LH->setStyleSheet(pbStylesheetRed);
			ui->LBL_openGloveStatus_LH->setText("未连接");
		}
	}

	{
		h2JoyStickPadWidget* lj = ui->WIG_jsLeftHand;
		h2JoystickABXY* lb = ui->WIG_abxyLeftHand;
		h2FingerClose* lc = ui->WIG_close_LH;

		//////
		if (isHandOnline)
		{
			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isJoyStickKeyDown(sn))
				{
					lj->setJoyStickDown(true);
				}
				else
				{
					lj->setJoyStickDown(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isJoyStickKeyDown(sn))
				{
					lj->setJoyStickDown(true);
				}
				else
				{
					lj->setJoyStickDown(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isAKeyDown(sn))
				{
					lb->setIsADown(true);
				}
				else
				{
					lb->setIsADown(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isAKeyDown(sn))
				{
					lb->setIsADown(true);
				}
				else
				{
					lb->setIsADown(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isXKeyDown(sn))
				{
					lb->setIsXDown(true);
				}
				else
				{
					lb->setIsXDown(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isXKeyDown(sn))
				{
					lb->setIsXDown(true);
				}
				else
				{
					lb->setIsXDown(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isYKeyDown(sn))
				{
					lb->setIsYDown(true);
				}
				else
				{
					lb->setIsYDown(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isYKeyDown(sn))
				{
					lb->setIsYDown(true);
				}
				else
				{
					lb->setIsYDown(false);
				}
			}

			
			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isBKeyDown(sn))
				{
					lb->setIsBDown(true);
				}
				else
				{
					lb->setIsBDown(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isBKeyDown(sn))
				{
					lb->setIsBDown(true);
				}
				else
				{
					lb->setIsBDown(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isThumbIndexTouch(sn))
				{
					lc->setIsFinger2Close(true);
				}
				else
				{
					lc->setIsFinger2Close(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isThumbIndexTouch(sn))
				{
					lc->setIsFinger2Close(true);
				}
				else
				{
					lc->setIsFinger2Close(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isThumbMiddleTouch(sn))
				{
					lc->setIsFinger3Close(true);
				}
				else
				{
					lc->setIsFinger3Close(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isThumbMiddleTouch(sn))
				{
					lc->setIsFinger3Close(true);
				}
				else
				{
					lc->setIsFinger3Close(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isThumbRingTouch(sn))
				{
					lc->setIsFinger4Close(true);
				}
				else
				{
					lc->setIsFinger4Close(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isThumbRingTouch(sn))
				{
					lc->setIsFinger4Close(true);
				}
				else
				{
					lc->setIsFinger4Close(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getJoystick_leftHand_isThumbPinkyTouch(sn))
				{
					lc->setIsFinger5Close(true);
				}
				else
				{
					lc->setIsFinger5Close(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getJoystick_rightHand_isThumbPinkyTouch(sn))
				{
					lc->setIsFinger5Close(true);
				}
				else
				{
					lc->setIsFinger5Close(false);
				}
			}

			if (m_bIsLeftHand)
			{
				//手势模拟按键
				if (H2_SDK_getIndexSimu_leftHand_isTrigger(sn))
				{
					lc->setIsTrigger(true);
				}
				else
				{
					lc->setIsTrigger(false);
				}
			}
			else if (m_bIsRightHand)
			{
				//手势模拟按键
				if (H2_SDK_getIndexSimu_rightHand_isTrigger(sn))
				{
					lc->setIsTrigger(true);
				}
				else
				{
					lc->setIsTrigger(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getIndexSimu_leftHand_isGrab(sn))
				{
					lc->setIsGrab(true);
				}
				else
				{
					lc->setIsGrab(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getIndexSimu_rightHand_isGrab(sn))
				{
					lc->setIsGrab(true);
				}
				else
				{
					lc->setIsGrab(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getIndexSimu_leftHand_isPinch(sn))
				{
					lc->setIsPinch(true);
				}
				else
				{
					lc->setIsPinch(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getIndexSimu_rightHand_isPinch(sn))
				{
					lc->setIsPinch(true);
				}
				else
				{
					lc->setIsPinch(false);
				}
			}

			if (m_bIsLeftHand)
			{
				if (H2_SDK_getIndexSimu_leftHand_isMenu(sn))
				{
					lc->setIsMenu(true);
				}
				else
				{
					lc->setIsMenu(false);
				}
			}
			else if (m_bIsRightHand)
			{
				if (H2_SDK_getIndexSimu_rightHand_isMenu(sn))
				{
					lc->setIsMenu(true);
				}
				else
				{
					lc->setIsMenu(false);
				}
			}

		}
		else
		{
			lj->setJoyStickDown(false);
			lb->setIsADown(false);
			lb->setIsBDown(false);
			lb->setIsXDown(false);
			lb->setIsYDown(false);

			lc->setIsFinger2Close(false);
			lc->setIsFinger3Close(false);
			lc->setIsFinger4Close(false);
			lc->setIsFinger5Close(false);
			lc->setIsTrigger(false);
			lc->setIsGrab(false);
			lc->setIsPinch(false);
			lc->setIsMenu(false);
		}


		lb->updateUI();
		lc->updateUI();
	}
}

void h2JoystickWidget::onHS_JSDeadZone_LH_Changed(int value)
{
	int deadZone_LH = ui->HS_JSDeadZone_LH->value();
	ui->LBL_JSDeadZone_LH->setText(QString::number(deadZone_LH) + "%");
	if (m_bIsLeftHand)
	{
		H2_SDK_setJoystick_leftHand_deadzone(m_gloveSN, deadZone_LH);

	}
	else if (m_bIsRightHand)
	{
		H2_SDK_setJoystick_rightHand_deadzone(m_gloveSN, deadZone_LH);

	}
}

void h2JoystickWidget::onHS_JSDeadZone_LH_Released()
{
	//保存
	h2GloveMgr newX2Suit;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(m_gloveSN, newX2Suit);
	if (isFind)
	{
		if (m_bIsLeftHand)
		{
			newX2Suit.LeftDeadZonePercent =
				ui->HS_JSDeadZone_LH->value();
		}
		else if (m_bIsRightHand)
		{
			newX2Suit.RightDeadZonePercent =
				ui->HS_JSDeadZone_LH->value();
		}

		H2_SDK_updateX2SuitMgrVec(m_gloveSN, newX2Suit);
	}
	else
	{
		printf(" ");
	}
}

void h2JoystickWidget::on_PB_openOpenGlove_clicked(void)
{
	 H2_SDK_setOpenGloveEnable(
		 m_gloveSN,
		true,true
	);
}

void h2JoystickWidget::on_PB_closeOpenGlove_clicked(void)
{
	H2_SDK_setOpenGloveEnable(
		m_gloveSN,
		false, false
	);
}

void h2JoystickWidget::startUpdateUITimer(void)
{
	if (h2JoyStickTimer == nullptr)
		h2JoyStickTimer = new QTimer(this);

	if (h2JoyStickTimer->isActive())
	{
		return;
	}

	QObject::connect(h2JoyStickTimer, SIGNAL(timeout()), this, SLOT(onH2JoystickTimer()));
	h2JoyStickTimer->start(1000 / h2JoyStickTimerHz);
}

void h2JoystickWidget::stopUpdateUITimer(void)
{
	if (h2JoyStickTimer->isActive())
	{

	}
	else
	{
		return;
	}

	QObject::disconnect(h2JoyStickTimer, SIGNAL(timeout()), this, SLOT(onH2JoystickTimer()));
	h2JoyStickTimer->stop();
}

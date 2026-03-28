#pragma execution_character_set("utf-8")

#include <QtGui/QWindow>
#include <cmath>
#include "../include/h2Widget.h"
#include "ui_h2Widget.h"
#include "h2sdk_def.h"
#include "ui/finger/h2FingerAdjustFreeModeWidget.h"
#include "ui/finger/h2FingerAdjustLockYawWidget.h"
#include "ui/finger/h2FingerAdjustEnhancedWidget.h"
#include "ui/joystick/h2FingerClose.h"

#include "ui/joystick/h2JoyStickPadWidget.h"
#include "ui/h2IndexSimuWidget.h"
#include "ui/joystick/h2JoystickWidget.h"
#include "ui/joystick/h2JoystickCali.h"
#include "ui/gesture/h2GestureContainer.h"
#include "ui/steamvr/h2SteamVRMonitorWidget.h"
#include "ui/skeleton/h2SkeletonWidget.h"
#include "ui/h2Notusable.h"
#include "ui/gesture/h2GestureDisplay.h"
#include "../inc/h2GloveSepMgr.h"
#include <QButtonGroup>

#include "CarbonUIClass.h"
#include "groupboxcollapser.h"

#ifdef _WIN32
#include "libH2SteamDriver.h"

#else

#endif


h2Widget::h2Widget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::h2Widget();
	ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground, true);
	// set background and a 1px border with color #212121
	//setStyleSheet("background-color: #3b3b3b; border-top: 1px solid #212121; border-left: 1px solid #212121; border-bottom: 1px solid #212121; border-right: none;");


	initUI();
	updateUartPorts();
#ifdef _WIN32
	if (m_isSteamVRMonitorEnabled)
		((h2SteamVRMonitorWidget*)WIG_steamVRMonitor)->init();
#else

#endif

	/*
	end
	*/

	//设置默认的tab页是手势识别
	//ui->TW_Config->setCurrentIndex(ui->TW_Config->indexOf(ui->TBL_gestureDisplay));

	//--end

	//手套捕捉模式
	//QObject::connect(ui->CBX_gloveCapMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onGloveCaptureModeChanged(int)));

}

h2Widget::~h2Widget()
{
	if (m_gestureContainer)
	{
		delete m_gestureContainer;
		m_gestureContainer = 0;
	}


	delete ui;
	ui = nullptr;
}

void h2Widget::setSNAndAN(const unsigned int& suitNumber3E,
	const std::string& actorName)
{
	setActorName(actorName);
	setSn(suitNumber3E);
	printf("setSNAndAN: suitNumber3E=%u, actorName=%s\n", suitNumber3E, actorName.c_str());
	//m_liveOrReplayIndex = liveOrReplayIndex;
	//只有第一套可以连接openglove
	int globalIndex = 0;
	H2_SDK_getCreateIndexGlobal(m_sn, &globalIndex);
	if (globalIndex == 0)
	{
		H2_SDK_setOpenGloveEnable(m_sn, true, true);
	}
	else
	{
		H2_SDK_setOpenGloveEnable(m_sn, false, false);
	}
	 

	((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->setGloveSN(m_sn);
	((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->setIsLeftHand(true);
	((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->readGloveConfigFileToUI();

	((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->setGloveSN(m_sn);
	((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->setIsRightHand(true);
	((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->readGloveConfigFileToUI();

	((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->setGloveSN(m_sn);
	((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->setIsLeftHand(true);
	((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->readGloveConfigFileToUI();
	
	((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->setGloveSN(m_sn);
	((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->setIsRightHand(true);
	((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->readGloveConfigFileToUI();

	//freemode
	((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->setGloveSN(m_sn);
	((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->setIsLeftHand(true);
	((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->readGloveConfigFileToUI();

	((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->setGloveSN(m_sn);
	((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->setIsRightHand(true);
	((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->readGloveConfigFileToUI();
#if 0
	//
	((h2JoystickWidget*)WIG_Joystick_LH)->setGloveSN(m_sn);
	((h2JoystickWidget*)WIG_Joystick_LH)->setIsLeftHand(true);
	((h2JoystickWidget*)WIG_Joystick_LH)->readGloveConfigFileToUI();

	((h2JoystickWidget*)WIG_Joystick_RH)->setGloveSN(m_sn);
	((h2JoystickWidget*)WIG_Joystick_RH)->setIsRightHand(true);
	((h2JoystickWidget*)WIG_Joystick_RH)->readGloveConfigFileToUI();
#endif



	//手势显示
	((h2GestureDisplay*)WIG_h2GestureDisplay_LH)->setGloveSN(m_sn);
	((h2GestureDisplay*)WIG_h2GestureDisplay_LH)->setIsLeftHand(true);
	((h2GestureDisplay*)WIG_h2GestureDisplay_LH)->readGloveConfigFileToUI();
	((h2GestureDisplay*)WIG_h2GestureDisplay_RH)->setGloveSN(m_sn);
	((h2GestureDisplay*)WIG_h2GestureDisplay_RH)->setIsRightHand(true);
	((h2GestureDisplay*)WIG_h2GestureDisplay_RH)->readGloveConfigFileToUI();
#if 0
	/*
		设置摇杆校准的左右手控件;
		*/

	if (WIG_jsCali_LH != 0)
	{
		((h2JoystickCali*)WIG_jsCali_LH)->setIsLeftHand(true);
		((h2JoystickCali*)WIG_jsCali_LH)->setSn(m_sn);
		((h2JoystickCali*)WIG_jsCali_LH)->restoreCaliValueFromConfig();
	}
	if (WIG_jsCali_RH != 0)
	{
		((h2JoystickCali*)WIG_jsCali_RH)->setIsRightHand(true);

		((h2JoystickCali*)WIG_jsCali_RH)->setSn(m_sn);
		((h2JoystickCali*)WIG_jsCali_RH)->restoreCaliValueFromConfig();
	}
	/*
	end
	*/
#endif


	if (h2MonitorTimer == NULL)
	{
		h2MonitorTimer = new QTimer(this);
		QObject::connect(h2MonitorTimer, SIGNAL(timeout()), this, SLOT(onH2MonitorTimer()));
		h2MonitorTimer->start(1000 / h2MonitorTimerHz);
		onH2MonitorTimer();
		readGloveConfigFileToUI();
	}

}

void h2Widget::onGloveCaptureModeChanged(int index)
{
#if 1
	if (m_sn == 0)
		return;
	printf("onGloveCaptureModeChanged:%d\r\n", index);
	m_MocapGloveMode = (h2GloveMode_TypeDef)index;

	{
		/*保存套装的配置文件*/
		/*已经有了信息 更新当前套装的配置信息*/
		h2GloveMgr newX2Suit;
		bool isFind = H2_SDK_getCurrentX2SuitBySn(m_sn, newX2Suit);
		if (isFind)
		{
			if (m_MocapGloveMode != newX2Suit.GloveMocapMode)
			{
				newX2Suit.GloveMocapMode = m_MocapGloveMode;
				H2_SDK_updateX2SuitMgrVec(m_sn, newX2Suit);
			}
		}
		else
		{
			//没找到 不可能出现这种情况
			printf(" ");
		}
	}

	{
		WIG_fingerAdjustFreeMode_LH->setVisible(false);
		WIG_fingerAdjustFreeMode_RH->setVisible(false);
		WIG_fingerAdjustLockYaw_LH->setVisible(false);
		WIG_fingerAdjustLockYaw_RH->setVisible(false);
		WIG_fingerAdjustEnhanced_LH->setVisible(false);
		WIG_fingerAdjustEnhanced_RH->setVisible(false);
		switch (m_MocapGloveMode)
		{
			case h2GloveMode_Free:
			{
				WIG_fingerAdjustFreeMode_LH->setVisible(true);
				((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->readGloveConfigFileToUI();
				WIG_fingerAdjustFreeMode_RH->setVisible(true);
				((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->readGloveConfigFileToUI();
				H2_SDK_setMode(m_sn, m_MocapGloveMode);
			}
			break;
			case h2GloveMode_YawLocked:
			{
				WIG_fingerAdjustLockYaw_LH->setVisible(true);
				((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->readGloveConfigFileToUI();
				WIG_fingerAdjustLockYaw_RH->setVisible(true);
				((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->readGloveConfigFileToUI();
				H2_SDK_setMode(m_sn, m_MocapGloveMode);
			}
			break;
			case h2GloveMode_Enhanced:
			{
				WIG_fingerAdjustEnhanced_LH->setVisible(true);
				((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->readGloveConfigFileToUI();
				WIG_fingerAdjustEnhanced_RH->setVisible(true);
				((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->readGloveConfigFileToUI();
				H2_SDK_setMode(m_sn, m_MocapGloveMode);
			}
			break;
			default:
				break;
		}
	}
#endif
}

void h2Widget::onH2MonitorTimer()
{
	/*
	如果有左右手上线就把手套设置窗口使能，应该单独写一个定时器
	*/
	unsigned int sn = m_sn;
	unsigned int isLeftHandOnline = 0;
	unsigned int isRightHandOnline = 0;
	unsigned int leftHandBattery = 0;
	unsigned int rightHandBattery = 0;
	unsigned int leftHandFwVer = 0;
	unsigned int rightHandFwVer = 0;
	H2_SDK_getStatus(sn,
		&isLeftHandOnline,
		&isRightHandOnline,
		&leftHandBattery,
		&rightHandBattery,
		&leftHandFwVer,
		&rightHandFwVer
	);
	if (isLeftHandOnline || isRightHandOnline)
	{
		if (!WIG_fingerAdjustFreeMode_LH->isEnabled())
			WIG_fingerAdjustFreeMode_LH->setEnabled(true);
		if (!WIG_fingerAdjustFreeMode_RH->isEnabled())
			WIG_fingerAdjustFreeMode_RH->setEnabled(true);
		if (!WIG_fingerAdjustLockYaw_LH->isEnabled())
			WIG_fingerAdjustLockYaw_LH->setEnabled(true);
		if (!WIG_fingerAdjustLockYaw_RH->isEnabled())
			WIG_fingerAdjustLockYaw_RH->setEnabled(true);
		if (!WIG_fingerAdjustEnhanced_LH->isEnabled())
			WIG_fingerAdjustEnhanced_LH->setEnabled(true);
		if (!WIG_fingerAdjustEnhanced_RH->isEnabled())
			WIG_fingerAdjustEnhanced_RH->setEnabled(true);
	}

	updateUIFromGloveOnlineStatus(isLeftHandOnline, isRightHandOnline);

	unsigned int leftHandFrameRate = 0;
	unsigned int rightHandFrameRate = 0;
	H2_SDK_getFrameRate(m_sn,
		&leftHandFrameRate,
		&rightHandFrameRate);

	ui->LBL_leftHandFR->setText(QString::number(leftHandFrameRate) + " fps");
	ui->LBL_rightHandFR->setText(QString::number(rightHandFrameRate) + " fps");

	//printf("%d %d\r\n", leftHandFrameRate, rightHandFrameRate);
}

/*
调试手套的手势识别
*/
void h2Widget::on_PB_gestureConfig_clicked()
{
	if (m_gestureContainer == 0)
	{
		m_gestureContainer = new h2GestureContainer(this);
	}

	((h2GestureContainer*)m_gestureContainer)->setGloveSN(m_sn);
	((h2GestureContainer*)m_gestureContainer)->generateUI();
	QVBoxLayout* layout = new QVBoxLayout();
	// 将子页面添加到布局管理器中
	layout->addWidget(m_gestureContainer);

	QDialog* dlg = new QDialog();
	dlg->setLayout(layout);

	dlg->show();
}

void h2Widget::on_PB_indexSimuDebug_LH_clicked()
{
	h2IndexSimuWidget* t_cali = new h2IndexSimuWidget();
	t_cali->setSn(m_sn);
	QVBoxLayout* layout = new QVBoxLayout();
	// 将子页面添加到布局管理器中
	layout->addWidget(t_cali);

	QDialog* dlg = new QDialog();
	dlg->setWindowTitle("模拟Index调试");
	dlg->setLayout(layout);
	t_cali->updateUI();
	dlg->show();
}

void h2Widget::on_PB_indexSimuDebug_RH_clicked()
{

}

/*
清除layout中的所有控件
*/
void clearLayout(QLayout* layout)
{
	if (!layout) return;

	QLayoutItem* item;
	while ((item = layout->takeAt(0)))
	{
		if (QWidget* widget = item->widget())
		{
			widget->setParent(nullptr); // 断开父子关系
			widget->deleteLater();      // 安排稍后删除
		}
		else if (QLayout* childLayout = item->layout())
		{
			clearLayout(childLayout);   // 递归清除子布局
			delete childLayout;         // 删除子布局本身
		}
		delete item;                    // 删除布局项
	}
}


void h2Widget::initUI()
{
	ui->TW_main->setCurrentWidget(ui->TW_main->widget(0)); // set first tab as default
	CarbonUIClass::applyScrollAreaStyle(ui->scrollArea);
	CarbonUIClass::applyScrollAreaStyle(ui->scrollArea_2);

	CarbonUIClass::setupTabWidgetStyle(ui->TW_main);
	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_FR, 0, true);


	WIG_fingerAdjustFreeMode_LH = new h2FingerAdjustFreeModeWidget(this);
	WIG_fingerAdjustFreeMode_LH->setObjectName(QStringLiteral("WIG_fingerAdjustFreeMode_LH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustFreeMode_LH);
	WIG_fingerAdjustFreeMode_RH = new h2FingerAdjustFreeModeWidget(this);
	WIG_fingerAdjustFreeMode_RH->setObjectName(QStringLiteral("WIG_fingerAdjustFreeMode_RH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustFreeMode_RH);

	WIG_fingerAdjustLockYaw_LH = new h2FingerAdjustLockYawWidget(this);
	WIG_fingerAdjustLockYaw_LH->setObjectName(QStringLiteral("WIG_fingerAdjustLockYaw_LH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustLockYaw_LH);
	WIG_fingerAdjustLockYaw_RH = new h2FingerAdjustLockYawWidget(this);
	WIG_fingerAdjustLockYaw_RH->setObjectName(QStringLiteral("WIG_fingerAdjustLockYaw_RH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustLockYaw_RH);

	WIG_fingerAdjustEnhanced_LH = new h2FingerAdjustEnhancedWidget(this);
	WIG_fingerAdjustEnhanced_LH->setObjectName(QStringLiteral("WIG_fingerAdjustEnhanced_LH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustEnhanced_LH);
	WIG_fingerAdjustEnhanced_RH = new h2FingerAdjustEnhancedWidget(this);
	WIG_fingerAdjustEnhanced_RH->setObjectName(QStringLiteral("WIG_fingerAdjustEnhanced_RH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustEnhanced_RH);

	WIG_fingerAdjustFreeMode_LH->setVisible(false);
	WIG_fingerAdjustFreeMode_RH->setVisible(false);
	WIG_fingerAdjustLockYaw_LH->setVisible(false);
	WIG_fingerAdjustLockYaw_RH->setVisible(false);
	WIG_fingerAdjustEnhanced_LH->setVisible(false);
	WIG_fingerAdjustEnhanced_RH->setVisible(false);

	WIG_h2SkeletonTree = new h2SkeletonWidget(this);
	WIG_h2SkeletonTree->setObjectName(QStringLiteral("WIG_h2SkeletonTree"));
	ui->VL_skeleton->addWidget(WIG_h2SkeletonTree);

#if 0
	WIG_Joystick_LH = new h2JoystickWidget(ui->TBL_JoyStick);
	WIG_Joystick_LH->setObjectName(QStringLiteral("WIG_Joystick_LH"));
	ui->HL_JoyStick_L->addWidget(WIG_Joystick_LH);
	WIG_Joystick_RH = new h2JoystickWidget(ui->TBL_JoyStick);
	WIG_Joystick_RH->setObjectName(QStringLiteral("WIG_Joystick_RH"));
	ui->HL_JoyStick_R->addWidget(WIG_Joystick_RH);

	WIG_jsCali_LH = new h2JoystickCali(ui->TBL_jsCali);
	WIG_jsCali_LH->setObjectName(QStringLiteral("WIG_jsCali_LH"));
	ui->HL_jsCali_L->addWidget(WIG_jsCali_LH);
	WIG_jsCali_RH = new h2JoystickCali(ui->TBL_jsCali);
	WIG_jsCali_RH->setObjectName(QStringLiteral("WIG_jsCali_RH"));
	ui->HL_jsCali_R->addWidget(WIG_jsCali_RH);
#endif


	WIG_h2GestureDisplay_LH = new h2GestureDisplay(ui->SA_gesture);
	WIG_h2GestureDisplay_LH->setObjectName(QStringLiteral("WIG_h2GestureDisplay_LH"));
	ui->HL_gloveGestureDisplay->addWidget(WIG_h2GestureDisplay_LH);
	WIG_h2GestureDisplay_RH = new h2GestureDisplay(ui->SA_gesture);
	WIG_h2GestureDisplay_RH->setObjectName(QStringLiteral("WIG_h2GestureDisplay_RH"));
	ui->HL_gloveGestureDisplay->addWidget(WIG_h2GestureDisplay_RH);

#ifdef _WIN32
	if (m_isSteamVRMonitorEnabled)
	{
		WIG_steamVRMonitor = new h2SteamVRMonitorWidget();
		WIG_steamVRMonitor->setObjectName(QStringLiteral("WIG_steamVRMonitor"));
		ui->HL_steamVRMonitor->addWidget(WIG_steamVRMonitor);
	}
#else

#endif 


	ui->PB_openUnityPort->setVisible(false);
	ui->PB_closeUnityPort->setVisible(false);
	ui->PB_unityWindowConfig->setVisible(false);

	

	CarbonUIClass::applyRadioStyle(ui->RB_freeMode, CarbonUIClass::Radio_LeftRound, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_lockMode, CarbonUIClass::Radio_Rectangle, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_mixMode, CarbonUIClass::Radio_RightRound, 64);
	// Create a button group for capture mode radio buttons so they are mutually exclusive
	QButtonGroup* capModeGroup = new QButtonGroup(this);
	// If the UI has these radio buttons, add them to the group with explicit ids
	if (ui->RB_freeMode) capModeGroup->addButton(ui->RB_freeMode, 0);
	if (ui->RB_lockMode) capModeGroup->addButton(ui->RB_lockMode, 1);
	if (ui->RB_mixMode) capModeGroup->addButton(ui->RB_mixMode, 2);
	capModeGroup->setExclusive(true);

	// Connect group button click to the existing slot
	QObject::connect(capModeGroup, SIGNAL(buttonClicked(int)), this, SLOT(onGloveCaptureModeChanged(int)));

}

void h2Widget::readGloveConfigFileToUI(void)
{
	/*加载配置文件中的配置*/
	h2GloveMgr newX2Suit;
	//m_x2SuitMgr = ((x2SuitMgr *)(MCAPP::Ins()->g_x2SuitMgr));
	bool isFind = H2_SDK_getCurrentX2SuitBySn(m_sn, newX2Suit);
	if (isFind)
	{
		/*仅双手*/
		//ui->CBX_gloveCapMode->setCurrentIndex(newX2Suit.GloveMocapMode);
		switch (newX2Suit.GloveMocapMode)
		{
			case h2GloveMode_Free:
				ui->RB_freeMode->setChecked(true);
				break;
			case h2GloveMode_YawLocked:
				ui->RB_lockMode->setChecked(true);
				break;
			case h2GloveMode_Enhanced:
				ui->RB_mixMode->setChecked(true);
				break;
			default:
				break;
		}
		onGloveCaptureModeChanged(newX2Suit.GloveMocapMode);

		//左手手指附加角度
		float valInfile_thumb_LH = newX2Suit.angleAdditionEuler[KHS32_LeftHandThumb1][1];
		{
			int val = (int)std::round(valInfile_thumb_LH + 100.0);

		}
		float valInfile_index_LH = newX2Suit.angleAdditionEuler[KHS32_LeftHandIndex1][1];
		{
			int val = (int)std::round(valInfile_index_LH + 100.0);

		}
		std::string defaultIP = newX2Suit.PopoutUnityWindowIP;
		int defaultPort = newX2Suit.PopoutUnityWindowPort;
		H2_SDK_addRemoteAddrPort(defaultIP, defaultPort);

	}
}

void h2Widget::updateUIFromGloveOnlineStatus(bool isLeftHandOnline, bool isRightHandOnline)
{
	//enable disable ui
	if (isLeftHandOnline)
	{
		if (WIG_h2GestureDisplay_LH->isEnabled())
			printf(" ");
		else
			WIG_h2GestureDisplay_LH->setEnabled(true);


		bool isLeftJsExist = false;
		bool isRightJsExist = false;
		H2_SDK_getJoystickExist(m_sn, isLeftJsExist, isRightJsExist);
		if (isLeftJsExist)
		{
			if (!m_leftJoystickVisible)
			{
				setLeftJoystickWidgetVisible(true);
				((h2JoystickWidget*)WIG_Joystick_LH)->setIsJoyStickExist(true);
				m_leftJoystickVisible = true;
			}

			// Only change layout if visibility state changed
			if (!m_leftJsCaliVisible)
			{
				setLeftJoystickCaliWidgetVisible(true);
				m_leftJsCaliVisible = true;
			}
		}
		else
		{
			if (m_leftJoystickVisible)
			{
				setLeftJoystickWidgetVisible(false);
				((h2JoystickWidget*)WIG_Joystick_LH)->setIsJoyStickExist(false);
				m_leftJoystickVisible = false;
			}

			// Only change layout if visibility state changed
			if (m_leftJsCaliVisible)
			{
				setLeftJoystickCaliWidgetVisible(false);
				m_leftJsCaliVisible = false;
			}
		}



		if (WIG_fingerAdjustLockYaw_LH->isVisible())
		{
			if (((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->getIsEnable())
				printf(" ");
			else
				((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->setIsEnable(true);
		}

		if (WIG_fingerAdjustEnhanced_LH->isVisible())
		{
			if (((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->getIsEnable())
				printf(" ");
			else
				((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->setIsEnable(true);
		}

		if (WIG_fingerAdjustFreeMode_LH->isVisible())
		{
			if (((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->getIsEnable())
				printf(" ");
			else
				((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->setIsEnable(true);
		}
	}
	else
	{
		if (WIG_h2GestureDisplay_LH->isEnabled())
			WIG_h2GestureDisplay_LH->setEnabled(false);
		else
			printf(" ");

		if (m_leftJoystickVisible)
		{
			setLeftJoystickWidgetVisible(false);
			if (WIG_Joystick_LH != 0)
			{
				((h2JoystickWidget*)WIG_Joystick_LH)->setIsJoyStickExist(false);
			}
			m_leftJoystickVisible = false;
		}
		// Only change layout if visibility state changed
		if (m_leftJsCaliVisible)
		{
			setLeftJoystickCaliWidgetVisible(false);
			m_leftJsCaliVisible = false;
		}

		if (WIG_fingerAdjustLockYaw_LH->isVisible())
		{
			if (((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->getIsEnable())
				((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->setIsEnable(false);
			else
				printf(" ");
		}

		if (WIG_fingerAdjustEnhanced_LH->isVisible())
		{
			if (((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->getIsEnable())
				((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->setIsEnable(false);
			else
				printf(" ");
		}

		if (WIG_fingerAdjustFreeMode_LH->isVisible())
		{
			if (((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->getIsEnable())
				((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->setIsEnable(false);
			else
				printf(" ");
		}
	}

	if (isRightHandOnline)
	{
		if (WIG_h2GestureDisplay_RH->isEnabled())
			printf(" ");
		else
			WIG_h2GestureDisplay_RH->setEnabled(true);
		//ui.GBX_joyKey_RH->isEnabled() ? printf("") : ui.GBX_joyKey_RH->setEnabled(true);
		//ui.GBX_joyCali_RH->isEnabled() ? printf("") : ui.GBX_joyCali_RH->setEnabled(true);
		bool isLeftJsExist = false;
		bool isRightJsExist = false;
		H2_SDK_getJoystickExist(m_sn, isLeftJsExist, isRightJsExist);
		if (isRightJsExist)
		{
			//((h2JoystickWidget*)WIG_Joystick_RH)->getIsEnable() ? printf("") : ((h2JoystickWidget*)WIG_Joystick_RH)->setIsEnable(true);
			//((h2JoystickWidget*)WIG_Joystick_RH)->setIsJoyStickExist(true);

			if (!m_rightJoystickVisible)
			{
				setRightJoystickWidgetVisible(true);
				m_rightJoystickVisible = true;
			}

			if (!m_rightJsCaliVisible)
			{
				setRightJoystickCaliWidgetVisible(true);
				m_rightJsCaliVisible = true;
			}
		}
		else
		{
			//((h2JoystickWidget*)WIG_Joystick_RH)->getIsEnable() ? printf("") : ((h2JoystickWidget*)WIG_Joystick_RH)->setIsEnable(true);
			//((h2JoystickWidget*)WIG_Joystick_RH)->setIsJoyStickExist(false);
			if (m_rightJoystickVisible)
			{
				setRightJoystickWidgetVisible(false);
				m_rightJoystickVisible = false;
			}

			if (m_rightJsCaliVisible)
			{
				setRightJoystickCaliWidgetVisible(false);
				m_rightJsCaliVisible = false;
			}
		}

		if (WIG_fingerAdjustLockYaw_RH->isVisible())
		{
			if (((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->getIsEnable())

				printf(" ");
			else
				((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->setIsEnable(true);
		}

		if (WIG_fingerAdjustEnhanced_RH->isVisible())
		{
			if (((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->getIsEnable())
				printf(" ");
			else
				((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->setIsEnable(true);
		}

		if (WIG_fingerAdjustFreeMode_RH->isVisible())
		{
			if (((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->getIsEnable())
				printf(" ");
			else
				((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->setIsEnable(true);
		}
	}
	else
	{
		if (WIG_h2GestureDisplay_RH->isEnabled())
			WIG_h2GestureDisplay_RH->setEnabled(false);
		else
			printf(" ");

		//((h2JoystickWidget*)WIG_Joystick_RH)->getIsEnable() ? ((h2JoystickWidget*)WIG_Joystick_RH)->setIsEnable(false) : printf("");
		//((h2JoystickWidget*)WIG_Joystick_RH)->setIsJoyStickExist(false);
		if (m_rightJoystickVisible)
		{
			setRightJoystickWidgetVisible(false);
			m_rightJoystickVisible = false;
		}
		// Only change layout if visibility state changed
		if (m_rightJsCaliVisible)
		{
			setRightJoystickCaliWidgetVisible(false);
			m_rightJsCaliVisible = false;
		}

		if (WIG_fingerAdjustLockYaw_RH->isVisible())
		{
			if (((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->getIsEnable())
				((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->setIsEnable(false);
			else
				printf(" ");
		}

		if (WIG_fingerAdjustEnhanced_RH->isVisible())
		{
			if (((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->getIsEnable())
				((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->setIsEnable(false);
			else
				printf(" ");
		}

		if (WIG_fingerAdjustFreeMode_RH->isVisible())
		{
			if (((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->getIsEnable())
				((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->setIsEnable(false);
			else
				printf(" ");
		}
	}
}

std::string h2Widget::GetLocalDllDirectory()
{
	std::string ret;


#ifdef _WIN32
	// 创建一个足够大的字符数组来保存路径
	char dllPath[MAX_PATH] = { 0 };
	HMODULE hModule = GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		if (GetModuleFileNameA(hModule, dllPath, sizeof(dllPath)))
		{
			std::string ws(dllPath);
			size_t pos = ws.find_last_of("\\");

			if (pos != std::string::npos)
			{
				// Extract the directory part of the path
				ws = ws.substr(0, pos);
			}
			ret = ws;
			//std::cout << "DLL Path: " << dllPath << std::endl;
		}
		else
		{
			//std::cerr << "Failed to get DLL path." << std::endl;
		}
	}
	else
	{
		//std::cerr << "Failed to get module handle." << std::endl;
	}
#else

#endif

	return ret;
}

void h2Widget::setLeftJoystickCaliWidgetVisible(bool isVisible)
{
#if 0
	if (isVisible)
	{
		// Remove any existing local placeholder(s) of type h2Notusable from the layout
		QLayout* lay = ui->HL_jsCali_L;
		for (int i = lay->count() - 1; i >= 0; --i)
		{
			QLayoutItem* it = lay->itemAt(i);
			if (!it) continue;
			QWidget* w = it->widget();
			if (w && qobject_cast<h2Notusable*>(w))
			{
				lay->removeWidget(w);
				w->deleteLater();
			}
		}

		if (WIG_jsCali_LH && WIG_jsCali_LH->parent() == nullptr)
		{
			ui->HL_jsCali_L->insertWidget(0, WIG_jsCali_LH);
		}
		if (WIG_jsCali_LH)
		{
			WIG_jsCali_LH->setEnabled(true);
			WIG_jsCali_LH->show();
		}
	}
	else
	{

		// If calibration widget currently in the layout, remove and hide it
		QLayout* lay = ui->HL_jsCali_L;
		for (int i = lay->count() - 1; i >= 0; --i)
		{
			QLayoutItem* it = lay->itemAt(i);
			if (!it) continue;
			QWidget* w = it->widget();
			if (w == WIG_jsCali_LH)
			{
				lay->removeWidget(w);
				WIG_jsCali_LH->hide();
				WIG_jsCali_LH->setParent(nullptr);
				break;
			}
		}

		// Create a local placeholder and insert it into the layout
		h2Notusable* placeholder = new h2Notusable();
		ui->HL_jsCali_L->insertWidget(0, placeholder);
		placeholder->setVisible(true);
	}
#endif

}

void h2Widget::setRightJoystickCaliWidgetVisible(bool isVisible)
{
#if 0
	if (isVisible)
	{
		// Remove any existing local placeholder(s) of type h2Notusable from the layout
		QLayout* lay = ui->HL_jsCali_R;
		for (int i = lay->count() - 1; i >= 0; --i)
		{
			QLayoutItem* it = lay->itemAt(i);
			if (!it) continue;
			QWidget* w = it->widget();
			if (w && qobject_cast<h2Notusable*>(w))
			{
				lay->removeWidget(w);
				w->deleteLater();
			}
		}

		if (WIG_jsCali_RH && WIG_jsCali_RH->parent() == nullptr)
		{
			ui->HL_jsCali_R->addWidget(WIG_jsCali_RH);
		}
		if (WIG_jsCali_RH)
		{
			WIG_jsCali_RH->setEnabled(true);
			WIG_jsCali_RH->show();
		}
	}
	else
	{
		// If calibration widget currently in the layout, remove and hide it
		QLayout* lay = ui->HL_jsCali_R;
		for (int i = lay->count() - 1; i >= 0; --i)
		{
			QLayoutItem* it = lay->itemAt(i);
			if (!it) continue;
			QWidget* w = it->widget();
			if (w == WIG_jsCali_RH)
			{
				lay->removeWidget(w);
				WIG_jsCali_RH->hide();
				WIG_jsCali_RH->setParent(nullptr);
				break;
			}
		}

		// Create a local placeholder and insert it into the layout
		h2Notusable* placeholder = new h2Notusable();
		ui->HL_jsCali_R->addWidget(placeholder);
		placeholder->setVisible(true);
	}

#endif

}


void h2Widget::setLeftJoystickWidgetVisible(bool isVisible)
{
#if 0
	if (isVisible)
	{
		// remove any existing h2Notusable placeholders from left joystick layout
		QLayout* lay = ui->HL_JoyStick_L;
		for (int i = lay->count() - 1; i >= 0; --i)
		{
			QLayoutItem* it = lay->itemAt(i);
			if (!it) continue;
			QWidget* w = it->widget();
			if (w && qobject_cast<h2Notusable*>(w))
			{
				lay->removeWidget(w);
				w->deleteLater();
			}
		}
		if (WIG_Joystick_LH && WIG_Joystick_LH->parent() == nullptr)
		{
			ui->HL_JoyStick_L->addWidget(WIG_Joystick_LH);
		}
		if (WIG_Joystick_LH)
		{
			WIG_Joystick_LH->setEnabled(true);
			WIG_Joystick_LH->show();
		}
	}
	else
	{
		QLayout* lay = ui->HL_JoyStick_L;
		for (int i = lay->count() - 1; i >= 0; --i)
		{
			QLayoutItem* it = lay->itemAt(i);
			if (!it) continue;
			QWidget* w = it->widget();
			if (w == WIG_Joystick_LH)
			{
				lay->removeWidget(w);
				WIG_Joystick_LH->hide();
				WIG_Joystick_LH->setParent(nullptr);
				break;
			}
		}
		h2Notusable* placeholder = new h2Notusable();
		ui->HL_JoyStick_L->addWidget(placeholder);
		placeholder->setVisible(true);
	}
#endif

}

void h2Widget::setRightJoystickWidgetVisible(bool isVisible)
{
#if 0
	if (isVisible)
	{
		// remove any existing h2Notusable placeholders from right joystick layout
		QLayout* lay = ui->HL_JoyStick_R;
		for (int i = lay->count() - 1; i >= 0; --i)
		{
			QLayoutItem* it = lay->itemAt(i);
			if (!it) continue;
			QWidget* w = it->widget();
			if (w && qobject_cast<h2Notusable*>(w))
			{
				lay->removeWidget(w);
				w->deleteLater();
			}
		}
		if (WIG_Joystick_RH && WIG_Joystick_RH->parent() == nullptr)
		{
			ui->HL_JoyStick_R->addWidget(WIG_Joystick_RH);
		}
		if (WIG_Joystick_RH)
		{
			WIG_Joystick_RH->setEnabled(true);
			WIG_Joystick_RH->show();
		}
	}
	else
	{
		QLayout* lay = ui->HL_JoyStick_R;
		for (int i = lay->count() - 1; i >= 0; --i)
		{
			QLayoutItem* it = lay->itemAt(i);
			if (!it) continue;
			QWidget* w = it->widget();
			if (w == WIG_Joystick_RH)
			{
				lay->removeWidget(w);
				WIG_Joystick_RH->hide();
				WIG_Joystick_RH->setParent(nullptr);
				break;
			}
		}
		h2Notusable* placeholder = new h2Notusable();
		ui->HL_JoyStick_R->addWidget(placeholder);
		placeholder->setVisible(true);
	}
#endif

}


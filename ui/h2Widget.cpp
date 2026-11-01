/**
 * @file  h2Widget.cpp
 * @brief 主界面控件 h2Widget 的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 控件样式统一由 CarbonUIClass 提供；通过 MotionGloveLLSDK 读写设备配置；依赖 uic 生成的 ui_*.h。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

#pragma execution_character_set("utf-8")

#include <QtGui/QWindow>
#include <QtCore/QDebug>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
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
#include "ui/h2CsvRecord.h"
#include "ui/gesture/h2GestureDisplay.h"
#include "../inc/h2GloveSepMgr.h"
#include <QButtonGroup>

#include "CarbonUIClass.h"
#include "groupboxcollapser.h"

#ifdef _WIN32
#include "libH2SteamDriver.h"

#else

#endif

namespace
{
/// 全页面统一的滚动条宽度，单位逻辑像素；「设置」页的 QScrollArea 与「骨骼」页的树必须同宽。
constexpr int kScrollBarWidth = 10;

void setStyledRadioText(QRadioButton* radioButton, const QString& text)
{
	if (!radioButton)
	{
		return;
	}

	radioButton->setText("");
	QLabel* centerLabel = radioButton->findChild<QLabel*>(
		radioButton->objectName() + QStringLiteral("_label"), Qt::FindDirectChildrenOnly);
	if (centerLabel)
	{
		centerLabel->setText(text);
	}
	else
	{
		radioButton->setText(text);
	}
	if (radioButton->style())
	{
		radioButton->style()->unpolish(radioButton);
		radioButton->style()->polish(radioButton);
	}
	radioButton->update();
	if (centerLabel)
	{
		centerLabel->update();
	}
}
}


h2Widget::h2Widget(QWidget* parent)
	: QWidget(parent)
{
	ui = new Ui::h2Widget();
	ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground, true);
	// set background and a 1px border with color #212121
	//setStyleSheet("background-color: #3b3b3b; border-top: 1px solid #212121; border-left: 1px solid #212121; border-bottom: 1px solid #212121; border-right: none;");

	// Update tab page titles based on language
	retranslateUI();

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
	if (WIG_h2CsvRecord != 0)
	{
		((h2CsvRecord*)WIG_h2CsvRecord)->setSn(m_sn);
	}
	//m_liveOrReplayIndex = liveOrReplayIndex;
	//只有第一套可以连接openglove
	int globalIndex = 0;
	H2_SDK_getCreateIndexGlobal(m_sn, &globalIndex);
	if (globalIndex == 0)
	{
		//H2_SDK_setOpenGloveEnable(m_sn, true, true);
	}
	else
	{
		//H2_SDK_setOpenGloveEnable(m_sn, false, false);
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

void h2Widget::applyMocapModeWidgetVisibility(h2GloveMode_TypeDef mode)
{
	WIG_fingerAdjustFreeMode_LH->setVisible(false);
	WIG_fingerAdjustFreeMode_RH->setVisible(false);
	WIG_fingerAdjustLockYaw_LH->setVisible(false);
	WIG_fingerAdjustLockYaw_RH->setVisible(false);
	WIG_fingerAdjustEnhanced_LH->setVisible(false);
	WIG_fingerAdjustEnhanced_RH->setVisible(false);

	switch (mode)
	{
		case h2GloveMode_Free:
			WIG_fingerAdjustFreeMode_LH->setVisible(true);
			WIG_fingerAdjustFreeMode_RH->setVisible(true);
			break;
		case h2GloveMode_YawLocked:
			WIG_fingerAdjustLockYaw_LH->setVisible(true);
			WIG_fingerAdjustLockYaw_RH->setVisible(true);
			break;
		case h2GloveMode_Enhanced:
			WIG_fingerAdjustEnhanced_LH->setVisible(true);
			WIG_fingerAdjustEnhanced_RH->setVisible(true);
			break;
		default:
			break;
	}
}

void h2Widget::onGloveCaptureModeChanged(int index)
{
#if 1
	printf("onGloveCaptureModeChanged:%d\r\n", index);
	m_MocapGloveMode = (h2GloveMode_TypeDef)index;

	// 未绑定设备时仍要切换可见性：软件刚打开、还没有设备时点击模式按钮，界面同样要跟着变。
	const bool hasDevice = (m_sn != 0);
	if (!hasDevice)
	{
		applyMocapModeWidgetVisibility(m_MocapGloveMode);
		return;
	}

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

	// 可见性统一由 applyMocapModeWidgetVisibility() 处理（它也可在未绑定设备时安全调用），
	// 这里只补上"显示出来的那组要读配置 + 通知 SDK"。
	applyMocapModeWidgetVisibility(m_MocapGloveMode);
	switch (m_MocapGloveMode)
	{
		case h2GloveMode_Free:
			((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_LH)->readGloveConfigFileToUI();
			((h2FingerAdjustFreeModeWidget*)WIG_fingerAdjustFreeMode_RH)->readGloveConfigFileToUI();
			H2_SDK_setMode(m_sn, m_MocapGloveMode);
			break;
		case h2GloveMode_YawLocked:
			((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_LH)->readGloveConfigFileToUI();
			((h2FingerAdjustLockYawWidget*)WIG_fingerAdjustLockYaw_RH)->readGloveConfigFileToUI();
			H2_SDK_setMode(m_sn, m_MocapGloveMode);
			break;
		case h2GloveMode_Enhanced:
			((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_LH)->readGloveConfigFileToUI();
			((h2FingerAdjustEnhancedWidget*)WIG_fingerAdjustEnhanced_RH)->readGloveConfigFileToUI();
			H2_SDK_setMode(m_sn, m_MocapGloveMode);
			break;
		default:
			break;
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

	ui->LBL_leftHandFR->setText(QString::number(leftHandFrameRate) + tr(" fps / fps"));
	ui->LBL_rightHandFR->setText(QString::number(rightHandFrameRate) + tr(" fps / fps"));

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
	dlg->setWindowTitle(tr("模拟Index调试 / Index Simulation Debug"));
	dlg->setLayout(layout);
	t_cali->updateUI();
	dlg->show();
}

void h2Widget::on_PB_indexSimuDebug_RH_clicked()
{

}

/**
 * @brief 移除布局中的所有子项并释放对应控件。
 * @param[in] layout 目标布局，**可为 nullptr**；为空时直接返回。
 * @note 递归清理嵌套布局；被移除的控件会被 delete，调用方不要继续持有指针。
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
	// 左右手是控件的静态属性、与是否绑定设备无关：必须在这里就设好，否则软件刚打开
	// （setSNAndAN() 尚未调用）两块面板的标题都是默认的"左手"。
	static_cast<h2FingerAdjustFreeModeWidget*>(WIG_fingerAdjustFreeMode_LH)->setIsLeftHand(true);
	static_cast<h2FingerAdjustFreeModeWidget*>(WIG_fingerAdjustFreeMode_RH)->setIsRightHand(true);

	WIG_fingerAdjustLockYaw_LH = new h2FingerAdjustLockYawWidget(this);
	WIG_fingerAdjustLockYaw_LH->setObjectName(QStringLiteral("WIG_fingerAdjustLockYaw_LH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustLockYaw_LH);
	WIG_fingerAdjustLockYaw_RH = new h2FingerAdjustLockYawWidget(this);
	WIG_fingerAdjustLockYaw_RH->setObjectName(QStringLiteral("WIG_fingerAdjustLockYaw_RH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustLockYaw_RH);
	static_cast<h2FingerAdjustLockYawWidget*>(WIG_fingerAdjustLockYaw_LH)->setIsLeftHand(true);
	static_cast<h2FingerAdjustLockYawWidget*>(WIG_fingerAdjustLockYaw_RH)->setIsRightHand(true);

	WIG_fingerAdjustEnhanced_LH = new h2FingerAdjustEnhancedWidget(this);
	WIG_fingerAdjustEnhanced_LH->setObjectName(QStringLiteral("WIG_fingerAdjustEnhanced_LH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustEnhanced_LH);
	WIG_fingerAdjustEnhanced_RH = new h2FingerAdjustEnhancedWidget(this);
	WIG_fingerAdjustEnhanced_RH->setObjectName(QStringLiteral("WIG_fingerAdjustEnhanced_RH"));
	ui->VL_gloveModeConfig->addWidget(WIG_fingerAdjustEnhanced_RH);
	static_cast<h2FingerAdjustEnhancedWidget*>(WIG_fingerAdjustEnhanced_LH)->setIsLeftHand(true);
	static_cast<h2FingerAdjustEnhancedWidget*>(WIG_fingerAdjustEnhanced_RH)->setIsRightHand(true);

	// 三组模式控件互斥显示：默认（m_MocapGloveMode = h2GloveMode_Free）直接把自由模式那组显示出来。
	// 不能只写 setVisible(false)——软件刚打开时没有设备、readGloveConfigFileToUI() 的配置查询也会失败，
	// 若此处全部隐藏就没有任何代码再把它们显示出来，界面会是一片空白，必须先点一次「自由模式」。
	applyMocapModeWidgetVisibility(m_MocapGloveMode);

	WIG_h2SkeletonTree = new h2SkeletonWidget(this);
	WIG_h2SkeletonTree->setObjectName(QStringLiteral("WIG_h2SkeletonTree"));
	ui->VL_skeleton->addWidget(WIG_h2SkeletonTree);

	// 骨骼透明度滑块：默认 100%，拖动时通知主界面刷新 3D 视图
	if (ui->SL_skeletonOpacity)
	{
		QObject::connect(ui->SL_skeletonOpacity, &QSlider::valueChanged,
			this, &h2Widget::onSkeletonOpacitySliderMoved);
		refreshSkeletonOpacityValueText();
	}

	// 关节点透明度滑块：与骨骼段是两条独立通道，默认 100%
	if (ui->SL_jointOpacity)
	{
		QObject::connect(ui->SL_jointOpacity, &QSlider::valueChanged,
			this, &h2Widget::onJointOpacitySliderMoved);
		refreshJointOpacityValueText();
	}

	// 骨骼连线（八面锥）：默认不勾选，勾选后才在 3D 视图里绘制
	if (ui->CB_boneLines)
	{
		QObject::connect(ui->CB_boneLines, &QCheckBox::toggled,
			this, &h2Widget::onBoneLinesToggled);
	}
	if (ui->SL_boneLineOpacity)
	{
		QObject::connect(ui->SL_boneLineOpacity, &QSlider::valueChanged,
			this, &h2Widget::onBoneLineOpacitySliderMoved);
		refreshBoneLineOpacityValueText();
	}

	applySkeletonTabStyle();

	WIG_h2CsvRecord = new h2CsvRecord(this);
	WIG_h2CsvRecord->setObjectName(QStringLiteral("WIG_h2CsvRecord"));
	ui->VL_csv->addWidget(WIG_h2CsvRecord);

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
	

	CarbonUIClass::applyRadioStyle(ui->RB_freeMode, CarbonUIClass::Radio_LeftRound, 64+16);
	CarbonUIClass::applyRadioStyle(ui->RB_lockMode, CarbonUIClass::Radio_Rectangle, 64+16);
	CarbonUIClass::applyRadioStyle(ui->RB_mixMode, CarbonUIClass::Radio_RightRound, 64+16);
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


void h2Widget::setFingerWidgetsLanguage(const QString& lang)
{
	// Set language for all finger adjustment widgets
	if (WIG_fingerAdjustEnhanced_LH) {
		static_cast<h2FingerAdjustEnhancedWidget*>(WIG_fingerAdjustEnhanced_LH)->setLanguage(lang);
	}
	if (WIG_fingerAdjustEnhanced_RH) {
		static_cast<h2FingerAdjustEnhancedWidget*>(WIG_fingerAdjustEnhanced_RH)->setLanguage(lang);
	}
	if (WIG_fingerAdjustFreeMode_LH) {
		static_cast<h2FingerAdjustFreeModeWidget*>(WIG_fingerAdjustFreeMode_LH)->setLanguage(lang);
	}
	if (WIG_fingerAdjustFreeMode_RH) {
		static_cast<h2FingerAdjustFreeModeWidget*>(WIG_fingerAdjustFreeMode_RH)->setLanguage(lang);
	}
	if (WIG_fingerAdjustLockYaw_LH) {
		static_cast<h2FingerAdjustLockYawWidget*>(WIG_fingerAdjustLockYaw_LH)->setLanguage(lang);
	}
	if (WIG_fingerAdjustLockYaw_RH) {
		static_cast<h2FingerAdjustLockYawWidget*>(WIG_fingerAdjustLockYaw_RH)->setLanguage(lang);
	}
}

void h2Widget::setTabPageLanguage(const QString& lang)
{
	m_language = lang;
	if (WIG_h2CsvRecord) {
		static_cast<h2CsvRecord*>(WIG_h2CsvRecord)->setLanguage(lang);
	}
	retranslateUI();
}

void h2Widget::retranslateUI()
{
	if (!ui || !ui->TW_main) return;

	if (m_language == "en") {
		if (ui->GBX_FR) ui->GBX_FR->setTitle("Frame Rate");
		ui->TW_main->setTabText(0, "Settings");
		ui->TW_main->setTabText(1, "Skeleton");
		ui->TW_main->setTabText(2, "CSV");
		ui->TW_main->setTabText(3, "Gesture");

		setStyledRadioText(ui->RB_freeMode, "Free Mode");
		setStyledRadioText(ui->RB_lockMode, "Finger Lock");
		setStyledRadioText(ui->RB_mixMode, "Mix Mode");

		if (ui->LBL_skeletonOpacityTitle) ui->LBL_skeletonOpacityTitle->setText("Bone Opacity");
		if (ui->LBL_jointOpacityTitle) ui->LBL_jointOpacityTitle->setText("Joint Opacity");

		if (ui->GB_joints) ui->GB_joints->setTitle("Joints");
		if (ui->GB_bones) ui->GB_bones->setTitle("Bones");
		if (ui->GB_boneLinks) ui->GB_boneLinks->setTitle("Bone Links");
		if (ui->CB_boneLines) ui->CB_boneLines->setText("Draw bone links (octahedral)");
		if (ui->LBL_boneLineOpacityTitle)
			ui->LBL_boneLineOpacityTitle->setText("Link Opacity");
	} else {
		if (ui->GBX_FR) ui->GBX_FR->setTitle("帧率");
		ui->TW_main->setTabText(0, "设置");
		ui->TW_main->setTabText(1, "骨骼");
		ui->TW_main->setTabText(2, "CSV");
		ui->TW_main->setTabText(3, "手势识别");

		setStyledRadioText(ui->RB_freeMode, "自由模式");
		setStyledRadioText(ui->RB_lockMode, "分指锁定");
		setStyledRadioText(ui->RB_mixMode, "混合模式");

		if (ui->LBL_skeletonOpacityTitle) ui->LBL_skeletonOpacityTitle->setText("骨骼透明度");
		if (ui->LBL_jointOpacityTitle) ui->LBL_jointOpacityTitle->setText("关节点透明度");

		if (ui->GB_joints) ui->GB_joints->setTitle("关节点");
		if (ui->GB_bones) ui->GB_bones->setTitle("骨骼");
		if (ui->GB_boneLinks) ui->GB_boneLinks->setTitle("骨骼连线");
		if (ui->CB_boneLines) ui->CB_boneLines->setText("绘制八面锥骨骼");
		if (ui->LBL_boneLineOpacityTitle)
			ui->LBL_boneLineOpacityTitle->setText("连线透明度");
	}

	refreshSkeletonOpacityValueText();
	refreshJointOpacityValueText();
	refreshBoneLineOpacityValueText();
}

void h2Widget::refreshSkeletonOpacityValueText()
{
	if (!ui || !ui->SL_skeletonOpacity || !ui->LBL_skeletonOpacityValue) return;

	ui->LBL_skeletonOpacityValue->setText(QString::number(ui->SL_skeletonOpacity->value()) + "%");
}

int h2Widget::skeletonOpacityPercent() const
{
	// 滑块缺失时回退 100（完全不透明），与改动前的渲染结果保持一致
	if (!ui || !ui->SL_skeletonOpacity)
	{
		return 100;
	}

	return ui->SL_skeletonOpacity->value();
}

int h2Widget::jointOpacityPercent() const
{
	// 滑块缺失时回退 100（完全不透明），与改动前的渲染结果保持一致
	if (!ui || !ui->SL_jointOpacity)
	{
		return 100;
	}

	return ui->SL_jointOpacity->value();
}

void h2Widget::refreshJointOpacityValueText()
{
	if (!ui || !ui->SL_jointOpacity || !ui->LBL_jointOpacityValue) return;

	ui->LBL_jointOpacityValue->setText(QString::number(ui->SL_jointOpacity->value()) + "%");
}

void h2Widget::onSkeletonOpacitySliderMoved(int percent)
{
	refreshSkeletonOpacityValueText();
	emit skeletonOpacityChanged(percent);
}

void h2Widget::onJointOpacitySliderMoved(int percent)
{
	refreshJointOpacityValueText();
	emit jointOpacityChanged(percent);
}

bool h2Widget::boneLinesVisible() const
{
	// 复选框缺失时回退 false：默认不绘制八面锥，与 GLWidget 的默认状态一致
	if (!ui || !ui->CB_boneLines)
	{
		return false;
	}

	return ui->CB_boneLines->isChecked();
}

int h2Widget::boneLineOpacityPercent() const
{
	if (!ui || !ui->SL_boneLineOpacity)
	{
		return 100;
	}

	return ui->SL_boneLineOpacity->value();
}

void h2Widget::refreshBoneLineOpacityValueText()
{
	if (!ui || !ui->SL_boneLineOpacity || !ui->LBL_boneLineOpacityValue) return;

	ui->LBL_boneLineOpacityValue->setText(
		QString::number(ui->SL_boneLineOpacity->value()) + "%");
}

void h2Widget::onBoneLinesToggled(bool checked)
{
	emit boneLinesVisibleChanged(checked);
}

void h2Widget::onBoneLineOpacitySliderMoved(int percent)
{
	refreshBoneLineOpacityValueText();
	emit boneLineOpacityChanged(percent);
}

void h2Widget::applySkeletonTabStyle()
{
	// 该页背景是 #3b3b3b，默认灰字对比度不足，因此整页文字统一置白；
	// 滚动条宽度取统一的 kScrollBarWidth，与「设置」页的 QScrollArea 保持同粗。
	const QString style = QStringLiteral(
		"QWidget#tab_skeleton { background-color: #3b3b3b; }"
		"QWidget#tab_skeleton QLabel, QWidget#tab_skeleton QCheckBox,"
		" QWidget#tab_skeleton QGroupBox { color: #ffffff; }"
		"QWidget#tab_skeleton QGroupBox { border: 1px solid #4a4a4a; border-radius: 4px;"
		" margin-top: 9px; padding-top: 3px; }"
		"QWidget#tab_skeleton QGroupBox::title { subcontrol-origin: margin;"
		" subcontrol-position: top left; left: 8px; padding: 0 3px; color: #ffffff; }"
		"QWidget#tab_skeleton QSlider::groove:horizontal { height: 4px; background: #5a5a5a;"
		" border-radius: 2px; }"
		"QWidget#tab_skeleton QSlider::handle:horizontal { width: 12px; margin: -5px 0;"
		" border-radius: 6px; background: #c8c8c8; }"
		"QScrollBar:vertical { width: %1px; background: #2b2b2b; margin: 0; }"
		"QScrollBar::handle:vertical { background: #6a6a6a; min-height: 24px;"
		" border-radius: %2px; }"
		"QScrollBar:horizontal { height: %1px; background: #2b2b2b; margin: 0; }"
		"QScrollBar::handle:horizontal { background: #6a6a6a; min-width: 24px;"
		" border-radius: %2px; }"
		"QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }"
		"QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }")
		.arg(kScrollBarWidth)
		.arg(kScrollBarWidth / 2);

	// 作用域是整个 h2Widget：QScrollBar 是「设置」页 QScrollArea 与「骨骼」页树的公共子控件，
	// 只有统一在这里设置才能保证各页滚动条同宽。
	setStyleSheet(style);
}

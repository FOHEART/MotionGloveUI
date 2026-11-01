/**
 * @file  h2FingerAdjustLockYawWidget.cpp
 * @brief 手指角度调节（锁定偏航）面板控件的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；控件样式统一由 CarbonUIClass 提供；通过 MotionGloveLLSDK 读写设备配置；界面布局由同名 .ui 文件生成。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

//#include "stdafx.h"
#pragma execution_character_set("utf-8")
#include "h2FingerAdjustLockYawWidget.h"
#include "CarbonUIClass.h"
#include "groupboxcollapser.h"
#include <QLocale>

#ifdef _WIN32
#include <windows.h>

#else

#endif
#include "h2sdk.h"
#include <QtCore/QMetaObject>
#include <QtCore/QCoreApplication>

h2FingerAdjustLockYawWidget::h2FingerAdjustLockYawWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	CarbonUIClass::setWidgetLabelsStyleRecursively(this);
	// install event filter to block wheel events on the stretch slider
	ui.HS_figureAngleStretch->installEventFilter(this);
	ui.HS_figureAngleFist->installEventFilter(this);
	ui.HS_thumbFigureAngleX->installEventFilter(this);
	ui.HS_thumbFigureAngleY->installEventFilter(this);
	ui.HS_thumbFigureAngleZ->installEventFilter(this);

	CarbonUIClass::applyButtonStyle(ui.PB_rotResetLockYaw, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyButtonStyle(ui.PB_rotResetLockYaw_undo, CarbonUIClass::Shape_RoundAll);

	setIsEnable(false);

	{
		//std::string imageDir = GetLocalDllDirectory() + "\\Resources\\Images\\libh2UI";
		//QIcon icon(QString::fromStdString(imageDir) + "\\undo.png");

		//ui.PB_rotResetLockYaw_undo->setIcon(icon);
		ui.PB_rotResetLockYaw_undo->setText(QObject::tr("撤销"));
		ui.PB_rotResetLockYaw_undo->setToolTip(QObject::tr("撤销重置"));
	}
	//隐藏拇指x z的设置
	ui.HS_thumbFigureAngleX->hide();
	ui.LBL_thumbFigureAngleX->hide();
	//ui.HS_thumbFigureAngleY->hide();
	//ui.LBL_thumbFigureAngleY->hide();

	QObject::connect(ui.HS_figureAngleStretch, SIGNAL(valueChanged(int)), this, SLOT(onSliderFigureAngleStretchChanged(int)));
	QObject::connect(ui.HS_figureAngleStretch, SIGNAL(sliderReleased()), this, SLOT(onSliderFigureAngleStretchReleased()));
	QObject::connect(ui.HS_figureAngleFist, SIGNAL(valueChanged(int)), this, SLOT(onSliderFigureAngleFistChanged(int)));
	QObject::connect(ui.HS_figureAngleFist, SIGNAL(sliderReleased()), this, SLOT(onSliderFigureAngleFistReleased()));

	//---start 拇指的角度
	QObject::connect(ui.HS_thumbFigureAngleX, SIGNAL(valueChanged(int)), this, SLOT(onSliderThumbFigureAngleZChanged(int)));
	QObject::connect(ui.HS_thumbFigureAngleX, SIGNAL(sliderReleased()), this, SLOT(onSliderThumbFigureAngleZReleased()));

	QObject::connect(ui.HS_thumbFigureAngleY, SIGNAL(valueChanged(int)), this, SLOT(onSliderThumbFigureAngleZChanged(int)));
	QObject::connect(ui.HS_thumbFigureAngleY, SIGNAL(sliderReleased()), this, SLOT(onSliderThumbFigureAngleZReleased()));

	QObject::connect(ui.HS_thumbFigureAngleZ, SIGNAL(valueChanged(int)), this, SLOT(onSliderThumbFigureAngleZChanged(int)));
	QObject::connect(ui.HS_thumbFigureAngleZ, SIGNAL(sliderReleased()), this, SLOT(onSliderThumbFigureAngleZReleased()));

	setupLanguageChangeConnection();
	retranslateUI();
}

h2FingerAdjustLockYawWidget::~h2FingerAdjustLockYawWidget()
{
}

void h2FingerAdjustLockYawWidget::ensureCollapserInstalled()
{
	if (m_collapserInstalled)
	{
		return;
	}

	QGroupBoxCollapser::installIntoGroupBox(ui.GBX_fingerAngleLockYaw, 0, true);
	m_collapserInstalled = true;
}

void h2FingerAdjustLockYawWidget::setIsLeftHand(bool val)
{
	m_bIsLeftHand = val;
	m_bIsRightHand = !val;
	retranslateUI();
	ensureCollapserInstalled();

}

void h2FingerAdjustLockYawWidget::setIsRightHand(bool val)
{
	m_bIsRightHand = val;
	m_bIsLeftHand = !val;
	retranslateUI();
	
	ensureCollapserInstalled();

}

void h2FingerAdjustLockYawWidget::setIsEnable(bool val)
{
	m_bIsEnable = val;

	ui.GBX_fingerAngleLockYaw->setEnabled(val);
}

void h2FingerAdjustLockYawWidget::readGloveConfigFileToUI(void)
{
	h2GloveMgr newX2Suit;
	unsigned int sn = m_gloveSN;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(sn, newX2Suit);
	if (isFind)
	{
		if (m_bIsLeftHand)
		{
			/*指尖活动量*/
			ui.HS_figureAngleStretch->setValue(newX2Suit.LeftFingerAngleStretch);
			ui.HS_figureAngleFist->setValue(newX2Suit.LeftFingerAngleFist);

			//ui.HS_figureAngleStretch_R->setValue(newX2Suit.RightFingerAngleStretch);
				/*拇指*/
			ui.HS_thumbFigureAngleY->setValue(newX2Suit.angleAdditionEuler[KHS32_LeftHandThumb1][1]);
			ui.HS_thumbFigureAngleZ->setValue(newX2Suit.angleAdditionEuler[KHS32_LeftHandThumb1][2]);
		}
		else if (m_bIsRightHand)
		{
			/*指尖活动量*/
			ui.HS_figureAngleStretch->setValue(newX2Suit.RightFingerAngleStretch);
			ui.HS_figureAngleFist->setValue(newX2Suit.RightFingerAngleFist);

			//ui.HS_figureAngleStretch_R->setValue(newX2Suit.RightFingerAngleStretch);
				/*拇指*/
			ui.HS_thumbFigureAngleY->setValue(newX2Suit.angleAdditionEuler[KHS32_RightHandThumb1][1]);
			ui.HS_thumbFigureAngleZ->setValue(newX2Suit.angleAdditionEuler[KHS32_RightHandThumb1][2]);
		}
	}
}

void h2FingerAdjustLockYawWidget::setShowConfigItemHeader(bool isShow)
{
	m_bShowConfigItemHeader = isShow;

	ui.LBL_figureAngleStretch_hdr->setVisible(m_bShowConfigItemHeader);
	ui.LBL_figureAngleFist_hdr->setVisible(m_bShowConfigItemHeader);
	ui.LBL_thumbFigureAngle_hdr->setVisible(m_bShowConfigItemHeader);

}

void h2FingerAdjustLockYawWidget::on_PB_rotResetLockYaw_clicked()
{
	unsigned int sn = m_gloveSN;

	if (m_bIsLeftHand)
	{
		H2_SDK_resetLeftHandRot_MotionVenusStandalone(sn);
	}
	else if (m_bIsRightHand)
	{
		H2_SDK_resetRightHandRot_MotionVenusStandalone(sn);
	}
}
void h2FingerAdjustLockYawWidget::on_PB_rotResetLockYaw_undo_clicked()
{
	unsigned int sn = m_gloveSN;

	if (m_bIsLeftHand)
	{
		H2_SDK_resetLeftHandRot_undo_MotionVenusStandalone(sn);
	}
	else if (m_bIsRightHand)
	{
		H2_SDK_resetRightHandRot_undo_MotionVenusStandalone(sn);
	}
}

void h2FingerAdjustLockYawWidget::onSliderFigureAngleStretchChanged(int value)
{
	unsigned int sn = m_gloveSN;

	float currentValue = ((float)value);
	if (m_bIsLeftHand)
	{
		H2_SDK_setLeftFinger_yawLocked_Strench_Degree(sn, currentValue);
	}
	else if (m_bIsRightHand)
	{
		H2_SDK_setRightFinger_yawLocked_Strench_Degree(sn, currentValue);
	}

	ui.LBL_figureAngleStretch->setText(QString::number(currentValue) + QObject::tr("°"));
}

void h2FingerAdjustLockYawWidget::onSliderFigureAngleStretchReleased()
{
	unsigned int sn = m_gloveSN;

	//保存
	h2GloveMgr newX2Suit;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(sn, newX2Suit);
	if (isFind)
	{
		if (m_bIsLeftHand)
		{
			newX2Suit.LeftFingerAngleStretch =
				ui.HS_figureAngleStretch->value();
		}
		else if (m_bIsRightHand)
		{
			newX2Suit.RightFingerAngleStretch =
				ui.HS_figureAngleStretch->value();
		}
		H2_SDK_updateX2SuitMgrVec(sn, newX2Suit);
	}
	else
	{
		printf("");
	}
}

void h2FingerAdjustLockYawWidget::onSliderFigureAngleFistChanged(int value)
{
	unsigned int sn = m_gloveSN;

	float currentValue = ((float)value);
	if (m_bIsLeftHand)
	{
		H2_SDK_setLeftFinger_yawLocked_Fist_Degree(sn, currentValue);
	}
	else if (m_bIsRightHand)
	{
		H2_SDK_setRightFinger_yawLocked_Fist_Degree(sn, currentValue);
	}

	ui.LBL_figureAngleFist->setText(QString::number(currentValue) + QObject::tr("°"));

}

void h2FingerAdjustLockYawWidget::onSliderFigureAngleFistReleased()
{
	unsigned int sn = m_gloveSN;

	//保存
	h2GloveMgr newX2Suit;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(sn, newX2Suit);
	if (isFind)
	{
		if (m_bIsLeftHand)
		{
			newX2Suit.LeftFingerAngleFist =
				ui.HS_figureAngleFist->value();
		}
		else if (m_bIsRightHand)
		{
			newX2Suit.RightFingerAngleFist =
				ui.HS_figureAngleFist->value();
		}
		H2_SDK_updateX2SuitMgrVec(sn, newX2Suit);
	}
	else
	{
		printf("");
	}
}

void h2FingerAdjustLockYawWidget::onSliderThumbFigureAngleZChanged(int value)
{
	unsigned int m_sn = m_gloveSN;
	QSlider* slider = qobject_cast<QSlider*>(sender());
	if (slider == ui.HS_thumbFigureAngleZ)
	{
		ui.LBL_thumbFigureAngleZ->setText(QString::number(value) + QObject::tr("°"));
	}
	else if (slider == ui.HS_thumbFigureAngleY)
	{
		ui.LBL_thumbFigureAngleY->setText(QString::number(value) + QObject::tr("°"));
	}
	else if (slider == ui.HS_thumbFigureAngleX)
	{
		ui.LBL_thumbFigureAngleX->setText(QString::number(value) + QObject::tr("°"));
	}
	else
	{

	}


	int thumbAngle[3];
	thumbAngle[0] = ui.HS_thumbFigureAngleX->value();
	thumbAngle[1] = ui.HS_thumbFigureAngleY->value();
	thumbAngle[2] = ui.HS_thumbFigureAngleZ->value();


	if (m_bIsLeftHand)
	{
		H2_SDK_setLeftHandThumbGap_Degree(m_sn, 0, (float)thumbAngle[0]);
		H2_SDK_setLeftHandThumbGap_Degree(m_sn, 1, (float)thumbAngle[1]);
		H2_SDK_setLeftHandThumbGap_Degree(m_sn, 2, (float)thumbAngle[2]);

	}
	else if (m_bIsRightHand)
	{
		H2_SDK_setRightHandThumbGap_Degree(m_sn, 0, (float)thumbAngle[0]);
		H2_SDK_setRightHandThumbGap_Degree(m_sn, 1, (float)thumbAngle[1]);
		H2_SDK_setRightHandThumbGap_Degree(m_sn, 2, (float)thumbAngle[2]);

	}
}

void h2FingerAdjustLockYawWidget::onSliderThumbFigureAngleZReleased()
{
	unsigned int m_sn = m_gloveSN;
	//这里写入h2glove的序列号
	h2GloveMgr newX2Suit;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(m_sn, newX2Suit);
	if (isFind)
	{
		if (m_bIsLeftHand)
		{
			/*拇指*/
			newX2Suit.angleAdditionEuler[KHS32_LeftHandThumb1][0] =
				ui.HS_thumbFigureAngleX->value();
			newX2Suit.angleAdditionEuler[KHS32_LeftHandThumb1][1] =
				ui.HS_thumbFigureAngleY->value();
			newX2Suit.angleAdditionEuler[KHS32_LeftHandThumb1][2] =
				ui.HS_thumbFigureAngleZ->value();
		}
		else if (m_bIsRightHand)
		{
			/*拇指*/
			newX2Suit.angleAdditionEuler[KHS32_RightHandThumb1][0] =
				ui.HS_thumbFigureAngleX->value();
			newX2Suit.angleAdditionEuler[KHS32_RightHandThumb1][1] =
				ui.HS_thumbFigureAngleY->value();
			newX2Suit.angleAdditionEuler[KHS32_RightHandThumb1][2] =
				ui.HS_thumbFigureAngleZ->value();
		}

		H2_SDK_updateX2SuitMgrVec(m_sn, newX2Suit);
	}
	else
	{
		printf("");
	}
}

void h2FingerAdjustLockYawWidget::applyUIToConfig(h2GloveMgr& suit) const
{
	const int thumbIndex = m_bIsLeftHand ? KHS32_LeftHandThumb1 : KHS32_RightHandThumb1;
	if (!m_bIsLeftHand && !m_bIsRightHand)
	{
		return;
	}

	if (m_bIsLeftHand)
	{
		suit.LeftFingerAngleStretch = ui.HS_figureAngleStretch->value();
		suit.LeftFingerAngleFist = ui.HS_figureAngleFist->value();
	}
	else
	{
		suit.RightFingerAngleStretch = ui.HS_figureAngleStretch->value();
		suit.RightFingerAngleFist = ui.HS_figureAngleFist->value();
	}

	suit.angleAdditionEuler[thumbIndex][0] = ui.HS_thumbFigureAngleX->value();
	suit.angleAdditionEuler[thumbIndex][1] = ui.HS_thumbFigureAngleY->value();
	suit.angleAdditionEuler[thumbIndex][2] = ui.HS_thumbFigureAngleZ->value();
}

bool h2FingerAdjustLockYawWidget::eventFilter(QObject* obj, QEvent* ev)
{
	if (obj == ui.HS_figureAngleStretch && ev->type() == QEvent::Wheel)
	{
		// Ignore wheel events to prevent accidental changes by mouse wheel
		return true;
	}
	if (obj == ui.HS_figureAngleFist && ev->type() == QEvent::Wheel)
	{
		// Ignore wheel events to prevent accidental changes by mouse wheel
		return true;
	}
	if (obj == ui.HS_thumbFigureAngleX && ev->type() == QEvent::Wheel)
	{
		// Ignore wheel events to prevent accidental changes by mouse wheel
		return true;
	}
	if (obj == ui.HS_thumbFigureAngleY && ev->type() == QEvent::Wheel)
	{
		// Ignore wheel events to prevent accidental changes by mouse wheel
		return true;
	}
	if (obj == ui.HS_thumbFigureAngleZ && ev->type() == QEvent::Wheel)
	{
		// Ignore wheel events to prevent accidental changes by mouse wheel
		return true;
	}


	return QWidget::eventFilter(obj, ev);
}

std::string h2FingerAdjustLockYawWidget::GetLocalDllDirectory()
{
	std::string ret;
#ifdef _WIN32
	// 创建一个足够大的字符数组来保存路径
	char dllPath[260] = { 0 };
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

void h2FingerAdjustLockYawWidget::setupLanguageChangeConnection()
{
	for (int i = 0; i < QCoreApplication::instance()->children().size(); ++i)
	{
		QObject *obj = QCoreApplication::instance()->children()[i];
		if (obj && obj->inherits("LanguageManager"))
		{
			connect(obj, SIGNAL(languageChanged(QString)), this, SLOT(retranslateUI()));
			break;
		}
	}
}

void h2FingerAdjustLockYawWidget::retranslateUI()
{
	if (&ui)
	{
		QString lang = m_language;  // Use stored language instead of QMetaObject lookup
		QString handTitle;
		if (m_bIsRightHand)
		{
			handTitle = (lang == "en") ? "Right Hand" : "右手";
		}
		else
		{
			handTitle = (lang == "en") ? "Left Hand" : "左手";
		}

		if (lang == "en") {
			ui.GBX_fingerAngleLockYaw->setTitle(handTitle);
			ui.LBL_figureAngleStretch_hdr->setText("Finger Gap Angle");
			ui.LBL_figureAngleFist_hdr->setText("Fist Finger\nAngle");
			ui.LBL_thumbFigureAngle_hdr->setText("Thumb Angle");
			ui.PB_rotResetLockYaw->setText("Reset Hand Back Angle");
			ui.PB_rotResetLockYaw_undo->setText("Undo");
			ui.PB_rotResetLockYaw_undo->setToolTip("Undo Reset");
		} else {
			ui.GBX_fingerAngleLockYaw->setTitle(handTitle);
			ui.LBL_figureAngleStretch_hdr->setText("指间角度");
			ui.LBL_figureAngleFist_hdr->setText("握拳指尖\n角度");
			ui.LBL_thumbFigureAngle_hdr->setText("拇指角度");
			ui.PB_rotResetLockYaw->setText("重置手背角度");
			ui.PB_rotResetLockYaw_undo->setText("撤销");
			ui.PB_rotResetLockYaw_undo->setToolTip("撤销重置");
		}
	}
}

void h2FingerAdjustLockYawWidget::setLanguage(const QString& lang)
{
	m_language = lang;
	retranslateUI();
}

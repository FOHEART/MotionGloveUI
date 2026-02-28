#pragma execution_character_set("utf-8")

#include "h2FingerAdjustEnhancedWidget.h"
#include "carbonuiclass.h"
#include "groupboxcollapser.h"

#ifdef _WIN32
#include <windows.h>
#else

#endif

#include "h2sdk.h"

h2FingerAdjustEnhancedWidget::h2FingerAdjustEnhancedWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	CarbonUIClass::setWidgetLabelsStyleRecursively(this);
	// install event filter to block wheel events on the stretch slider
	ui.HS_figureAngleStretch->installEventFilter(this);
	ui.HS_thumbFigureAngleX->installEventFilter(this);
	ui.HS_thumbFigureAngleY->installEventFilter(this);
	ui.HS_thumbFigureAngleZ->installEventFilter(this);

	CarbonUIClass::applyButtonStyle(ui.PB_rotResetLockYaw, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyButtonStyle(ui.PB_rotResetLockYaw_undo, CarbonUIClass::Shape_RoundAll);


	ui.GBX_fingerAngleLockYaw->setEnabled(false);

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


	//---start 拇指的角度
	QObject::connect(ui.HS_thumbFigureAngleX, SIGNAL(valueChanged(int)), this, SLOT(onSliderThumbFigureAngleZChanged(int)));
	QObject::connect(ui.HS_thumbFigureAngleX, SIGNAL(sliderReleased()), this, SLOT(onSliderThumbFigureAngleZReleased()));

	QObject::connect(ui.HS_thumbFigureAngleY, SIGNAL(valueChanged(int)), this, SLOT(onSliderThumbFigureAngleZChanged(int)));
	QObject::connect(ui.HS_thumbFigureAngleY, SIGNAL(sliderReleased()), this, SLOT(onSliderThumbFigureAngleZReleased()));

	QObject::connect(ui.HS_thumbFigureAngleZ, SIGNAL(valueChanged(int)), this, SLOT(onSliderThumbFigureAngleZChanged(int)));
	QObject::connect(ui.HS_thumbFigureAngleZ, SIGNAL(sliderReleased()), this, SLOT(onSliderThumbFigureAngleZReleased()));

}

h2FingerAdjustEnhancedWidget::~h2FingerAdjustEnhancedWidget()
{
}

void h2FingerAdjustEnhancedWidget::setIsLeftHand(bool val)
{
	m_bIsLeftHand = val;
	ui.GBX_fingerAngleLockYaw->setTitle(QObject::tr("左手"));
	QGroupBoxCollapser::installIntoGroupBox(ui.GBX_fingerAngleLockYaw, 0, true);
}

void h2FingerAdjustEnhancedWidget::setIsRightHand(bool val)
{
	m_bIsRightHand = val;
	ui.GBX_fingerAngleLockYaw->setTitle(QObject::tr("右手"));
	QGroupBoxCollapser::installIntoGroupBox(ui.GBX_fingerAngleLockYaw, 0, true);
}

void h2FingerAdjustEnhancedWidget::setIsEnable(bool val)
{
	m_bIsEnable = val;

	ui.GBX_fingerAngleLockYaw->setEnabled(val);
}

void h2FingerAdjustEnhancedWidget::readGloveConfigFileToUI(void)
{
	h2GloveMgr newX2Suit;
	unsigned int sn = m_gloveSN;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(sn, newX2Suit);
	if (isFind)
	{
		if (m_bIsLeftHand)
		{
			/*指尖活动量*/
			ui.HS_figureAngleStretch->setValue(newX2Suit.LeftFingerEnhancedModeFistAngle);
			//ui.HS_figureAngleStretch_R->setValue(newX2Suit.RightFingerAngleStretch);
				/*拇指*/
			ui.HS_thumbFigureAngleY->setValue(newX2Suit.angleAdditionEuler[KHS32_LeftHandThumb1][1]);
			ui.HS_thumbFigureAngleZ->setValue(newX2Suit.angleAdditionEuler[KHS32_LeftHandThumb1][2]);
		}
		else if (m_bIsRightHand)
		{
			/*指尖活动量*/
			ui.HS_figureAngleStretch->setValue(newX2Suit.RightFingerEnhancedModeFistAngle);
			//ui.HS_figureAngleStretch_R->setValue(newX2Suit.RightFingerAngleStretch);
				/*拇指*/
			ui.HS_thumbFigureAngleY->setValue(newX2Suit.angleAdditionEuler[KHS32_RightHandThumb1][1]);
			ui.HS_thumbFigureAngleZ->setValue(newX2Suit.angleAdditionEuler[KHS32_RightHandThumb1][2]);
		}
	}
}

bool h2FingerAdjustEnhancedWidget::eventFilter(QObject* obj, QEvent* ev)
{
	if (obj == ui.HS_figureAngleStretch && ev->type() == QEvent::Wheel)
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

void h2FingerAdjustEnhancedWidget::on_PB_rotResetLockYaw_clicked()
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
void h2FingerAdjustEnhancedWidget::on_PB_rotResetLockYaw_undo_clicked()
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

void h2FingerAdjustEnhancedWidget::onSliderFigureAngleStretchChanged(int value)
{
	unsigned int sn = m_gloveSN;

	float currentValue = ((float)value);
	if (m_bIsLeftHand)
	{
		H2_SDK_setLeftHandEnhanceModeFingerGap_Degree(sn, currentValue,0,0);
	}
	else if (m_bIsRightHand)
	{
		H2_SDK_setRightHandEnhanceModeFingerGap_Degree(sn, currentValue, 0, 0);
	}

	ui.LBL_figureAngleStretch->setText(QString::number(currentValue) + QObject::tr("°"));
}

void h2FingerAdjustEnhancedWidget::onSliderFigureAngleStretchReleased()
{
	unsigned int sn = m_gloveSN;

	//保存
	h2GloveMgr newX2Suit;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(sn, newX2Suit);
	if (isFind)
	{
		if (m_bIsLeftHand)
		{
			newX2Suit.LeftFingerEnhancedModeFistAngle =
				ui.HS_figureAngleStretch->value();
		}
		else if (m_bIsRightHand)
		{
			newX2Suit.RightFingerEnhancedModeFistAngle =
				ui.HS_figureAngleStretch->value();
		}
		H2_SDK_updateX2SuitMgrVec(sn, newX2Suit);
	}
	else
	{
		printf("");
	}
}

void h2FingerAdjustEnhancedWidget::onSliderThumbFigureAngleZChanged(int value)
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

void h2FingerAdjustEnhancedWidget::onSliderThumbFigureAngleZReleased()
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

std::string h2FingerAdjustEnhancedWidget::GetLocalDllDirectory()
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

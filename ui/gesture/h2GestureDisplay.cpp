/**
 * @file  h2GestureDisplay.cpp
 * @brief 单个手势显示控件的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 通过 MotionGloveLLSDK 读写设备配置；界面布局由同名 .ui 文件生成。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

#pragma execution_character_set("utf-8")

#include "h2GestureDisplay.h"
#include <QtWidgets/qwidget.h>
#ifdef _WIN32
#include <windows.h>

#else

#endif
#include "h2sdk.h"

h2GestureDisplay::h2GestureDisplay(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	// 设置 QLabel 的大小策略为可扩展（Expanding）
//ui.LBL_gesture->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
// 设置 QLabel 的对齐方式为居中
	ui.LBL_gesture->setAlignment(Qt::AlignCenter);
	ui.LBL_gestureString->setAlignment(Qt::AlignCenter);
	// set font size 12 and white color
	ui.LBL_gestureString->setStyleSheet("color: white; font-size: 12pt;");
	// 使 QLabel 中的图片保持宽高比并自动缩放
	ui.LBL_gesture->setScaledContents(true);
	
	addGestureDisplayPicture(true, false);
	addGestureDisplayPicture(false, true);

	h2GestureTimer = new QTimer(this);
	QObject::connect(h2GestureTimer, SIGNAL(timeout()), this, SLOT(onH2UpdateGestureTimer()));
	h2GestureTimer->start(1000 / h2GestureTimerHz);
}

h2GestureDisplay::~h2GestureDisplay()
{
}

void h2GestureDisplay::setIsLeftHand(bool val)
{
	m_bIsLeftHand = val;
}

void h2GestureDisplay::setIsRightHand(bool val)
{
	m_bIsRightHand = val;
}

void h2GestureDisplay::setIsEnable(bool val)
{
	m_bIsEnable = val;
}

void h2GestureDisplay::readGloveConfigFileToUI(void)
{

}

void h2GestureDisplay::onH2UpdateGestureTimer()
{
	unsigned int sn = m_gloveSN;
	QString pbStylesheetGreen = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(0) + "," +
		QString::number(255) + "," +
		QString::number(0) + ");}";
	QString pbStylesheetRed = "QLabel{border-radius:5px;background-color: rgb(" +
		QString::number(255) + "," +
		QString::number(0) + "," +
		QString::number(0) + ");}";

	bool isLeftHandOnline = H2_SDK_isLeftHand_Connected(sn);
	bool isRightHandOnline = H2_SDK_isRightHand_Connected(sn);

	if (isLeftHandOnline && m_bIsLeftHand)
	{
		int t_leftHandGesture, t_rightHandGesture;
		char t_leftHandGestureString[32] = { 0 };
		char t_rightHandGestureString[32] = { 0 };
		H2_SDK_getGesture(sn,
			&t_leftHandGesture,
			t_leftHandGestureString,
			&t_rightHandGesture,
			t_rightHandGestureString
		);

		std::string gestureName(t_leftHandGestureString);
		ui.LBL_gestureString->setText(t_leftHandGestureString);

		if (m_gesturePixmapMap_LH.find(t_leftHandGestureString) == m_gesturePixmapMap_LH.end())
		{

		}
		else
		{
			// 设置图片到 QLabel
			QPixmap m = m_gesturePixmapMap_LH[gestureName];
			ui.LBL_gesture->setPixmap(m);
			ui.LBL_gesture->show();
		}
	}

	if (isRightHandOnline && m_bIsRightHand)
	{
		int t_leftHandGesture, t_rightHandGesture;
		char t_leftHandGestureString[32] = { 0 };
		char t_rightHandGestureString[32] = { 0 };
		H2_SDK_getGesture(sn,
			&t_leftHandGesture,
			t_leftHandGestureString,
			&t_rightHandGesture,
			t_rightHandGestureString
		);
		std::string gestureName(t_rightHandGestureString);
		//ui.LBL_gesture->setText(QString::number(t_rightHandGesture));
		ui.LBL_gestureString->setText(QString::fromLocal8Bit(t_rightHandGestureString));

		if (m_gesturePixmapMap_RH.find(t_rightHandGestureString) == m_gesturePixmapMap_RH.end())
		{

		}
		else
		{
			// 设置图片到 QLabel
			QPixmap m = m_gesturePixmapMap_RH[gestureName];
			ui.LBL_gesture->setPixmap(m);
			ui.LBL_gesture->show();
		}
	}
}

std::string h2GestureDisplay::GetLocalDllDirectory()
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

void h2GestureDisplay::addGestureDisplayPicture(bool isLeftHand,bool isRightHand)
{
	std::string currentDir = GetLocalDllDirectory();
	std::string imagePath;
	std::map<std::string, QPixmap>* p_gesturePixmapMap ;
	if (isLeftHand)
	{
		p_gesturePixmapMap=&	 m_gesturePixmapMap_LH;
	}
	if(isRightHand)
	{
		p_gesturePixmapMap = &m_gesturePixmapMap_RH;
	}
	std::string leftHandAffix = "LH";
	std::string rightHandAffix = "RH";
	std::string handAffix = isLeftHand ? leftHandAffix : rightHandAffix;

#if 0
	imagePath = currentDir + "\\Resources\\Images\\libh2UI\\gesture\\gesture_1_"+ handAffix+".png";
	(*p_gesturePixmapMap)["1"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = currentDir + "\\Resources\\Images\\libh2UI\\gesture\\gesture_2_" + handAffix + ".png";
	(*p_gesturePixmapMap)["2"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = currentDir + "\\Resources\\Images\\libh2UI\\gesture\\gesture_3_" + handAffix + ".png";
	(*p_gesturePixmapMap)["3"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = currentDir + "\\Resources\\Images\\libh2UI\\gesture\\gesture_4_" + handAffix + ".png";
	(*p_gesturePixmapMap)["4"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = currentDir + "\\Resources\\Images\\libh2UI\\gesture\\gesture_5_" + handAffix + ".png";
	(*p_gesturePixmapMap)["5"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = currentDir + "\\Resources\\Images\\libh2UI\\gesture\\gesture_thumbUp_" + handAffix + ".png";
	(*p_gesturePixmapMap)["thumbUp"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = currentDir + "\\Resources\\Images\\libh2UI\\gesture\\UndefinedGesture.png";
	(*p_gesturePixmapMap)["UndefinedGesture"] = QPixmap(QString::fromStdString(imagePath));
#else
	// Use Qt resource paths directly (no filesystem prefix). Prefixing with currentDir broke 
	// the ":/" resource scheme (e.g. "C:\...:/gestures/...") so QPixmap couldn't find the files.
	imagePath = ":/gestures/Resources/gesture/gesture_1_" + handAffix + ".png";
	(*p_gesturePixmapMap)["1"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = ":/gestures/Resources/gesture/gesture_2_" + handAffix + ".png";
	(*p_gesturePixmapMap)["2"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = ":/gestures/Resources/gesture/gesture_3_" + handAffix + ".png";
	(*p_gesturePixmapMap)["3"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = ":/gestures/Resources/gesture/gesture_4_" + handAffix + ".png";
	(*p_gesturePixmapMap)["4"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = ":/gestures/Resources/gesture/gesture_5_" + handAffix + ".png";
	(*p_gesturePixmapMap)["5"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = ":/gestures/Resources/gesture/gesture_thumbUp_" + handAffix + ".png";
	(*p_gesturePixmapMap)["thumbUp"] = QPixmap(QString::fromStdString(imagePath));
	imagePath = ":/gestures/Resources/gesture/UndefinedGesture.png";
	(*p_gesturePixmapMap)["UndefinedGesture"] = QPixmap(QString::fromStdString(imagePath));

#endif
}

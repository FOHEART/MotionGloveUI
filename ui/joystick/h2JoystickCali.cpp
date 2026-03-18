//#include "stdafx.h"
#include "h2JoystickCali.h"
#include "h2sdk.h"
#include <QtGui/QMovie>
#ifdef _WIN32
#include <windows.h>

#else

#endif

h2JoystickCali::h2JoystickCali(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	std::string blankImageDir = GetLocalDllDirectory() + "\\Resources\\Images\\libh2UI\\Joystick\\joystick.png";
	pixmap_blank = new QPixmap(QString::fromStdString(blankImageDir));

	QObject::connect(&m_caliTimer, SIGNAL(timeout()), this, SLOT(on_caliTimer_timeout()));

	{
		// 创建一个 QMovie 对象，并指定要播放的 GIF 文件路径
		std::string movieDir = GetLocalDllDirectory() + "\\Resources\\Images\\libh2UI\\Joystick\\centerCali.gif";
		movie_centerCali = new QMovie(QString::fromStdString(movieDir));
	}
	{
		// 创建一个 QMovie 对象，并指定要播放的 GIF 文件路径
		std::string movieDir = GetLocalDllDirectory() + "\\Resources\\Images\\libh2UI\\Joystick\\rotCali.gif";
		movie_rotCali = new QMovie(QString::fromStdString(movieDir));
	}
	  
	showBlankImage();
}

std::string h2JoystickCali::GetLocalDllDirectory()
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

void h2JoystickCali::showBlankImage()
{
	// 将 QPixmap 关联到 QLabel 上
	QLabel* label = ui.LBL_movie;
	label->setAlignment(Qt::AlignCenter);
	label->setPixmap(*pixmap_blank);
	label->setScaledContents(true);
}

void h2JoystickCali::showCenterCaliImage()
{
	// 创建一个 QLabel 用来显示 GIF 动画
	QLabel* label = ui.LBL_movie;
	label->setAlignment(Qt::AlignCenter); // 设置文本居中显示
	// 将 QMovie 关联到 QLabel 上
	label->setMovie(movie_centerCali);
	// 开始播放 GIF 动画
	movie_centerCali->start();
}

void h2JoystickCali::showRotCaliImage()
{
	// 创建一个 QLabel 用来显示 GIF 动画
	QLabel* label = ui.LBL_movie;
	label->setAlignment(Qt::AlignCenter); // 设置文本居中显示
	// 将 QMovie 关联到 QLabel 上
	label->setMovie(movie_rotCali);
	// 开始播放 GIF 动画
	movie_rotCali->start();
}

h2JoystickCali::~h2JoystickCali()
{

}

void h2JoystickCali::restoreCaliValueFromConfig(void)
{
	//保存
	//h2GloveMgr newX2Suit;
	KHHS32JoystickMessage_TypeDef newX2Suit;
	bool isFind = H2_SDK_getJoystickBySn(m_sn, newX2Suit);
	if (isFind)
	{

		if (isLeftHand)
		{
#if 1
			H2_SDK_setJoystick_leftHand_centerCaliValue(m_sn,
				newX2Suit.joystick_LH.JSCenterX,
				newX2Suit.joystick_LH.JSCenterY);

			H2_SDK_setJoystick_leftHand_minMaxCaliValue(m_sn,
				newX2Suit.joystick_LH.JSMinX,
				newX2Suit.joystick_LH.JSMaxX,
				newX2Suit.joystick_LH.JSMinY,
				newX2Suit.joystick_LH.JSMaxY
			);
#endif
		}
		if (isRightHand)
		{
#if 1
			H2_SDK_setJoystick_rightHand_centerCaliValue(m_sn,
				newX2Suit.joystick_LH.JSCenterX,
				newX2Suit.joystick_LH.JSCenterY);

			H2_SDK_setJoystick_rightHand_minMaxCaliValue(m_sn,
				newX2Suit.joystick_LH.JSMinX,
				newX2Suit.joystick_LH.JSMaxX,
				newX2Suit.joystick_LH.JSMinY,
				newX2Suit.joystick_LH.JSMaxY
			);
#endif
		}
	}
	else
	{
		printf("");
	}

}

void h2JoystickCali::on_caliTimer_timeout()
{
	switch (m_js_caliFunc)
	{
		case JS_CaliFunction_None:
		{

		}
		break;
		case JS_CaliFunction_Center:
		{
			/*
			中心点 静止 校准结束
			*/
			m_centerCaliCounter++;
			unsigned int gap = 100 / m_centerCaliCounterMax;

			ui.progressBar->setValue(gap * m_centerCaliCounter);
			if (m_centerCaliCounter == m_centerCaliCounterMax)
			{
				m_caliTimer.stop();
				movie_centerCali->stop();
				showBlankImage();

				int xCenterCalivalue, yCenterCaliValue = 0;
				if (0 == H2_SDK_joystick_centerCali_Stop(m_sn, isLeftHand, isRightHand))
				{
					/*
					从sdk里获取刚校准的值
					*/
					if (isLeftHand)
					{
						H2_SDK_getJoystick_leftHand_centerCaliValue(m_sn,
							&xCenterCalivalue, &yCenterCaliValue);
						{
							//保存
							KHHS32JoystickMessage_TypeDef newX2Suit;
							bool isFind = H2_SDK_getJoystickBySn(m_sn, newX2Suit);
							if (isFind)
							{
								newX2Suit.joystick_LH.JSCenterX = xCenterCalivalue;
								newX2Suit.joystick_LH.JSCenterY = yCenterCaliValue;
								H2_SDK_updateJoystickVec(m_sn, newX2Suit);
							}
							else
							{
								printf("");
							}
						}
					}

					if (isRightHand)
					{
						H2_SDK_getJoystick_rightHand_centerCaliValue(m_sn,
							&xCenterCalivalue, &yCenterCaliValue);
						{
							//保存
							KHHS32JoystickMessage_TypeDef newX2Suit;
							bool isFind = H2_SDK_getJoystickBySn(m_sn, newX2Suit);
							if (isFind)
							{
								newX2Suit.joystick_RH.JSCenterX = xCenterCalivalue;
								newX2Suit.joystick_RH.JSCenterY = yCenterCaliValue;
								H2_SDK_updateJoystickVec(m_sn, newX2Suit);
							}
							else
							{
								printf("");
							}
						}
					}
					/*
					end
					*/
				}

				ui.PB_joystickCaliCenter->setEnabled(true);
				ui.PB_joystickCaliRot->setEnabled(true);
				ui.progressBar->setValue(100);
			}
		}
		break;
		case JS_CaliFunction_Rot:
		{
			/*
			转动校准结束
			*/
			m_rotCaliCounter++;
			unsigned int gap = 100 / m_rotCaliCounterMax;

			ui.progressBar->setValue(gap * m_rotCaliCounter);
			if (m_rotCaliCounter == m_rotCaliCounterMax)
			{
				m_caliTimer.stop();

				movie_centerCali->stop();
				showBlankImage();

				int xMinCalivalue, xMaxCaliValue = 0;
				int yMinCalivalue, yMaxCaliValue = 0;

				if (0 == H2_SDK_joystick_rotateCali_Stop(m_sn, isLeftHand, isRightHand))
				{
					/*
					从sdk里获取刚校准的值
					*/
					if (isLeftHand)
					{
						H2_SDK_getJoystick_leftHand_minMaxCaliValue(m_sn,
							&xMinCalivalue, &xMaxCaliValue,
							&yMinCalivalue, &yMaxCaliValue
						);

						{
							//保存
							KHHS32JoystickMessage_TypeDef newX2Suit;
							bool isFind = H2_SDK_getJoystickBySn(m_sn, newX2Suit);
							if (isFind)
							{
								newX2Suit.joystick_LH.JSMinX = xMinCalivalue;
								newX2Suit.joystick_LH.JSMaxX = xMaxCaliValue;
								newX2Suit.joystick_LH.JSMinY = yMinCalivalue;
								newX2Suit.joystick_LH.JSMaxY = yMaxCaliValue;
								H2_SDK_updateJoystickVec(m_sn, newX2Suit);
							}
							else
							{
								printf("");
							}
						}
					}

					if (isRightHand)
					{
						H2_SDK_getJoystick_rightHand_minMaxCaliValue(m_sn,
							&xMinCalivalue, &xMaxCaliValue,
							&yMinCalivalue, &yMaxCaliValue
						);

						{
							//保存
							KHHS32JoystickMessage_TypeDef newX2Suit;
							bool isFind = H2_SDK_getJoystickBySn(m_sn, newX2Suit);
							if (isFind)
							{
								newX2Suit.joystick_RH.JSMinX = xMinCalivalue;
								newX2Suit.joystick_RH.JSMaxX = xMaxCaliValue;
								newX2Suit.joystick_RH.JSMinY = yMinCalivalue;
								newX2Suit.joystick_RH.JSMaxY = yMaxCaliValue;
								H2_SDK_updateJoystickVec(m_sn, newX2Suit);
							}
							else
							{
								printf("");
							}
						}
					}
					/*
					end
					*/
				}
				ui.PB_joystickCaliCenter->setEnabled(true);
				ui.PB_joystickCaliRot->setEnabled(true);
				ui.progressBar->setValue(100);
			}
		}
		break;
		default:
			break;
	}
}

void h2JoystickCali::on_PB_joystickCaliCenter_clicked()
{
	if (m_caliTimer.isActive())
		return;
	ui.PB_joystickCaliCenter->setEnabled(false);
	ui.PB_joystickCaliRot->setEnabled(false);

	m_centerCaliCounter = 0;
	ui.progressBar->setValue(0);
	m_caliTimer.setInterval(200);
	m_js_caliFunc = JS_CaliFunction_Center;
	m_caliTimer.start();

	H2_SDK_joystick_centerCali_Start(m_sn, isLeftHand, isRightHand);

	showCenterCaliImage();

}

void h2JoystickCali::on_PB_joystickCaliRot_clicked()
{
	if (m_caliTimer.isActive())
		return;
	ui.PB_joystickCaliCenter->setEnabled(false);
	ui.PB_joystickCaliRot->setEnabled(false);

	m_rotCaliCounter = 0;
	ui.progressBar->setValue(0);
	m_caliTimer.setInterval(200);
	m_js_caliFunc = JS_CaliFunction_Rot;

	m_caliTimer.start();

	H2_SDK_joystick_rotateCali_Start(m_sn, isLeftHand, isRightHand);
	showRotCaliImage();
}

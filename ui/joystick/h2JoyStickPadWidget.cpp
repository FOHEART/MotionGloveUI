/**
 * @file  h2JoyStickPadWidget.cpp
 * @brief 摇杆姿态显示控件的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 纯声明或转发头，无运行期逻辑。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

//#include "stdafx.h"
#include "h2JoyStickPadWidget.h"
#ifdef _WIN32
#include <windows.h>

#else

#endif
#include <cmath>

h2JoyStickPadWidget::h2JoyStickPadWidget(QWidget *parent)
	: QWidget(parent)
{
	Init(); //初始化函数
}

h2JoyStickPadWidget::~h2JoyStickPadWidget()
{

}

void h2JoyStickPadWidget::setJoyStickDown(bool s)
{
	isKeyDown = s;
}


void h2JoyStickPadWidget::setXY(float x, float y)
{
	int newX = std::round((x / 2 + 0.5)  * BIG_CIRCLE_RADIUS * 2);
	int newY = std::round((y / 2 + 0.5)  * BIG_CIRCLE_RADIUS * 2);


	isValue1_JS = false;
	isValue0_JS = false;
	if (
		(x == 1) || (x == -1) || (y == 1) || (y == -1)
		)
	{
		isValue1_JS = true;
	}
	else if ((x == 0) || (y == 0))
	{
		isValue0_JS = true;
	}
	else
	{
		isValue1_JS = false;
		isValue0_JS = false;
	}

	BigCir_xy.setX(newX);
	BigCir_xy.setY(newY);
	update();
}

//初始化函数
void h2JoyStickPadWidget::Init()
{
	//设置窗口无边框且窗口显示在最顶层
	//this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	bigCircle_Pixmap.load(QString::fromStdString(GetLocalDllDirectory()) + "\\Resources\\Images\\libh2UI\\aim.png");
	bigCircleEdge_Pixmap.load(QString::fromStdString(GetLocalDllDirectory()) + "\\Resources\\Images\\libh2UI\\aim_edge.png");

	smallCircle_Pixmap.load(QString::fromStdString(GetLocalDllDirectory()) + "\\Resources\\Images\\libh2UI\\aim2.png");
	smallCircleDown_Pixmap.load(QString::fromStdString(GetLocalDllDirectory()) + "\\Resources\\Images\\libh2UI\\aim2_down.png");
	smallCircleZero_Pixmap.load(QString::fromStdString(GetLocalDllDirectory()) + "\\Resources\\Images\\libh2UI\\aim3.png");

	//设置大圆圆心位置
	SmallCir_xy.setX(BIG_CIRCLE_RADIUS);
	SmallCir_xy.setY(BIG_CIRCLE_RADIUS);
	//设置小圆圆心位置，与大圆相同
	BigCir_xy = SmallCir_xy;

	//设置窗口固定大小为400*400
	this->setFixedSize(BIG_CIRCLE_RADIUS * 2, BIG_CIRCLE_RADIUS * 2);

	//鼠标点击标志初始化
	MousePressFlag = false;
}

//绘图事件：绘制地图、摇杆中的大圆和小圆
void h2JoyStickPadWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	//绘图画笔
	QPainter painter(this);
	//抗锯齿
	painter.setRenderHint(QPainter::Antialiasing, true);
	//消锯齿
	painter.setRenderHints(QPainter::SmoothPixmapTransform);

	//绘制摇杆中的大圆
	//QPixmap bigCircle_Pixmap;
	//bigCircle_Pixmap.load(G_APP_DIR+ "/Resources/Images/aim.png");

	//
	if (isValue1_JS)
	{
		painter.drawPixmap(SmallCir_xy.x() - BIG_CIRCLE_RADIUS, SmallCir_xy.y() - BIG_CIRCLE_RADIUS, \
			BIG_CIRCLE_RADIUS * 2, BIG_CIRCLE_RADIUS * 2, bigCircleEdge_Pixmap);
	}
	else
	{
		painter.drawPixmap(SmallCir_xy.x() - BIG_CIRCLE_RADIUS, SmallCir_xy.y() - BIG_CIRCLE_RADIUS, \
			BIG_CIRCLE_RADIUS * 2, BIG_CIRCLE_RADIUS * 2, bigCircle_Pixmap);
	}


	//绘制摇杆中的小圆
	if (isKeyDown)
	{
		painter.drawPixmap(BigCir_xy.x() - SMALL_CIRCLE_RADIUS, BigCir_xy.y() - SMALL_CIRCLE_RADIUS, \
			SMALL_CIRCLE_RADIUS * 2, SMALL_CIRCLE_RADIUS * 2, smallCircleDown_Pixmap);
	}
	else if (isValue0_JS)
	{
		painter.drawPixmap(BigCir_xy.x() - SMALL_CIRCLE_RADIUS, BigCir_xy.y() - SMALL_CIRCLE_RADIUS, \
			SMALL_CIRCLE_RADIUS * 2, SMALL_CIRCLE_RADIUS * 2, smallCircleZero_Pixmap);
	}
	else
	{
		painter.drawPixmap(BigCir_xy.x() - SMALL_CIRCLE_RADIUS, BigCir_xy.y() - SMALL_CIRCLE_RADIUS, \
			SMALL_CIRCLE_RADIUS * 2, SMALL_CIRCLE_RADIUS * 2, smallCircle_Pixmap);
	}

}

std::string h2JoyStickPadWidget::GetLocalDllDirectory()
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

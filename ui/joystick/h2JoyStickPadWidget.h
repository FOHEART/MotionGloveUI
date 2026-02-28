#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>

#define SMALL_CIRCLE_RADIUS 10 //小圆半径 
#define BIG_CIRCLE_RADIUS (100/2) //大圆半径 

class h2JoyStickPadWidget : public QWidget
{
	Q_OBJECT

public:
	h2JoyStickPadWidget(QWidget *parent = nullptr);
	~h2JoyStickPadWidget();

	void setJoyStickDown(bool s);

	//x y range:0~65535
	//void setXY(int x, int y);
	void setXY(float x, float y);

private:
	QPoint BigCir_xy; //大圆圆心坐标
	QPoint SmallCir_xy; //小圆圆心坐标

	bool MousePressFlag; //鼠标按下的标志位

	QPoint MapRemov_Old;

	//绘制摇杆中的大圆
	QPixmap bigCircle_Pixmap;//灰色圈
	QPixmap bigCircleEdge_Pixmap;//蓝色圈
	bool isValue1_JS = false;//摇杆达到最大值

	//绘制摇杆中的小圆
	QPixmap smallCircle_Pixmap;//没按下
	QPixmap smallCircleDown_Pixmap;//按下
	QPixmap smallCircleZero_Pixmap;//0值
	bool isValue0_JS = false;//摇杆达到0值

	//摇杆按下
	bool isKeyDown = false;
	void Init();

	void paintEvent(QPaintEvent *event); //绘图事件：绘制地图、摇杆中的大圆和小圆
	std::string GetLocalDllDirectory();
};


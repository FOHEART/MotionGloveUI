#pragma once
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include "ui_h2JoystickCali.h"

typedef enum{
	JS_CaliFunction_None,
	JS_CaliFunction_Center,//中心校准
	JS_CaliFunction_Rot//旋转校准
}JS_CaliFunction;



class  h2JoystickCali : public QWidget
{
	Q_OBJECT

public:
	h2JoystickCali(QWidget *parent = Q_NULLPTR);
	~h2JoystickCali();

	unsigned int getSn() const { return m_sn; }
	void setSn(unsigned int val) { m_sn = val; }

	void restoreCaliValueFromConfig(void);

	bool getIsLeftHand() const { return isLeftHand; }
	void setIsLeftHand(bool val) { isLeftHand = val; }

	bool getIsRightHand() const { return isRightHand; }
	void setIsRightHand(bool val) { isRightHand = val; }

private:
	std::string GetLocalDllDirectory();

	void showBlankImage();
	void showCenterCaliImage();
	void showRotCaliImage();

	public slots:
	void on_caliTimer_timeout();
	void on_PB_joystickCaliCenter_clicked();
	void on_PB_joystickCaliRot_clicked();

private:
	Ui::h2JoystickCali ui;

	QTimer m_caliTimer;
	unsigned int m_sn = 0;

	bool isLeftHand = false;
	bool isRightHand = false;

	JS_CaliFunction m_js_caliFunc = JS_CaliFunction_None;

	QMovie *movie_centerCali=nullptr;
	QMovie *movie_rotCali = nullptr;

	QPixmap *pixmap_blank = nullptr; 
private:
	unsigned int m_centerCaliCounter = 0;
	unsigned int m_centerCaliCounterMax = 4;

	unsigned int m_rotCaliCounter = 0;
	unsigned int m_rotCaliCounterMax = 16;
};

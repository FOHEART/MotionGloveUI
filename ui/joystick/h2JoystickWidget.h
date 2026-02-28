#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QString>
#include <QtCore/QTimer>

// Hide the generated UI header from consumers of this widget by forward
// declaring the UI class and keeping the real include inside the .cpp.
namespace Ui { class h2JoystickWidget; }

class  h2JoystickWidget : public QWidget
{
	Q_OBJECT

public:
	h2JoystickWidget(QWidget *parent = Q_NULLPTR);
	~h2JoystickWidget();

	unsigned int getGloveSN() const { return m_gloveSN; }
	void setGloveSN(unsigned int val) { m_gloveSN = val; }

	bool getIsLeftHand() const { return m_bIsLeftHand; }
	void setIsLeftHand(bool val);
	bool getIsRightHand() const { return m_bIsRightHand; }
	void setIsRightHand(bool val);

	bool getIsEnable() const { return m_bIsEnable; }
	void setIsEnable(bool val);

	bool getIsJoyStickExist() const { return m_bJoyStickExist; }
	void setIsJoyStickExist(bool val);

	void readGloveConfigFileToUI(void);

	


private slots:
	void onH2JoystickTimer();
	void onHS_JSDeadZone_LH_Changed(int value);
	void onHS_JSDeadZone_LH_Released();

	void on_PB_openOpenGlove_clicked(void);
	void on_PB_closeOpenGlove_clicked(void);
private:
	Ui::h2JoystickWidget* ui = nullptr;

	unsigned int m_gloveSN = 0;
	bool m_bIsLeftHand = false;
	bool m_bIsRightHand = false;

	bool m_bIsEnable = false;
	bool m_bJoyStickExist = false;


	QString pbStylesheetGreen ;
	QString pbStylesheetRed ;

	/*
	更新摇杆的UI界面 帧率尽量保证60hz
	*/
	QTimer* h2JoyStickTimer = nullptr;
	unsigned int h2JoyStickTimerHz = 60;
	//是否显示Index模拟调试按钮
	bool m_isIndexSimuDebug = false;
private:
	void startUpdateUITimer(void);
	void stopUpdateUITimer(void);

};

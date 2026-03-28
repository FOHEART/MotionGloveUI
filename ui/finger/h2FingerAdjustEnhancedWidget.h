#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2FingerAdjustEnhancedWidget.h"

class h2FingerAdjustEnhancedWidget : public QWidget
{
	Q_OBJECT

public:
	h2FingerAdjustEnhancedWidget(QWidget *parent = Q_NULLPTR);
	~h2FingerAdjustEnhancedWidget();
	unsigned int getGloveSN() const { return m_gloveSN; }
	void setGloveSN(unsigned int val) { m_gloveSN = val; }

	bool getIsLeftHand() const { return m_bIsLeftHand; }
	void setIsLeftHand(bool val);
	bool getIsRightHand() const { return m_bIsRightHand; }
	void setIsRightHand(bool val);

	bool getIsEnable() const { return m_bIsEnable; }
	void setIsEnable(bool val);

	/*
	将配置文件中的设置读出来显示到界面上
	*/
	void readGloveConfigFileToUI(void);
protected:
	// Filter events to block wheel events on the stretch slider
	bool eventFilter(QObject* obj, QEvent* ev) override;
private slots:
	void on_PB_rotResetLockYaw_clicked();
	void on_PB_rotResetLockYaw_undo_clicked();

	void onSliderFigureAngleStretchChanged(int value);

	void onSliderFigureAngleStretchReleased();


	//拇指的设定---x
	//void  onSliderThumbFigureAngleXChanged(int value);
	//void onSliderThumbFigureAngleXReleased();
	//---y
	//void  onSliderThumbFigureAngleYChanged(int value);
	//void onSliderThumbFigureAngleYReleased();
	//---z
	void  onSliderThumbFigureAngleZChanged(int value);
	void onSliderThumbFigureAngleZReleased();
private:
	void ensureCollapserInstalled();

	Ui::h2FingerAdjustEnhancedWidget ui;
	bool m_collapserInstalled = false;
	unsigned int m_gloveSN = 0;
	bool m_bIsLeftHand = false;
	bool m_bIsRightHand = false;

	bool m_bIsEnable = false;

private:
	std::string GetLocalDllDirectory();
};

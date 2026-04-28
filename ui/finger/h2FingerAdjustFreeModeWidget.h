#pragma once
#include <QtWidgets/QWidget>
#include "ui_h2FingerAdjustFreeModeWidget.h"

class  h2FingerAdjustFreeModeWidget : public QWidget
{
	Q_OBJECT

public:
	h2FingerAdjustFreeModeWidget(QWidget *parent = Q_NULLPTR);
	~h2FingerAdjustFreeModeWidget();
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
	void setupLanguageChangeConnection();
	
	// Set language explicitly ("en" or "zh")
	void setLanguage(const QString& lang);
protected:
	// Filter events to block wheel events on the stretch slider
	bool eventFilter(QObject* obj, QEvent* ev) override;
private slots:
	void on_PB_rotResetFingerFreeMode_clicked();
	void on_PB_rotResetFingerFreeMode_undo_clicked();
	void on_PB_rotResetHandFreeMode_clicked();
	void on_PB_rotResetHandFreeMode_undo_clicked();
	
	//拇指的设定---x
	//void  onSliderThumbFigureAngleXChanged(int value);
	//void onSliderThumbFigureAngleXReleased();
	//---y
	//void  onSliderThumbFigureAngleYChanged(int value);
	//void onSliderThumbFigureAngleYReleased();
	//---z
	void  onSliderThumbFigureAngleZChanged(int value);
	void onSliderThumbFigureAngleZReleased();
private slots:
	void retranslateUI();
private:
	void ensureCollapserInstalled();

	Ui::h2FingerAdjustFreeModeWidget ui;
	bool m_collapserInstalled = false;

	unsigned int m_gloveSN = 0;
	bool m_bIsLeftHand = false;
	bool m_bIsRightHand = false;

	//设置整个窗体是否可用
	bool m_bIsEnable = false;
	
	QString m_language = "zh";  // Current language ("en" or "zh")
private:
	std::string GetLocalDllDirectory();

};

#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2FingerAdjustLockYawWidget.h"

class  h2FingerAdjustLockYawWidget : public QWidget
{
	Q_OBJECT

public:
	h2FingerAdjustLockYawWidget(QWidget *parent = Q_NULLPTR);
	~h2FingerAdjustLockYawWidget();
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

	/*
	设置是否显示配置项目左侧的标签
	*/
	void setShowConfigItemHeader(bool isShow);
	void setupLanguageChangeConnection();
	
	// Set language explicitly ("en" or "zh")
	void setLanguage(const QString& lang);

private slots:
	void on_PB_rotResetLockYaw_clicked();
	void on_PB_rotResetLockYaw_undo_clicked();

	void onSliderFigureAngleStretchChanged(int value);
	void onSliderFigureAngleStretchReleased();

	void onSliderFigureAngleFistChanged(int value);
	void onSliderFigureAngleFistReleased();
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
protected:
	// Filter events to block wheel events on the stretch slider
	bool eventFilter(QObject* obj, QEvent* ev) override;

//public slots:
//	void onSyncRightHandConfig();
//signals:
//	//当左右手的配置相同时，点击了手的配置 改变了左手的设置 向上层发送信号 通知右手也改变
//	void signalLeftHandConfigChanged();

private:
	void ensureCollapserInstalled();

	Ui::h2FingerAdjustLockYawWidget ui;
	bool m_collapserInstalled = false;
	unsigned int m_gloveSN = 0;
	bool m_bIsLeftHand = false;
	bool m_bIsRightHand = false;

	bool m_bIsEnable = false;
	bool m_bShowConfigItemHeader = true; //是否显示配置项目的标题标签 例如 显示“张开” “握拳” 等标签	
	QString m_language = "zh";  // Current language ("en" or "zh")private:
	std::string GetLocalDllDirectory();

};

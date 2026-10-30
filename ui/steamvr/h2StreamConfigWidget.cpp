/**
 * @file  h2StreamConfigWidget.cpp
 * @brief 串流配置控件的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；控件样式统一由 CarbonUIClass 提供；通过 MotionGloveLLSDK 读写设备配置；依赖 uic 生成的 ui_*.h。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

#include "../../include/h2StreamConfigWidget.h"

#include "h2sdk.h"
#include "ui_h2StreamConfigWidget.h"

#include "CarbonUIClass.h"
#include "groupboxcollapser.h"
#include <QtCore/QMetaObject>
#include <QtCore/QCoreApplication>



h2StreamConfigWidget::h2StreamConfigWidget(QWidget* parent)
	: QWidget(parent), ui(std::make_unique<Ui::h2StreamConfigWidget>())
{
	ui->setupUi(this);

	CarbonUIClass::setWidgetLabelsStyleRecursively(this);
	CarbonUIClass::applyScrollAreaStyle(ui->scrollArea);

	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_stream, 0, true);
	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_frame, 0, true);
	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_loc, 0, true);
	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_rot, 0, true);
	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_rotAxis, 0, true);
	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_sensorRot, 0, true);
	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_sensor, 0, true);
	QGroupBoxCollapser::installIntoGroupBox(ui->GBX_rotOrder, 0, true);

	CarbonUIClass::applyCheckStyle(ui->CBX_jumboFrame);

	CarbonUIClass::applyCheckStyle(ui->CBX_displacement);
	
		
	

	CarbonUIClass::applyRadioStyle(ui->RB_binary, CarbonUIClass::Radio_LeftRound, 64);
	//CarbonUIClass::applyRadioStyle(ui->RB_lockMode, CarbonUIClass::Radio_Rectangle, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_string, CarbonUIClass::Radio_RightRound, 64);

	CarbonUIClass::applyRadioStyle(ui->RB_skeletonQuat, CarbonUIClass::Radio_LeftRound, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_skeletonEuler, CarbonUIClass::Radio_RightRound, 64);
	
	CarbonUIClass::applyRadioStyle(ui->RB_skeletonLocalCoord, CarbonUIClass::Radio_LeftRound, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_skeletonGlobalCoord, CarbonUIClass::Radio_RightRound, 64);

	CarbonUIClass::applyRadioStyle(ui->RB_rtSensorOff, CarbonUIClass::Radio_LeftRound, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_rtSensorQuat, CarbonUIClass::Radio_Rectangle, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_rtSensorEuler, CarbonUIClass::Radio_RightRound, 64);

	CarbonUIClass::applyRadioStyle(ui->RB_xyz, CarbonUIClass::Radio_LeftRound, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_xzy, CarbonUIClass::Radio_Rectangle, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_yxz, CarbonUIClass::Radio_Rectangle, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_yzx, CarbonUIClass::Radio_Rectangle, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_zxy, CarbonUIClass::Radio_Rectangle, 64);
	CarbonUIClass::applyRadioStyle(ui->RB_zyx, CarbonUIClass::Radio_RightRound, 64);

	updateUIFromConfig();
	
	// Setup language change connection for this widget
	for (int i = 0; i < QCoreApplication::instance()->children().size(); ++i)
	{
		QObject *obj = QCoreApplication::instance()->children()[i];
		if (obj && obj->inherits("LanguageManager"))
		{
			connect(obj, SIGNAL(languageChanged(QString)), this, SLOT(retranslateUI()));
			break;
		}
	}
	
	// Apply current language translations on initialization
	retranslateUI();
}

h2StreamConfigWidget::~h2StreamConfigWidget() = default;

void h2StreamConfigWidget::updateUIFromConfig(void)
{
	h2GloveStreamConfig cfg;
	H2_SDK_getGloveStreamConfig(&cfg);

	ui->GBX_sensor->setVisible(false);
	switch (cfg.skeletonAttitude)
	{
		case SkeletonAttitude_Euler:
		{
			ui->RB_skeletonEuler->setChecked(true);
			break;
		}
		case SkeletonAttitude_Quat:
		{
			ui->RB_skeletonQuat->setChecked(true);
			break;
		}
	}

	switch (cfg.skeletonCoordinate)
	{
		case SkeletonCoordinate_Relative:
		{
			ui->RB_skeletonLocalCoord->setChecked(true);
			break;
		}
		case SkeletonCoordinate_Global:
		{
			ui->RB_skeletonGlobalCoord->setChecked(true);
			break;
		}
	}

	switch (cfg.channelOrder)
	{
		case H2Euler_XYZ:
		{
			ui->RB_xyz->setChecked(true);
			break;
		}
		case H2Euler_XZY:
		{
			ui->RB_xzy->setChecked(true);
			break;
		}
		case H2Euler_YXZ:
		{
			ui->RB_yxz->setChecked(true);
			break;
		}
		case H2Euler_YZX:
		{
			ui->RB_yzx->setChecked(true);
			break;
		}
		case H2Euler_ZXY:
		{
			ui->RB_zxy->setChecked(true);
			break;
		}
		case H2Euler_ZYX:
		{
			ui->RB_zyx->setChecked(true);
			break;
		}
	}

	ui->CBX_jumboFrame->setChecked(cfg.jumboFrameEnable != 0);
}

void h2StreamConfigWidget::writeH2StreamConfig(void)
{
	h2GloveStreamConfig cfg;
	H2_SDK_getGloveStreamConfig(&cfg);

	if (ui->RB_skeletonEuler->isChecked())
	{
		cfg.skeletonAttitude = SkeletonAttitude_Euler;
	}
	else if (ui->RB_skeletonQuat->isChecked())
	{
		cfg.skeletonAttitude = SkeletonAttitude_Quat;
	}

	if (ui->RB_skeletonLocalCoord->isChecked())
	{
		cfg.skeletonCoordinate = SkeletonCoordinate_Relative;
	}
	else if (ui->RB_skeletonGlobalCoord->isChecked())
	{
		cfg.skeletonCoordinate = SkeletonCoordinate_Global;
	}

	cfg.jumboFrameEnable = ui->CBX_jumboFrame->isChecked() ? 1 : 0;

	if (ui->RB_xyz->isChecked())
	{
		cfg.channelOrder = H2Euler_XYZ;
	}
	else if (ui->RB_xzy->isChecked())
	{
		cfg.channelOrder = H2Euler_XZY;
	}
	else if (ui->RB_yxz->isChecked())
	{
		cfg.channelOrder = H2Euler_YXZ;
	}
	else if (ui->RB_yzx->isChecked())
	{
		cfg.channelOrder = H2Euler_YZX;
	}
	else if (ui->RB_zxy->isChecked())
	{
		cfg.channelOrder = H2Euler_ZXY;
	}
	else if (ui->RB_zyx->isChecked())
	{
		cfg.channelOrder = H2Euler_ZYX;
	}

	H2_SDK_setGloveStreamConfig(cfg);
}

void h2StreamConfigWidget::setupLanguageChangeConnection()
{
	for (int i = 0; i < QCoreApplication::instance()->children().size(); ++i)
	{
		QObject *obj = QCoreApplication::instance()->children()[i];
		if (obj && obj->inherits("LanguageManager"))
		{
			connect(obj, SIGNAL(languageChanged(QString)), this, SLOT(retranslateUI()));
			break;
		}
	}
}

void h2StreamConfigWidget::retranslateUI()
{
	// Update groupbox titles with current language
	if (ui) {
		ui->GBX_stream->setTitle(tr("数据流 / Data Stream"));
		ui->GBX_frame->setTitle(tr("帧设置 / Frame Settings"));
		ui->GBX_loc->setTitle(tr("位移 / Displacement"));
		ui->GBX_rot->setTitle(tr("旋转 / Rotation"));
		ui->GBX_rotAxis->setTitle(tr("骨骼旋转系 / Skeleton Rotation System"));
		ui->GBX_sensorRot->setTitle(tr("传感器原始旋转 / Raw Sensor Rotation"));
		ui->GBX_sensor->setTitle(tr("传感器原始数据 / Raw Sensor Data"));
		ui->GBX_rotOrder->setTitle(tr("欧拉角旋转顺序 / Euler Angle Rotation Order"));
	}
	
	// Update data stream radio button texts
	if (ui) {
		ui->RB_binary->setText(tr("二进制 / Binary"));
		ui->RB_string->setText(tr("字符串 / String"));
	}
	
	// Update skeleton attitude radio button texts
	if (ui) {
		ui->RB_skeletonQuat->setText(tr("四元数 / Quaternion"));
		ui->RB_skeletonEuler->setText(tr("欧拉角 / Euler Angle"));
	}
	
	// Update skeleton coordinate radio button texts
	if (ui) {
		ui->RB_skeletonLocalCoord->setText(tr("本地坐标系 / Local Coordinate System"));
		ui->RB_skeletonGlobalCoord->setText(tr("全局坐标系 / Global Coordinate System"));
	}
	
	// Update raw sensor rotation radio button texts
	if (ui) {
		ui->RB_rtSensorOff->setText(tr("无 / None"));
		ui->RB_rtSensorQuat->setText(tr("四元数 / Quaternion"));
		ui->RB_rtSensorEuler->setText(tr("欧拉角 / Euler Angle"));
	}
	
	// Update euler angle rotation order texts
	if (ui) {
		ui->RB_xyz->setText(tr("XYZ"));
		ui->RB_xzy->setText(tr("XZY"));
		ui->RB_yxz->setText(tr("YXZ"));
		ui->RB_yzx->setText(tr("YZX"));
		ui->RB_zxy->setText(tr("ZXY"));
		ui->RB_zyx->setText(tr("ZYX"));
	}
	
	// Update checkbox texts
	if (ui) {
		ui->CBX_jumboFrame->setText(tr("巨型帧 / Jumbo Frame"));
		ui->label->setText(tr("巨型帧 / Jumbo Frame"));
		ui->CBX_displacement->setText(tr("骨骼坐标 / Skeleton Coordinates"));
		ui->label_2->setText(tr("骨骼坐标 / Skeleton Coordinates"));
		ui->CBX_rtSensorAccel->setText(tr("加速度 / Acceleration"));
		ui->CBX_rtSensorLAccel->setText(tr("线性加速度 / Linear Acceleration"));
		ui->CBX_rtSensorGyro->setText(tr("角速度 / Angular Velocity"));
		ui->CBX_rtSensorMag->setText(tr("磁力值 / Magnetic Field"));
		ui->CBX_rtSensorFlex->setText(tr("弯曲传感器 / Flex Sensor"));
	}
	
	// Update label texts
	if (ui) {
		ui->LBL_skeleton_hint->setText(tr("双手32段骨骼模型。 / Dual hand 32-segment skeleton model."));
	}
}

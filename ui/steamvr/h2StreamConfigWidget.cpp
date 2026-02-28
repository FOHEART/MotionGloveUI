#include "../../include/h2StreamConfigWidget.h"

#include "h2sdk.h"
#include "ui_h2StreamConfigWidget.h"

#include "CarbonUIClass.h"
#include "groupboxcollapser.h"



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

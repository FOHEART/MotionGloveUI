/**
 * @file  h2Widget_OKCali.cpp
 * @brief 主界面控件中 OK 校准面板逻辑的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 通过 MotionGloveLLSDK 读写设备配置。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

//#include "stdafx.h"
#include "../include/h2Widget.h"
#include "h2Calibration.h"
#include "h2sdk.h"
#include "core/h2RatioConversion.h"

#if H2_OKCALI_FUNC_EN

void h2Widget::on_HS_thumbRatio_LH_Changed(int value)
{
	int val = ui.HS_thumbRatio_LH->value();
	float percentVal = (float)val / 100.0;
	//m_pActorModel->setLeftHandRatio(0, percentVal);
	ui.LBL_LH_ThumbRatio->setText(QString::number(val) + QObject::tr("%"));
	{
		H2Calibration_Config v;
		H2_SDK_getOKCali(m_sn, &v);

		v.lefthandThumbRatio = static_cast<float>(h2core::sliderValueToRatioOffset(val));
		H2_SDK_setOKCali(m_sn, v);
	}

	printf("HS_thumbRatio_LH %f\r\n", percentVal);
}

void h2Widget::on_HS_thumbRatio_LH_Released()
{
	writeOKCaliConfigToFile();
	/*if (m_pActorModel)
	{
	m_pActorModel->writeGloveFigureRatioDataLR(true, true);
	}*/
}

void h2Widget::on_HS_indexRatio_LH_Changed(int value)
{
	int val = ui.HS_indexRatio_LH->value();
	float percentVal = (float)val / 100.0;
	//m_pActorModel->setLeftHandRatio(1, percentVal);
	ui.LBL_LH_IndexRatio->setText(QString::number(val) + QObject::tr("%"));
	{
		H2Calibration_Config v;
		H2_SDK_getOKCali(m_sn, &v);

		v.lefthandIndexRatio = static_cast<float>(h2core::sliderValueToRatioOffset(val));
		H2_SDK_setOKCali(m_sn, v);
	}
	printf("HS_indexRatio_LH %f\r\n", percentVal);

}

void h2Widget::on_HS_indexRatio_LH_Released()
{
	/*if (m_pActorModel)
	{
	m_pActorModel->writeGloveFigureRatioDataLR(true, true);
	}*/
	writeOKCaliConfigToFile();
}



void h2Widget::on_HS_thumbRatio_RH_Changed(int value)
{
	int val = ui.HS_thumbRatio_RH->value();
	float percentVal = (float)val / 100.0;
	//m_pActorModel->setRightHandRatio(0, percentVal);
	ui.LBL_RH_ThumbRatio->setText(QString::number(val) + QObject::tr("%"));
	printf("HS_thumbRatio_RH %f\r\n", percentVal);
}

void h2Widget::on_HS_thumbRatio_RH_Released()
{
	/*if (m_pActorModel)
	{
	m_pActorModel->writeGloveFigureRatioDataLR(true, true);
	}*/
}

void h2Widget::on_HS_indexRatio_RH_Changed(int value)
{
	int val = ui.HS_indexRatio_RH->value();
	float percentVal = (float)val / 100.0;
	//m_pActorModel->setRightHandRatio(1, percentVal);
	ui.LBL_RH_IndexRatio->setText(QString::number(val) + QObject::tr("%"));
	printf("HS_indexRatio_RH %f\r\n", percentVal);
}

void h2Widget::on_HS_indexRatio_RH_Released()
{
	/*if (m_pActorModel)
	{
	m_pActorModel->writeGloveFigureRatioDataLR(true, true);
	}*/
}
#endif
#if H2_OKCALI_FUNC_EN

void h2Widget::on_PB_LH_okCali_clicked()
{
	//取出最大最小值
	int thumbMinValue = ui.HS_thumbRatio_LH->minimum();
	int thumbMaxValue = ui.HS_thumbRatio_LH->maximum();
	int indexMinValue = ui.HS_indexRatio_LH->minimum();
	int indexMaxValue = ui.HS_indexRatio_LH->maximum();

	if (m_lefthand_ok_caliTimer.isActive())
		return;

	m_lefthand_ok_caliTimer.setInterval(200);
	m_lefthand_ok_caliTimer.start();


	H2_SDK_startOKCali(m_sn, true, false);
	/*for (int i = 0; i < 5; i++)
	m_pActorModel->setLeftHandRatio(i, 1);



	m_pActorModel->GetH1OKCalibration()->startCali_LH();
	m_pActorModel->GetH1OKCalibration()->process_LH();*/

	{
		ui.PBR_okCaliProcess_LH->setValue(0);
		ui.HS_thumbRatio_LH->setEnabled(false);
		ui.HS_indexRatio_LH->setEnabled(false);
		ui.PB_LH_okCali->setEnabled(false);
		ui.PB_LH_okCaliReset->setEnabled(false);
		QObject::disconnect(ui.HS_thumbRatio_LH, SIGNAL(valueChanged(int)), this, SLOT(on_HS_thumbRatio_LH_Changed(int)));
		QObject::disconnect(ui.HS_indexRatio_LH, SIGNAL(valueChanged(int)), this, SLOT(on_HS_indexRatio_LH_Changed(int)));
	}
}

void h2Widget::on_PB_LH_okCaliReset_clicked()
{
	H2Calibration_Config v;
	H2_SDK_getOKCali(m_sn, &v);

	v.lefthandThumbRatio = 0;
	v.lefthandIndexRatio = 0;
	H2_SDK_setOKCali(m_sn, v);

	{
		int val = h2core::ratioOffsetToSliderValue(
			static_cast<int>(std::round(v.lefthandThumbRatio)));
		ui.HS_thumbRatio_LH->setValue(val);
		ui.LBL_LH_ThumbRatio->setText(QString::number(val) + QObject::tr("%"));
	}
		{
			int val = h2core::ratioOffsetToSliderValue(
				static_cast<int>(std::round(v.lefthandIndexRatio)));
			ui.HS_indexRatio_LH->setValue(val);
			ui.LBL_LH_IndexRatio->setText(QString::number(val) + QObject::tr("%"));
		}

		writeOKCaliConfigToFile();
}

void h2Widget::on_PB_RH_okCali_clicked()
{

}

void h2Widget::on_PB_RH_okCaliReset_clicked()
{

}

void h2Widget::on_PB_LH_okCaliBoth_clicked()
{

}

#endif
#if H2_OKCALI_FUNC_EN

void h2Widget::on_m_lefthand_ok_caliTimer_timeout()
{
	H2Calibration_Config v;
	H2_SDK_getOKCali(m_sn, &v);



	int val = h2core::ratioOffsetToSliderValue(
		static_cast<int>(std::round(v.lefthandThumbRatio)));
	ui.HS_thumbRatio_LH->setValue(val);
	ui.LBL_LH_ThumbRatio->setText(QString::number(val) + QObject::tr("%"));

	if (v.lefthandCalibrationDone == 1)
	{

		ui.PBR_okCaliProcess_LH->setValue(100);
		ui.HS_thumbRatio_LH->setEnabled(true);
		ui.HS_indexRatio_LH->setEnabled(true);
		ui.PB_LH_okCali->setEnabled(true);
		ui.PB_LH_okCaliReset->setEnabled(true);
		QObject::connect(ui.HS_thumbRatio_LH, SIGNAL(valueChanged(int)), this, SLOT(on_HS_thumbRatio_LH_Changed(int)));
		QObject::connect(ui.HS_indexRatio_LH, SIGNAL(valueChanged(int)), this, SLOT(on_HS_indexRatio_LH_Changed(int)));

		m_lefthand_ok_caliTimer.stop();


		writeOKCaliConfigToFile();
	}
}

void h2Widget::on_m_righthand_ok_caliTimer_timeout()
{

}

void h2Widget::on_lefthand_ok_caliResetTimer()
{

}

void h2Widget::on_righthand_ok_caliResetTimer()
{

}

#endif
#if H2_OKCALI_FUNC_EN
void h2Widget::writeOKCaliConfigToFile(void)
{
	H2Calibration_Config v;
	H2_SDK_getOKCali(m_sn, &v);

	//写入配置

	x2Suit_TypeDef x2Mgr;

	bool isFind = m_x2SuitMgr->getCurrentX2SuitBySn(m_sn >> 8, x2Mgr);
	if (!isFind)
	{
		/*没找到*/
		return;
	}
	x2Mgr.m_h2GloveMgr.angleAdditionEuler[KHS32_LeftHandThumb1][1] = v.lefthandThumbRatio;
	x2Mgr.m_h2GloveMgr.angleAdditionEuler[KHS32_LeftHandIndex1][1] = v.lefthandIndexRatio;

	m_x2SuitMgr->updateX2SuitMgrVec(x2Mgr);
}
#endif
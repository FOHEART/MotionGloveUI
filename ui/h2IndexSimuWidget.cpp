//#include "stdafx.h"
#include "h2IndexSimuWidget.h"
#include "h2sdk.h"

h2IndexSimuWidget::h2IndexSimuWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.CBX_leftHandEn, SIGNAL(toggled(bool)), this, SLOT(onLeftHandEn(bool)));
	QObject::connect(ui.CBX_rightHandEn, SIGNAL(toggled(bool)), this, SLOT(onRightHandEn(bool)));


	QObject::connect(ui.HS_thumb1_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_thumb2_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_thumb3_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));


	QObject::connect(ui.HS_index1_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_index2_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_index3_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));


	QObject::connect(ui.HS_middle1_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_middle2_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_middle3_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));


	QObject::connect(ui.HS_ring1_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_ring2_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_ring3_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));


	QObject::connect(ui.HS_pinky1_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_pinky2_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
	QObject::connect(ui.HS_pinky3_LH, SIGNAL(valueChanged(int)), this, SLOT(onHS_thumb1_LHChanged(int)));
}

h2IndexSimuWidget::~h2IndexSimuWidget()
{
}

void h2IndexSimuWidget::updateUI()
{
	if (H2_SDK_getSimulatorValveIndex_LeftHand_En(m_sn))
	{
		ui.CBX_leftHandEn->setChecked(true);
	}
	else
	{
		ui.CBX_leftHandEn->setChecked(false);
	}

	if (H2_SDK_getSimulatorValveIndex_RightHand_En(m_sn))
	{
		ui.CBX_rightHandEn->setChecked(true);
	}
	else
	{
		ui.CBX_rightHandEn->setChecked(false);
	}
}

void h2IndexSimuWidget::onLeftHandEn(bool bCheck)
{
	H2_SDK_setSimulatorValveIndex_LeftHand_En(m_sn, bCheck);
	if (bCheck)
	{
		ui.HS_thumb1_LH->setEnabled(false);
		ui.HS_thumb2_LH->setEnabled(false);
		ui.HS_thumb3_LH->setEnabled(false);

		ui.HS_index1_LH->setEnabled(false);
		ui.HS_index2_LH->setEnabled(false);
		ui.HS_index3_LH->setEnabled(false);

		ui.HS_middle1_LH->setEnabled(false);
		ui.HS_middle2_LH->setEnabled(false);
		ui.HS_middle3_LH->setEnabled(false);

		ui.HS_ring1_LH->setEnabled(false);
		ui.HS_ring2_LH->setEnabled(false);
		ui.HS_ring3_LH->setEnabled(false);

		ui.HS_pinky1_LH->setEnabled(false);
		ui.HS_pinky2_LH->setEnabled(false);
		ui.HS_pinky3_LH->setEnabled(false);
	}
	else
	{
		ui.HS_thumb1_LH->setEnabled(true);
		ui.HS_thumb2_LH->setEnabled(true);
		ui.HS_thumb3_LH->setEnabled(true);

		ui.HS_index1_LH->setEnabled(true);
		ui.HS_index2_LH->setEnabled(true);
		ui.HS_index3_LH->setEnabled(true);

		ui.HS_middle1_LH->setEnabled(true);
		ui.HS_middle2_LH->setEnabled(true);
		ui.HS_middle3_LH->setEnabled(true);

		ui.HS_ring1_LH->setEnabled(true);
		ui.HS_ring2_LH->setEnabled(true);
		ui.HS_ring3_LH->setEnabled(true);

		ui.HS_pinky1_LH->setEnabled(true);
		ui.HS_pinky2_LH->setEnabled(true);
		ui.HS_pinky3_LH->setEnabled(true);
	}
}

void h2IndexSimuWidget::onRightHandEn(bool bCheck)
{
	H2_SDK_setSimulatorValveIndex_RightHand_En(m_sn, bCheck);

}

void h2IndexSimuWidget::onHS_thumb1_LHChanged(int val)
{
	getUIValue_LeftHand();
}

void h2IndexSimuWidget::getUIValue_LeftHand(void)
{
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 0, (float)ui.HS_thumb1_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 1, (float)ui.HS_thumb2_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 2, (float)ui.HS_thumb3_LH->value() / 100);


	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 3, (float)ui.HS_index1_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 4, (float)ui.HS_index2_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 5, (float)ui.HS_index3_LH->value() / 100);

	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 6, (float)ui.HS_middle1_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 7, (float)ui.HS_middle2_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 8, (float)ui.HS_middle3_LH->value() / 100);

	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 9, (float)ui.HS_ring1_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 10, (float)ui.HS_ring2_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 11, (float)ui.HS_ring3_LH->value() / 100);

	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 12, (float)ui.HS_pinky1_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 13, (float)ui.HS_pinky2_LH->value() / 100);
	H2_SDK_setSimulatorValveIndex_LeftHand_setValue(m_sn, 14, (float)ui.HS_pinky3_LH->value() / 100);
}

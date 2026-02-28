//#include "stdafx.h"
#include "h2GestureContainer.h"
#include <functional>
#include "h2GestureCompare.h"

h2GestureContainer::h2GestureContainer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);




}

h2GestureContainer::~h2GestureContainer()
{
}

#if 0
std::function<void(QLayout *layout)> func_clear = [&](QLayout *layout)
{
	int count = layout->layout()->count();
	for (int j = 0; j < count; j++)
	{
		QLayoutItem *item = layout->layout()->itemAt(0);
		QWidget *wid = item->widget();
		QLayout *layoutItem = item->layout();
		if (wid)
		{
			layout->removeWidget(wid);//先移除控件
			//wid->setHidden(true);//再设置不可见
			delete wid;
		}
		if (layoutItem)
		{
			func_clear(layoutItem);//递归检查layout下的子控件情况
			layout->removeItem(layoutItem);
			delete layoutItem;
		}
	}
};
#endif

void h2GestureContainer::generateUI(void)
{
	if (m_gloveSN == 0)
		return;
	//先删除布局中的所有label
	//func_clear(ui.VL_LH);
	//func_clear(ui.VL_RH);

	int gestureCount = 0;
	char gestureName[32][32] = { 0 };
	float minMaxVal[32][10] = { 0 };

	//左右手都一样 这里用左手的
	H2_SDK_getGestureManager(
		m_gloveSN,
		&gestureCount,
		&gestureName[0][0],//32*32 array
		&minMaxVal[0][0]
		);

	for (int i = 0; i < gestureCount; i++)
	{
		h2GestureCompare *t = new h2GestureCompare();
		std::string name(&gestureName[i][0]);
		t->setName(name);
		t->setIsLeftHand(true);

		t->setMinMaxVal(&minMaxVal[i][0]);
		ui.VL_LH->addWidget(t);
	}
	for (int i = 0; i < gestureCount; i++)
	{
		h2GestureCompare *t = new h2GestureCompare();
		std::string name(&gestureName[i][0]);
		t->setName(name);
		t->setIsRightHand(true);
		t->setMinMaxVal(&minMaxVal[i][0]);
		ui.VL_RH->addWidget(t);
	}
}

void h2GestureContainer::updateFigureVal(float *euler_LH/*5 array */,
	float *euler_RH)
{
	{
		ui.LBL_thumbEulerLH->setText(QString::number(euler_LH[0]));
		ui.LBL_indexEulerLH->setText(QString::number(euler_LH[1]));
		ui.LBL_middleEulerLH->setText(QString::number(euler_LH[2]));
		ui.LBL_ringEulerLH->setText(QString::number(euler_LH[3]));
		ui.LBL_pinkyEulerLH->setText(QString::number(euler_LH[4]));

		QProgressBar *PGBArray[5] = {
			ui.PGB_thumbEulerLH,
			ui.PGB_indexEulerLH,
			ui.PGB_middleEulerLH,
			ui.PGB_ringEulerLH,
			ui.PGB_pinkyEulerLH
		};
		for (int i = 0; i < 5; i++)
		{

			int progressVal = 0;
			if (euler_LH[i] < 0)
				progressVal = 0;
			else if (euler_LH[i] > 180)
				progressVal = 180;
			else
				progressVal = euler_LH[i];
			PGBArray[i]->setValue(progressVal);
		}
	}
	{
		ui.LBL_thumbEulerRH->setText(QString::number(euler_RH[0]));
		ui.LBL_indexEulerRH->setText(QString::number(euler_RH[1]));
		ui.LBL_middleEulerRH->setText(QString::number(euler_RH[2]));
		ui.LBL_ringEulerRH->setText(QString::number(euler_RH[3]));
		ui.LBL_pinkyEulerRH->setText(QString::number(euler_RH[4]));

		QProgressBar *PGBArray[5] = {
			ui.PGB_thumbEulerRH,
			ui.PGB_indexEulerRH,
			ui.PGB_middleEulerRH,
			ui.PGB_ringEulerRH,
			ui.PGB_pinkyEulerRH
		};
		for (int i = 0; i < 5; i++)
		{

			int progressVal = 0;
			if (euler_RH[i] < 0)
				progressVal = 0;
			else if (euler_RH[i] > 180)
				progressVal = 180;
			else
				progressVal = euler_RH[i];
			PGBArray[i]->setValue(progressVal);
		}
	}


}

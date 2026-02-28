#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2IndexSimuWidget.h"

class  h2IndexSimuWidget : public QWidget
{
	Q_OBJECT

public:
	h2IndexSimuWidget(QWidget *parent = Q_NULLPTR);
	~h2IndexSimuWidget();

	unsigned int getSn() const { return m_sn; }
	void setSn(unsigned int val) { m_sn = val; }

	void updateUI();

private slots:
void onLeftHandEn(bool bCheck);
void onRightHandEn(bool bCheck);

void onHS_thumb1_LHChanged(int val);

private:
	Ui::h2IndexSimuWidget ui;

	unsigned int m_sn=0;

	void getUIValue_LeftHand(void);
};

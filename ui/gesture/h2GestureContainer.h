#pragma once
#include <QtWidgets/QWidget>
#include "ui_h2GestureContainer.h"
#include "h2sdk.h"

class  h2GestureContainer : public QWidget
{
	Q_OBJECT

public:
	h2GestureContainer(QWidget *parent = Q_NULLPTR);
	~h2GestureContainer();

	unsigned int getGloveSN() const { return m_gloveSN; }
	void setGloveSN(unsigned int val) { m_gloveSN = val; }

	void generateUI(void);
	//5根手指的实时值
	void updateFigureVal(float *euler_LH/*5 array */,
		float *euler_RH);
private:
	unsigned int m_gloveSN = 0;

private:
	Ui::h2GestureContainer ui;
};

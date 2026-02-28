#pragma once

#include <QWidget>
#include "ui_h2Notusable.h"

class h2Notusable : public QWidget
{
	Q_OBJECT

public:
	h2Notusable(QWidget *parent = nullptr);
	~h2Notusable();

private:
	Ui::h2NotusableClass ui;
};


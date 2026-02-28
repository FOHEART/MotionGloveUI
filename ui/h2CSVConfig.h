#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2CSVConfig.h"

class  h2CSVConfig : public QWidget
{
	Q_OBJECT

public:
	h2CSVConfig(QWidget *parent = Q_NULLPTR);
	~h2CSVConfig();

private:
	Ui::h2CSVConfig ui;
};

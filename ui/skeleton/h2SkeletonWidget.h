#pragma once

#include <QWidget>
#include "ui_h2SkeletonWidget.h"

class h2SkeletonWidget : public QWidget
{
	Q_OBJECT

public:
	h2SkeletonWidget(QWidget *parent = nullptr);
	~h2SkeletonWidget();

private:
	Ui::h2SkeletonWidgetClass ui;
	void populateSkeletonTree();
	void applyStylesheet();
};


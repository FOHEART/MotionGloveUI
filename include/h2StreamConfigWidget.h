#pragma once
#include "libh2ui_global.h"

#include <QtWidgets/QWidget>
#include <memory>

namespace Ui { class h2StreamConfigWidget; }

class LIBH2UI_API h2StreamConfigWidget : public QWidget
{
	Q_OBJECT

public:
	h2StreamConfigWidget(QWidget *parent = Q_NULLPTR);
	~h2StreamConfigWidget();

	void writeH2StreamConfig(void);


private:
	std::unique_ptr<Ui::h2StreamConfigWidget> ui;
	void updateUIFromConfig(void);

};

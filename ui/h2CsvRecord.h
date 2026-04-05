#pragma once

#include <QWidget>
#include "ui_h2CsvRecord.h"

class h2CsvRecord : public QWidget
{
	Q_OBJECT

public:
	h2CsvRecord(QWidget *parent = nullptr);
	~h2CsvRecord();

	unsigned int getSn() const { return m_sn; }
	void setSn(unsigned int sn);

private:
	Ui::h2CsvRecordClass ui;
	unsigned int m_sn = 0;
};


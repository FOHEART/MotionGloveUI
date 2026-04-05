#include "h2CsvRecord.h"

h2CsvRecord::h2CsvRecord(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

h2CsvRecord::~h2CsvRecord()
{}

void h2CsvRecord::setSn(unsigned int sn)
{
	m_sn = sn;
}


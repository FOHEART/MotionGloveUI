#include "h2CsvRecord.h"
#include "h2sdk.h"

#include <QFileDialog>
#include <QObject>

h2CsvRecord::h2CsvRecord(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	syncCsvRecordDirToUi();
	QObject::connect(ui.PB_setCsvRecordDir, &QPushButton::clicked, this, [this]()
	{
		onSetCsvRecordDirClicked();
	});
}

h2CsvRecord::~h2CsvRecord()
{}

void h2CsvRecord::setSn(unsigned int sn)
{
	m_sn = sn;
}

void h2CsvRecord::syncCsvRecordDirToUi()
{
	const char* csvDir = H2_SDK_getCSVRecordFilesDir();
	printf("Current CSV Record Directory: %s\r\n", csvDir);
	ui.LE_csvRecordDir->setText(QString::fromUtf8(csvDir));
}

void h2CsvRecord::onSetCsvRecordDirClicked()
{
	const QString currentDir = ui.LE_csvRecordDir->text().trimmed();
	const QString selectedDir = QFileDialog::getExistingDirectory(
		this,
		tr("Select CSV Record Directory"),
		currentDir.isEmpty() ? QString::fromUtf8(H2_SDK_getCSVRecordFilesDir()) : currentDir);

	if (selectedDir.isEmpty())
	{
		return;
	}

	if (H2_SDK_setCSVRecordFilesDir(selectedDir.toUtf8().constData()) == 0)
	{
		ui.LE_csvRecordDir->setText(selectedDir);
		return;
	}

	syncCsvRecordDirToUi();
}

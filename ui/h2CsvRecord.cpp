#include "h2CsvRecord.h"
#include "h2sdk.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QFontMetrics>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include "CarbonUIClass.h"

namespace
{
QString formatFileNameForDisplay(const QString& text, int maxCharsPerLine = 40)
{
	if (text.isEmpty() || maxCharsPerLine <= 0)
	{
		return text;
	}

	QStringList lines;
	for (int i = 0; i < text.size(); i += maxCharsPerLine)
	{
		lines << text.mid(i, maxCharsPerLine);
	}
	return lines.join("\n");
}
}

h2CsvRecord::h2CsvRecord(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	{
		CarbonUIClass::setWidgetLabelsStyleRecursively(this);
		CarbonUIClass::applyButtonStyle(ui.PB_setCsvRecordDir, CarbonUIClass::Shape_RoundAll);
		CarbonUIClass::applyButtonStyle(ui.PB_startRecord, CarbonUIClass::Shape_RoundAll);
		CarbonUIClass::applyButtonStyle(ui.PB_stopRecord, CarbonUIClass::Shape_RoundAll);
		CarbonUIClass::applyButtonStyle(ui.PB_openFileDir, CarbonUIClass::Shape_RoundAll);

		CarbonUIClass::applyLineEditStyle_numberInput(ui.LE_csvRecordDir, 250);
	}
	ui.LBL_fileName->setWordWrap(true);
	ui.LBL_fileName->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	const int fileNameLabelHeight = QFontMetrics(ui.LBL_fileName->font()).lineSpacing() * 5;
	ui.LBL_fileName->setMinimumHeight(fileNameLabelHeight);
	ui.LBL_fileName->setMaximumHeight(fileNameLabelHeight);

	syncCsvRecordDirToUi();
	syncRecordButtonsToUi();
	QObject::connect(ui.PB_setCsvRecordDir, &QPushButton::clicked, this, [this]()
					 { onSetCsvRecordDirClicked(); });
	QObject::connect(ui.PB_openFileDir, &QPushButton::clicked, this, [this]()
					 { onOpenFileDirClicked(); });
	QObject::connect(ui.PB_startRecord, &QPushButton::clicked, this, [this]()
					 { onStartRecordClicked(); });
	QObject::connect(ui.PB_stopRecord, &QPushButton::clicked, this, [this]()
					 { onStopRecordClicked(); });
}

h2CsvRecord::~h2CsvRecord()
{
}

void h2CsvRecord::setSn(unsigned int sn)
{
	m_sn = sn;
}

void h2CsvRecord::syncCsvRecordDirToUi()
{
	const char *csvDir = H2_SDK_getCSVRecordFilesDir();
	printf("Current CSV Record Directory: %s\r\n", csvDir);
	ui.LE_csvRecordDir->setText(QString::fromUtf8(csvDir));
}

void h2CsvRecord::syncRecordButtonsToUi()
{
	const bool isRecording = (H2_SDK_isCSVRecording() == 1);
	ui.PB_startRecord->setEnabled(!isRecording);
	ui.PB_stopRecord->setEnabled(isRecording);
	ui.PB_setCsvRecordDir->setEnabled(!isRecording);
	ui.LE_csvRecordDir->setEnabled(!isRecording);
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

void h2CsvRecord::onOpenFileDirClicked()
{
	const QString dirPath = ui.LE_csvRecordDir->text().trimmed();
	if (dirPath.isEmpty())
	{
		return;
	}

	QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
}

void h2CsvRecord::onStartRecordClicked()
{
	char actorName[128] = {0};
	if (m_sn == 0)
	{
		printf("h2CsvRecord::onStartRecordClicked failed: sn is 0\r\n");
		return;
	}

	if (H2_SDK_getGloveName(m_sn, actorName) != 0 || actorName[0] == 0)
	{
		printf("h2CsvRecord::onStartRecordClicked failed: cannot resolve actor name for sn=%u\r\n", m_sn);
		return;
	}

	char fullFileName[256] = { 0 };
	const int ret = H2_SDK_startCSVRecord(actorName, fullFileName);
	printf("h2CsvRecord::onStartRecordClicked, sn=%u, actorName=%s, file=%s, ret=%d\r\n", m_sn, actorName, fullFileName, ret);
	if (ret == 0)
	{
		ui.LBL_fileName->setText(formatFileNameForDisplay(QString::fromUtf8(fullFileName), 40));
	}
	syncRecordButtonsToUi();
}

void h2CsvRecord::onStopRecordClicked()
{
	const int ret = H2_SDK_stopCSVRecord();
	printf("h2CsvRecord::onStopRecordClicked, sn=%u, ret=%d\r\n", m_sn, ret);
	syncRecordButtonsToUi();
}

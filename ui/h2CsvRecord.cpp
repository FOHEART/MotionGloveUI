/**
 * @file  h2CsvRecord.cpp
 * @brief CSV 录制控件的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；控件样式统一由 CarbonUIClass 提供；通过 MotionGloveLLSDK 读写设备配置；界面布局由同名 .ui 文件生成。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

#include "h2CsvRecord.h"
#include "h2sdk.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QFontMetrics>
#include <QCoreApplication>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include "CarbonUIClass.h"
#include "core/h2TextWrap.h"

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

	setupLanguageChangeConnection();
	retranslateUI();
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

void h2CsvRecord::setupLanguageChangeConnection()
{
	if (!QCoreApplication::instance())
	{
		return;
	}

	for (int i = 0; i < QCoreApplication::instance()->children().size(); ++i)
	{
		QObject* obj = QCoreApplication::instance()->children()[i];
		if (obj && obj->inherits("LanguageManager"))
		{
			connect(obj, SIGNAL(languageChanged(QString)), this, SLOT(retranslateUI()));
			QVariant langVar = obj->property("currentLanguage");
			if (langVar.isValid())
			{
				m_language = langVar.toString();
			}
			break;
		}
	}
}

void h2CsvRecord::retranslateUI()
{
	if (m_language == "en")
	{
		ui.label->setText("CSV Record Directory:");
		ui.PB_setCsvRecordDir->setText("Change");
		ui.PB_startRecord->setText("Record");
		ui.PB_stopRecord->setText("Stop");
		ui.label_3->setText("File Name:");
		ui.PB_openFileDir->setText("Open File Directory");
	}
	else
	{
		ui.label->setText("录制CSV文件路径：");
		ui.PB_setCsvRecordDir->setText("更改");
		ui.PB_startRecord->setText("录制");
		ui.PB_stopRecord->setText("停止");
		ui.label_3->setText("文件名称：");
		ui.PB_openFileDir->setText("打开文件路径");
	}
}

void h2CsvRecord::setLanguage(const QString& lang)
{
	m_language = lang;
	retranslateUI();
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
		(m_language == "en") ? QStringLiteral("Select CSV Record Directory") : QStringLiteral("选择CSV录制目录"),
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
		ui.LBL_fileName->setText(h2core::wrapFixedWidth(QString::fromUtf8(fullFileName), 40));
	}
	syncRecordButtonsToUi();
}

void h2CsvRecord::onStopRecordClicked()
{
	const int ret = H2_SDK_stopCSVRecord();
	printf("h2CsvRecord::onStopRecordClicked, sn=%u, ret=%d\r\n", m_sn, ret);
	syncRecordButtonsToUi();
}

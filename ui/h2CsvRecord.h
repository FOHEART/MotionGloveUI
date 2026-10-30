/**
 * @file  h2CsvRecord.h
 * @brief CSV 录制控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QWidget>
#include "ui_h2CsvRecord.h"

/**
 * @brief CSV 录制控件：选择录制目录、开始/停止录制并显示当前文件名。
 *
 * @details 录制本身由 SDK 完成，本控件只负责目录选择、按钮状态与文件名展示。
 *          界面由 uic 生成的 `ui_h2CsvRecord.h` 提供。
 */
class h2CsvRecord : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件：应用 CarbonUI 样式、绑定按钮信号并同步当前录制状态。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2CsvRecord(QWidget *parent = nullptr);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2CsvRecord();

	/**
	 * @brief 读取当前绑定的套装序列号。
	 * @return 序列号原始值，0 表示未绑定。
	 */
	unsigned int getSn() const { return m_sn; }

	/**
	 * @brief 设置绑定的套装序列号。
	 * @param[in] sn 序列号原始值，0 表示未绑定；不校验取值范围。
	 */
	void setSn(unsigned int sn);

	/**
	 * @brief 订阅 LanguageManager 的语言切换信号，触发 retranslateUI()。
	 * @note 应用里没有名为 `LanguageManager` 的子对象时什么也不做。
	 */
	void setupLanguageChangeConnection();
	/**
	 * @brief 直接指定界面语言并立即刷新。
	 * @param[in] lang 语言代码，取值 `"en"` 或 `"zh"`；其他值按中文处理。
	 */
	void setLanguage(const QString& lang);

private slots:
	/** @brief 按 m_language 刷新按钮与标签文案。 */
	void retranslateUI();

private:
	/** @brief 把当前录制目录同步到只读输入框。 */
	void syncCsvRecordDirToUi();

	/** @brief 依据录制是否进行中，切换开始/停止按钮的可用性。 */
	void syncRecordButtonsToUi();

	/** @brief 「更改目录」按钮：弹出目录选择框并写回配置。 */
	void onSetCsvRecordDirClicked();
	/** @brief 「打开文件路径」按钮：用系统文件管理器打开录制目录。 */
	void onOpenFileDirClicked();

	/** @brief 「录制」按钮：按当前目录开始录制。 */
	void onStartRecordClicked();

	/** @brief 「停止」按钮：停止录制并把文件名显示到界面。 */
	void onStopRecordClicked();

	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2CsvRecordClass ui;
	/** @brief 绑定的套装序列号，0 表示未绑定。 */
	unsigned int m_sn = 0;
	/** @brief 当前界面语言，取值 `"en"` 或 `"zh"`，默认中文。 */
	QString m_language = "zh";
};


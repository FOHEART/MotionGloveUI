#pragma once

#include <QWidget>
#include "ui_h2CsvRecord.h"

class h2CsvRecord : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief Construct the CSV record widget.
	 * @brief 中文: 构造 CSV 录制控件。
	 *
	 * @param parent Parent widget
	 */
	h2CsvRecord(QWidget *parent = nullptr);

	/**
	 * @brief Destroy the CSV record widget.
	 * @brief 中文: 析构 CSV 录制控件。
	 */
	~h2CsvRecord();

	/**
	 * @brief Get the current suit serial number.
	 * @brief 中文: 获取当前套装序列号。
	 *
	 * @return unsigned int Current suit serial number
	 */
	unsigned int getSn() const { return m_sn; }

	/**
	 * @brief Set the current suit serial number.
	 * @brief 中文: 设置当前套装序列号。
	 *
	 * @param sn Suit serial number
	 */
	void setSn(unsigned int sn);

private:
	/**
	 * @brief Synchronize the CSV record directory into the line edit.
	 * @brief 中文: 将当前 CSV 录制目录同步到界面输入框。
	 */
	void syncCsvRecordDirToUi();

	/**
	 * @brief Synchronize start/stop button enabled state from record status.
	 * @brief 中文: 根据录制状态同步开始/停止按钮的可用性。
	 */
	void syncRecordButtonsToUi();

	/**
	 * @brief Handle the click on the set-directory button.
	 * @brief 中文: 处理设置录制目录按钮的点击事件。
	 */
	void onSetCsvRecordDirClicked();
	void onOpenFileDirClicked();

	/**
	 * @brief Handle the click on the start-record button.
	 * @brief 中文: 处理开始录制按钮的点击事件。
	 */
	void onStartRecordClicked();

	/**
	 * @brief Handle the click on the stop-record button.
	 * @brief 中文: 处理停止录制按钮的点击事件。
	 */
	void onStopRecordClicked();

	Ui::h2CsvRecordClass ui;
	unsigned int m_sn = 0;
};


/**
 * @file  h2Notusable.h
 * @brief 占位控件 h2Notusable 的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QWidget>
#include "ui_h2Notusable.h"

/**
 * @brief 「不可用」占位控件：功能未实现或当前平台不支持时显示。
 *
 * @details 界面由 uic 生成的 `ui_h2Notusable.h` 提供，只做文案展示。
 */
class h2Notusable : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件并执行界面初始化。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2Notusable(QWidget *parent = nullptr);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2Notusable();

	/**
	 * @brief 按当前语言刷新静态文案。
	 * @post 调用后界面文本与 LanguageManager 的当前语言一致。
	 */
	void retranslateUI();

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2NotusableClass ui;
};


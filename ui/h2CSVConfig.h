/**
 * @file  h2CSVConfig.h
 * @brief CSV 相关配置面板控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2CSVConfig.h"

/**
 * @brief CSV 配置页的占位控件。
 *
 * @details 界面由 uic 生成的 `ui_h2CSVConfig.h` 提供，当前没有任何业务逻辑，
 *          仅为保持界面结构完整而存在。
 */
class  h2CSVConfig : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件并执行 uic 生成的界面初始化。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2CSVConfig(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2CSVConfig();

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2CSVConfig ui;
};

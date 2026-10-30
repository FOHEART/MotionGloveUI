/**
 * @file  h2SkeletonWidget.h
 * @brief 手部骨骼显示控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QWidget>
#include "ui_h2SkeletonWidget.h"

/**
 * @brief 手部骨骼层级树面板。
 *
 * @details 用 QTreeWidget 展示 SDK 给出的骨骼结构，方便排查父子关系；
 *          界面由 uic 生成的 `ui_h2SkeletonWidget.h` 提供。
 */
class h2SkeletonWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件、建立骨骼树并应用样式表。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2SkeletonWidget(QWidget *parent = nullptr);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2SkeletonWidget();

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2SkeletonWidgetClass ui;
	/**
	 * @brief 按 SDK 提供的骨骼层级填充树控件。
	 * @post 重复调用会先清空再重建，不会累积重复节点。
	 */
	void populateSkeletonTree();
	/** @brief 应用深色主题样式表（背景、字色、选中色）。 */
	void applyStylesheet();
};


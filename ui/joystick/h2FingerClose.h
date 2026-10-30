/**
 * @file  h2FingerClose.h
 * @brief 握拳/张手状态显示控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2FingerClose.h"

/**
 * @brief 手指/握持动作状态指示控件。
 *
 * @details 把 SDK 给出的握拳、抓取、捏合等布尔状态渲染成高亮/灰暗样式，
 *          左右手各用一个实例。所有 set 接口只记录状态，需再调 updateUI() 刷新。
 */
class  h2FingerClose : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件并执行界面初始化。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2FingerClose(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2FingerClose();

	/** @brief 标记本实例代表左手，并清除右手标记。 */
	void setIsLeftHand(void);
	/** @brief 标记本实例代表右手，并清除左手标记。 */
	void setIsRightHand(void);

	/** @brief 设置第 2 根手指闭合状态。@param[in] s true 表示闭合。 */
	void setIsFinger2Close(bool s);
	/** @brief 设置第 3 根手指闭合状态。@param[in] s true 表示闭合。 */
	void setIsFinger3Close(bool s);
	/** @brief 设置第 4 根手指闭合状态。@param[in] s true 表示闭合。 */
	void setIsFinger4Close(bool s);
	/** @brief 设置第 5 根手指闭合状态。@param[in] s true 表示闭合。 */
	void setIsFinger5Close(bool s);

	/** @brief 设置扳机状态。@param[in] s true 表示已扣下。 */
	void setIsTrigger(bool s);
	/** @brief 设置抓取状态。@param[in] s true 表示抓取中。 */
	void setIsGrab(bool s);
	/** @brief 设置捏合状态。@param[in] s true 表示捏合中。 */
	void setIsPinch(bool s);
	/** @brief 设置菜单手势状态。@param[in] s true 表示触发中。 */
	void setIsMenu(bool s);


	/**
	 * @brief 依据全部状态位刷新界面样式。
	 * @post 处于激活状态的项使用 downStyleSheet，否则使用 upStyleSheet。
	 */
	void updateUI(void);
private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2FingerClose ui;
	/** @brief 是否为左手实例。 */
	bool isLeftHand=false;
	/** @brief 是否为右手实例。 */
	bool isRightHand = false;

	/** @brief 第 2 根手指是否闭合。 */
	bool isFinger2Close = false;
	/** @brief 第 3 根手指是否闭合。 */
	bool isFinger3Close = false;
	/** @brief 第 4 根手指是否闭合。 */
	bool isFinger4Close = false;
	/** @brief 第 5 根手指是否闭合。 */
	bool isFinger5Close = false;

	/** @brief 扳机是否扣下。 */
	bool isTrigger = false;
	/** @brief 是否处于抓取状态。 */
	bool isGrab = false;
	/** @brief 是否处于捏合状态。 */
	bool isPinch = false;
	/** @brief 是否触发菜单手势。 */
	bool isMenu = false;

	/** @brief 未激活时的样式表。 */
	QString upStyleSheet;
	/** @brief 激活时的样式表。 */
	QString downStyleSheet;
};

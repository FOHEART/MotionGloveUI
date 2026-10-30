/**
 * @file  h2JoystickABXY.h
 * @brief 手柄 ABXY 按键状态显示控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once
#include <QtWidgets/QWidget>
#include "ui_h2JoystickABXY.h"

/**
 * @brief 手柄 ABXY 四个按键的状态指示控件。
 *
 * @details 每个按键有两种样式表（按下 / 弹起），updateUI() 依据内部状态切换。
 */
class  h2JoystickABXY : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件并执行界面初始化。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2JoystickABXY(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2JoystickABXY();

	/** @brief 查询 A 键是否按下。@return true 表示按下。 */
	bool getIsADown() const { return isADown; }
	/** @brief 设置 A 键按下状态。@param[in] val true 表示按下；需再调用 updateUI() 才会刷新界面。 */
	void setIsADown(bool val) { isADown = val; }
	/** @brief 查询 B 键是否按下。@return true 表示按下。 */
	bool getIsBDown() const { return isBDown; }
	/** @brief 设置 B 键按下状态。@param[in] val true 表示按下；需再调用 updateUI() 才会刷新界面。 */
	void setIsBDown(bool val) { isBDown = val; }
	/** @brief 查询 X 键是否按下。@return true 表示按下。 */
	bool getIsXDown() const { return isXDown; }
	/** @brief 设置 X 键按下状态。@param[in] val true 表示按下；需再调用 updateUI() 才会刷新界面。 */
	void setIsXDown(bool val) { isXDown = val; }
	/** @brief 查询 Y 键是否按下。@return true 表示按下。 */
	bool getIsYDown() const { return isYDown; }
	/** @brief 设置 Y 键按下状态。@param[in] val true 表示按下；需再调用 updateUI() 才会刷新界面。 */
	void setIsYDown(bool val) { isYDown = val; }

	/**
	 * @brief 依据四个按键的当前状态刷新界面样式。
	 * @post 按下与弹起的按键分别使用 downStyleSheet / upStyleSheet。
	 */
	void updateUI(void);
private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2JoystickABXY ui;

	/** @brief A 键是否按下。 */
	bool isADown = false;
	/** @brief B 键是否按下。 */
	bool isBDown = false;
	/** @brief X 键是否按下。 */
	bool isXDown = false;
	/** @brief Y 键是否按下。 */
	bool isYDown = false;

	/** @brief 按键弹起时的样式表。 */
	QString upStyleSheet;
	/** @brief 按键按下时的样式表。 */
	QString downStyleSheet;
};

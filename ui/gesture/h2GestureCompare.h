/**
 * @file  h2GestureCompare.h
 * @brief 手势幅度对比显示控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>

#include "ui_h2GestureCompare.h"

/**
 * @brief 单只手的五根手指「幅度区间」对比显示控件。
 *
 * @details 每根手指一行，显示该手指幅度的最小值与最大值；
 *          左右手的显示顺序不同：左手按小指到拇指，右手按拇指到小指。
 */
class   h2GestureCompare : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件并执行界面初始化。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2GestureCompare(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2GestureCompare();

	/**
	 * @brief 设置显示在控件顶部的名称。
	 * @param[in] name 名称文本，UTF-8 编码；允许空串。
	 */
	void setName(std::string name);

	/**
	 * @brief 写入五根手指的幅度区间并刷新标签。
	 * @param[in] v 长度为 10 的浮点数组，**不可为 nullptr**，布局为
	 *              `{min0, max0, min1, max1, ..., min4, max4}`，
	 *              i 从 0 到 4 依次对应小指、无名指、中指、食指、拇指
	 *              （右手显示时会反序排到界面上）。
	 * @pre 必须先调用 setIsLeftHand(true) 或 setIsRightHand(true)，
	 *      两者都为 false 时内部标签指针尚未初始化。
	 * @warning 左右手标志都为 false 而直接调用本函数会解引用未初始化的
	 *          指针并崩溃。
	 */
	void setMinMaxVal(float *v);

	/** @brief 查询是否按左手布局显示。@return true 表示左手。 */
	bool getIsLeftHand() const { return isLeftHand; }
	/**
	 * @brief 设置是否按左手布局显示。
	 * @param[in] val true 表示左手。
	 * @note 需在 setMinMaxVal() 之前调用；本函数不刷新已有标签。
	 */
	void setIsLeftHand(bool val) { isLeftHand = val; }
	/** @brief 查询是否按右手布局显示。@return true 表示右手。 */
	bool getIsRightHand() const { return isRightHand; }
	/**
	 * @brief 设置是否按右手布局显示。
	 * @param[in] val true 表示右手。
	 * @note 需在 setMinMaxVal() 之前调用；本函数不刷新已有标签。
	 */
	void setIsRightHand(bool val) { isRightHand = val; }

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2GestureCompare ui;
	/** @brief 五根手指的幅度区间，`[i][0]` 为最小值、`[i][1]` 为最大值。 */
	float minMaxVal[5][2];
	/** @brief 按显示槽位排列的最小值标签，槽位顺序取决于左右手标志。 */
	QLabel *handLabelMin[5];
	/** @brief 按显示槽位排列的最大值标签，槽位顺序取决于左右手标志。 */
	QLabel *handLabelMax[5];

	/** @brief 是否按左手布局显示。 */
	bool isLeftHand=false;
	/** @brief 是否按右手布局显示。 */
	bool isRightHand = false;
};

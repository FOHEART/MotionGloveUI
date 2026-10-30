/**
 * @file  h2GestureContainer.h
 * @brief 手势列表容器控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once
#include <QtWidgets/QWidget>
#include "ui_h2GestureContainer.h"
#include "h2sdk.h"

/**
 * @brief 手势调试窗口：集中展示左右手五根手指的实时值与当前手势。
 *
 * @details 由一个 h2GestureDisplay（左右手各一个）与若干 h2GestureCompare
 *          组成，界面由 uic 生成的 `ui_h2GestureContainer.h` 提供。
 */
class  h2GestureContainer : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造窗口并执行界面初始化。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2GestureContainer(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2GestureContainer();

	/** @brief 读取窗口绑定的套装序列号。@return 序列号原始值，0 表示未绑定。 */
	unsigned int getGloveSN() const { return m_gloveSN; }

	/**
	 * @brief 设置窗口绑定的套装序列号。
	 * @param[in] val 序列号原始值，0 表示未绑定。
	 */
	void setGloveSN(unsigned int val) { m_gloveSN = val; }

	/**
	 * @brief 按当前序列号重建窗口内的手势条目。
	 * @post 重复调用会先清理旧条目再生成，不会重复叠加。
	 */
	void generateUI(void);

	/**
	 * @brief 刷新左右手五根手指的实时值。
	 * @param[in] euler_LH 左手五指数据，长度为 5 的数组，**不可为 nullptr**。
	 * @param[in] euler_RH 右手五指数据，长度为 5 的数组，**不可为 nullptr**。
	 * @note 元素顺序与 SDK 的手指索引一致（小指在前、拇指在后）。
	 */
	void updateFigureVal(float *euler_LH/*5 array */,
		float *euler_RH);
private:
	/** @brief 窗口绑定的套装序列号，0 表示未绑定。 */
	unsigned int m_gloveSN = 0;

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2GestureContainer ui;
};

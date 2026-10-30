/**
 * @file  h2IndexSimuWidget.h
 * @brief 手指索引仿真面板控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2IndexSimuWidget.h"

/**
 * @brief 手指 Index 传感器仿真调试面板。
 *
 * @details 用于在没有真实硬件的情况下手动给出左右手 Index 通道的数值，
 *          观察下游显示是否正常。界面由 uic 生成的 `ui_h2IndexSimuWidget.h` 提供。
 */
class  h2IndexSimuWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件并执行界面初始化。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2IndexSimuWidget(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2IndexSimuWidget();

	/** @brief 读取当前绑定的套装序列号。@return 序列号原始值，0 表示未绑定。 */
	unsigned int getSn() const { return m_sn; }

	/**
	 * @brief 设置要仿真的套装序列号。
	 * @param[in] val 序列号原始值，0 表示未绑定。
	 */
	void setSn(unsigned int val) { m_sn = val; }

	/**
	 * @brief 从当前仿真值刷新界面显示。
	 * @note 只在 UI 线程调用；不写回 SDK 配置。
	 */
	void updateUI();

private slots:
	/**
	 * @brief 左手仿真开关变化。
	 * @param[in] bCheck true 表示启用左手仿真。
	 */
void onLeftHandEn(bool bCheck);
	/**
	 * @brief 右手仿真开关变化。
	 * @param[in] bCheck true 表示启用右手仿真。
	 */
void onRightHandEn(bool bCheck);

	/**
	 * @brief 左手拇指 Index 滑杆变化。
	 * @param[in] val 滑杆的新值，单位与滑杆刻度一致（整数）。
	 */
void onHS_thumb1_LHChanged(int val);

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2IndexSimuWidget ui;

	/** @brief 当前仿真的套装序列号，0 表示未绑定。 */
	unsigned int m_sn=0;

	/** @brief 读取左手各滑杆当前值并同步到界面标签。 */
	void getUIValue_LeftHand(void);
};

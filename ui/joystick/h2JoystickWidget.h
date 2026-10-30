/**
 * @file  h2JoystickWidget.h
 * @brief 摇杆综合面板控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QString>
#include <QtCore/QTimer>

// Hide the generated UI header from consumers of this widget by forward
// declaring the UI class and keeping the real include inside the .cpp.
namespace Ui { class h2JoystickWidget; }

/**
 * @brief 摇杆综合面板：姿态盘 + 按键状态 + 死区设置 + 开/关摇杆操作。
 *
 * @details 以 60 Hz 定时器从 SDK 拉取摇杆数据并刷新子控件；
 *          界面定义放在 `.cpp` 里包含，避免把 uic 生成头暴露给使用方。
 */
class  h2JoystickWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造面板并启动刷新定时器。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2JoystickWidget(QWidget *parent = Q_NULLPTR);

	/** @brief 析构：停止定时器并释放界面对象。 */
	~h2JoystickWidget();

	/** @brief 读取绑定的套装序列号。@return 序列号原始值，0 表示未绑定。 */
	unsigned int getGloveSN() const { return m_gloveSN; }
	/** @brief 设置绑定的套装序列号。@param[in] val 序列号原始值，0 表示未绑定。 */
	void setGloveSN(unsigned int val) { m_gloveSN = val; }

	/** @brief 查询本实例是否代表左手。@return true 表示左手。 */
	bool getIsLeftHand() const { return m_bIsLeftHand; }
	/**
	 * @brief 设置本实例是否代表左手。
	 * @param[in] val true 表示左手；与 setIsRightHand() 互斥设置。
	 */
	void setIsLeftHand(bool val);
	/** @brief 查询本实例是否代表右手。@return true 表示右手。 */
	bool getIsRightHand() const { return m_bIsRightHand; }
	/**
	 * @brief 设置本实例是否代表右手。
	 * @param[in] val true 表示右手；与 setIsLeftHand() 互斥设置。
	 */
	void setIsRightHand(bool val);

	/** @brief 查询面板是否启用。@return true 表示启用。 */
	bool getIsEnable() const { return m_bIsEnable; }
	/**
	 * @brief 启用或停用整个面板（整页置灰）。
	 * @param[in] val true 表示启用。
	 */
	void setIsEnable(bool val);

	/** @brief 查询摇杆硬件是否存在。@return true 表示存在。 */
	bool getIsJoyStickExist() const { return m_bJoyStickExist; }
	/**
	 * @brief 设置摇杆硬件是否存在，并据此使能操作按钮。
	 * @param[in] val true 表示存在。
	 */
	void setIsJoyStickExist(bool val);

	/**
	 * @brief 从 SDK 读出摇杆配置并回填界面（死区等）。
	 * @note 只读取、不写回；设备不可用时保持界面原状。
	 */
	void readGloveConfigFileToUI(void);

	

private slots:
	/** @brief 刷新定时器回调：读取摇杆数据并刷新姿态盘与按键状态。 */
	void onH2JoystickTimer();
	/**
	 * @brief 死区滑杆拖动中回调。
	 * @param[in] value 滑杆的新值，单位与滑杆刻度一致（0 到最大值）。
	 */
	void onHS_JSDeadZone_LH_Changed(int value);
	/** @brief 死区滑杆松开：把当前值写回配置。 */
	void onHS_JSDeadZone_LH_Released();

	/** @brief 「打开摇杆」按钮：向 SDK 下发开启命令。 */
	void on_PB_openOpenGlove_clicked(void);
	/** @brief 「关闭摇杆」按钮：向 SDK 下发关闭命令。 */
	void on_PB_closeOpenGlove_clicked(void);
private:
	/** @brief uic 生成的界面对象，由析构函数负责 delete。 */
	Ui::h2JoystickWidget* ui = nullptr;

	/** @brief 绑定的套装序列号，0 表示未绑定。 */
	unsigned int m_gloveSN = 0;
	/** @brief 是否为左手实例。 */
	bool m_bIsLeftHand = false;
	/** @brief 是否为右手实例。 */
	bool m_bIsRightHand = false;

	/** @brief 面板是否启用。 */
	bool m_bIsEnable = false;
	/** @brief 摇杆硬件是否存在。 */
	bool m_bJoyStickExist = false;


	/** @brief 表示「已开启」的按钮样式表。 */
	QString pbStylesheetGreen ;
	/** @brief 表示「已关闭」的按钮样式表。 */
	QString pbStylesheetRed ;

	/** @brief 轮询摇杆数据的定时器。 */
	QTimer* h2JoyStickTimer = nullptr;
	/** @brief 轮询频率，单位 Hz，默认 60。 */
	unsigned int h2JoyStickTimerHz = 60;
	/** @brief 是否显示 Index 模拟调试按钮。 */
	bool m_isIndexSimuDebug = false;
private:
	/**
	 * @brief 启动刷新定时器。
	 * @post 定时器已在运行时重复调用无副作用。
	 */
	void startUpdateUITimer(void);
	/**
	 * @brief 停止刷新定时器。
	 * @post 定时器未运行时调用无副作用。
	 */
	void stopUpdateUITimer(void);

};

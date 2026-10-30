/**
 * @file  h2JoystickCali.h
 * @brief 摇杆校准控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
#include "ui_h2JoystickCali.h"

/**
 * @brief 摇杆校准功能的状态机取值。
 * @details 0 为未校准，负值不使用；本枚举不参与位运算。
 */
typedef enum{
	JS_CaliFunction_None,  ///< 当前没有进行中的校准。
	JS_CaliFunction_Center,///< 正在进行中心校准（摇杆回中）。
	JS_CaliFunction_Rot    ///< 正在进行旋转校准（绕轴转一圈）。
}JS_CaliFunction;



/**
 * @brief 摇杆校准面板：引导用户完成中心校准与旋转校准，并播放对应动画。
 *
 * @details 校准过程中按定时器轮询 SDK 的进度，结束后恢复空白图；
 *          左右手各用一个实例。动画素材来自模块目录下的 gif。
 */
class  h2JoystickCali : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件：加载空白图与两段校准动画，绑定进度定时器。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2JoystickCali(QWidget *parent = Q_NULLPTR);

	/** @brief 析构：释放动画与图片对象。 */
	~h2JoystickCali();

	/** @brief 读取绑定的套装序列号。@return 序列号原始值，0 表示未绑定。 */
	unsigned int getSn() const { return m_sn; }
	/** @brief 设置绑定的套装序列号。@param[in] val 序列号原始值，0 表示未绑定。 */
	void setSn(unsigned int val) { m_sn = val; }

	/**
	 * @brief 从配置恢复已保存的校准值并刷新界面。
	 * @post 设备存在时界面显示配置中的校准参数；设备不存在时保持原状。
	 */
	void restoreCaliValueFromConfig(void);

	/** @brief 查询本实例是否代表左手。@return true 表示左手。 */
	bool getIsLeftHand() const { return isLeftHand; }
	/**
	 * @brief 标记本实例代表左手。
	 * @param[in] val true 表示左手；调用方负责保证与右手标记互斥。
	 */
	void setIsLeftHand(bool val) { isLeftHand = val; }

	/** @brief 查询本实例是否代表右手。@return true 表示右手。 */
	bool getIsRightHand() const { return isRightHand; }
	/**
	 * @brief 标记本实例代表右手。
	 * @param[in] val true 表示右手；调用方负责保证与左手标记互斥。
	 */
	void setIsRightHand(bool val) { isRightHand = val; }

private:
	/**
	 * @brief 取本模块所在目录，用于定位校准动画与图片。
	 * @return 目录绝对路径，结尾不带分隔符；非 Windows 平台返回空串。
	 */
	std::string GetLocalDllDirectory();

	/** @brief 显示「等待校准」的静态占位图。 */
	void showBlankImage();
	/** @brief 显示中心校准动画并开始播放。 */
	void showCenterCaliImage();
	/** @brief 显示旋转校准动画并开始播放。 */
	void showRotCaliImage();

	public slots:
	/**
	 * @brief 校准进度定时器回调：推进计数并根据是否完成切换界面状态。
	 * @note 由 m_caliTimer 周期触发（构造时固定 200 ms 量级）。
	 */
	void on_caliTimer_timeout();
	/** @brief 「中心校准」按钮：启动中心校准流程。 */
	void on_PB_joystickCaliCenter_clicked();
	/** @brief 「旋转校准」按钮：启动旋转校准流程。 */
	void on_PB_joystickCaliRot_clicked();

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2JoystickCali ui;

	/** @brief 轮询校准进度的定时器。 */
	QTimer m_caliTimer;
	/** @brief 绑定的套装序列号，0 表示未绑定。 */
	unsigned int m_sn = 0;

	/** @brief 是否为左手实例。 */
	bool isLeftHand = false;
	/** @brief 是否为右手实例。 */
	bool isRightHand = false;

	/** @brief 当前正在进行的校准类型。 */
	JS_CaliFunction m_js_caliFunc = JS_CaliFunction_None;

	/** @brief 中心校准动画。 */
	QMovie *movie_centerCali=nullptr;
	/** @brief 旋转校准动画。 */
	QMovie *movie_rotCali = nullptr;

	/** @brief 等待校准时的占位图。 */
	QPixmap *pixmap_blank = nullptr; 
private:
	/** @brief 中心校准已轮询的次数。 */
	unsigned int m_centerCaliCounter = 0;
	/** @brief 中心校准的轮询次数上限，达到即判定结束。 */
	unsigned int m_centerCaliCounterMax = 4;

	/** @brief 旋转校准已轮询的次数。 */
	unsigned int m_rotCaliCounter = 0;
	/** @brief 旋转校准的轮询次数上限，达到即判定结束。 */
	unsigned int m_rotCaliCounterMax = 16;
};

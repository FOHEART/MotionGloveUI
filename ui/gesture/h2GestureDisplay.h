/**
 * @file  h2GestureDisplay.h
 * @brief 单个手势显示控件的声明。
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
#include <map>
#include "ui_h2GestureDisplay.h"

/**
 * @brief 单只手的当前手势图标显示控件。
 *
 * @details 定时（默认 5 Hz）从 SDK 读取手势名，再从预加载的图片表里取对应
 *          图标显示出来；左右手各用一个实例。
 */
class h2GestureDisplay : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件、预加载左右手手势图标并启动刷新定时器。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2GestureDisplay(QWidget *parent = Q_NULLPTR);

	/** @brief 析构：停止定时器并释放界面对象。 */
	~h2GestureDisplay();
	/** @brief 读取绑定的套装序列号。@return 序列号原始值，0 表示未绑定。 */
	unsigned int getGloveSN() const { return m_gloveSN; }
	/** @brief 设置绑定的套装序列号。@param[in] val 序列号原始值，0 表示未绑定。 */
	void setGloveSN(unsigned int val) { m_gloveSN = val; }

	/** @brief 查询当前实例是否代表左手。@return true 表示左手。 */
	bool getIsLeftHand() const { return m_bIsLeftHand; }
	/**
	 * @brief 设置当前实例是否代表左手。
	 * @param[in] val true 表示左手；与 setIsRightHand() 互斥设置。
	 */
	void setIsLeftHand(bool val);
	/** @brief 查询当前实例是否代表右手。@return true 表示右手。 */
	bool getIsRightHand() const { return m_bIsRightHand; }
	/**
	 * @brief 设置当前实例是否代表右手。
	 * @param[in] val true 表示右手；与 setIsLeftHand() 互斥设置。
	 */
	void setIsRightHand(bool val);

	/** @brief 查询控件当前是否启用。@return true 表示启用。 */
	bool getIsEnable() const { return m_bIsEnable; }
	/**
	 * @brief 启用或停用控件。
	 * @param[in] val true 表示启用；停用后整体置灰。
	 */
	void setIsEnable(bool val);

	/**
	 * @brief 从 SDK 读出手势配置并回填界面。
	 * @note 只读取、不写回；设备不可用时保持界面原状。
	 */
	void readGloveConfigFileToUI(void);

private slots:
	/**
	 * @brief 刷新定时器回调：读取当前手势名并切换图标。
	 * @note 由 h2GestureTimer 周期触发，频率见 h2GestureTimerHz。
	 */
	void onH2UpdateGestureTimer();

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2GestureDisplay ui;
	/** @brief 绑定的套装序列号，0 表示未绑定。 */
	unsigned int m_gloveSN = 0;
	/** @brief 是否为左手实例。 */
	bool m_bIsLeftHand = false;
	/** @brief 是否为右手实例。 */
	bool m_bIsRightHand = false;
	/** @brief 控件是否启用。 */
	bool m_bIsEnable = false;
	/** @brief 刷新手势图标的定时器。 */
	QTimer* h2GestureTimer = nullptr;
	/** @brief 刷新频率，单位 Hz，默认 5。 */
	unsigned int h2GestureTimerHz = 5;

	/** @brief 左手手势名到图标的缓存。 */
	std::map<std::string, QPixmap> m_gesturePixmapMap_LH;
	/** @brief 右手手势名到图标的缓存。 */
	std::map<std::string, QPixmap> m_gesturePixmapMap_RH;

private:
	/**
	 * @brief 取本模块所在目录，用于定位 Resources 下的手势图片。
	 * @return 目录绝对路径，结尾不带分隔符；非 Windows 平台返回空串。
	 */
	std::string GetLocalDllDirectory();
	/**
	 * @brief 把指定手的全部手势图片加载进缓存。
	 * @param[in] isLeftHand  true 表示加载左手图片。
	 * @param[in] isRightHand true 表示加载右手图片。
	 * @note 两者都为 false 时不加载任何图片。
	 */
	void addGestureDisplayPicture(bool isLeftHand, bool isRightHand);
};

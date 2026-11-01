/**
 * @file  h2FingerAdjustEnhancedWidget.h
 * @brief 手指角度调节（增强模式）面板控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2FingerAdjustEnhancedWidget.h"
#include "h2sdk.h"

/**
 * @brief 手指角度调节面板（增强模式）。
 *
 * @details 用滑杆调节手指张开/握拳幅度与拇指角度，改动实时写入 MotionGloveLLSDK
 *          的套装配置；左右手各用一个实例。界面由 uic 生成的
 *          `ui_h2FingerAdjustEnhancedWidget.h` 提供。
 */
class h2FingerAdjustEnhancedWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造面板：绑定滑杆信号、拦截滚轮事件并应用 CarbonUI 样式。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2FingerAdjustEnhancedWidget(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2FingerAdjustEnhancedWidget();
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
	 * @brief 启用或停用整个面板。
	 * @param[in] val true 表示启用；停用后滑杆不可操作。
	 */
	void setIsEnable(bool val);

	/**
	 * @brief 从 SDK 读出当前套装的设置并回填滑杆。
	 * @note 只读取、不写回；设备不可用时保持界面原状。
	 */
	void readGloveConfigFileToUI(void);

	/**
	 * @brief 把本面板当前的滑杆值写进套装配置结构体（不写盘）。
	 * @param[in,out] suit 目标配置；只覆盖本面板负责的字段，其余字段保持原值。
	 * @note 混合模式的滑杆写进 `*FingerEnhancedModeFistAngle`（与该模式落盘字段一致），
	 *       拇指 X/Y/Z 写进 `angleAdditionEuler[KHS32_*HandThumb1][0..2]`，单位 度（°）。
	 *       注意不要写成 `*FingerAngleStretch`：那是分指锁定的字段，会互相覆盖。
	 */
	void applyUIToConfig(h2GloveMgr& suit) const;

	/**
	 * @brief 订阅 LanguageManager 的语言切换信号，触发 retranslateUI()。
	 * @note 应用里没有名为 `LanguageManager` 的子对象时什么也不做。
	 */
	void setupLanguageChangeConnection();
	
	/**
	 * @brief 直接指定界面语言并立即刷新。
	 * @param[in] lang 语言代码，取值 `"en"` 或 `"zh"`；其他值按中文处理。
	 */
	void setLanguage(const QString& lang);
protected:
	/**
	 * @brief 拦截滑杆上的滚轮事件，避免误滚改值。
	 * @param[in] obj 事件目标；仅对已安装过滤器的滑杆生效。
	 * @param[in] ev  事件对象，**不可为 nullptr**。
	 * @return true 表示事件被吞掉，false 表示继续默认处理。
	 */
	bool eventFilter(QObject* obj, QEvent* ev) override;
private slots:
	/** @brief 「重置角度」按钮：把当前手的手指角度恢复默认。 */
	void on_PB_rotResetLockYaw_clicked();
	/** @brief 「撤销重置」按钮：撤销上一次重置。 */
	void on_PB_rotResetLockYaw_undo_clicked();

	/**
	 * @brief 张开幅度滑杆拖动中回调。
	 * @param[in] value 滑杆新值，单位与滑杆刻度一致。
	 */
	void onSliderFigureAngleStretchChanged(int value);

	/** @brief 张开幅度滑杆松开：把当前值写回配置。 */
	void onSliderFigureAngleStretchReleased();


	/**
	 * @brief 拇指引子滑杆拖动中回调。
	 * @param[in] value 滑杆新值，单位与滑杆刻度一致（角度值）。
	 */
	void  onSliderThumbFigureAngleZChanged(int value);
	/** @brief 拇指引子滑杆松开：把当前值写回配置。 */
	void onSliderThumbFigureAngleZReleased();
private slots:
	/** @brief 按 m_language 刷新面板内所有静态文案。 */
	void retranslateUI();
private:
	/**
	 * @brief 给分组框安装折叠器。
	 * @post 重复调用只在首次真正安装，之后直接返回。
	 */
	void ensureCollapserInstalled();

	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2FingerAdjustEnhancedWidget ui;
	/** @brief 分组框折叠器是否已安装。 */
	bool m_collapserInstalled = false;
	/** @brief 绑定的套装序列号，0 表示未绑定。 */
	unsigned int m_gloveSN = 0;
	/** @brief 是否为左手实例。 */
	bool m_bIsLeftHand = false;
	/** @brief 是否为右手实例。 */
	bool m_bIsRightHand = false;
	/** @brief 面板是否启用。 */
	bool m_bIsEnable = false;
	
	/** @brief 当前界面语言，取值 `"en"` 或 `"zh"`，默认中文。 */
	QString m_language = "zh";  // Current language ("en" or "zh")

private:
	/**
	 * @brief 取本模块所在目录，用于定位 Resources 下的图标。
	 * @return 目录绝对路径，结尾不带分隔符；非 Windows 平台返回空串。
	 */
	std::string GetLocalDllDirectory();
};

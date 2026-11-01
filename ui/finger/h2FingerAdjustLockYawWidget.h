/**
 * @file  h2FingerAdjustLockYawWidget.h
 * @brief 手指角度调节（锁定偏航）面板控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2FingerAdjustLockYawWidget.h"
#include "h2sdk.h"

/**
 * @brief 手指角度调节面板（锁定偏航模式）。
 *
 * @details 与另外两种模式相比多了「张开 / 握拳」两条角度滑杆与拇指 Y/Z 调节，
 *          改动实时写入 MotionGloveLLSDK 的套装配置；左右手各用一个实例。
 */
class  h2FingerAdjustLockYawWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造面板：绑定滑杆与按钮信号、拦截滚轮事件并应用 CarbonUI 样式。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2FingerAdjustLockYawWidget(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2FingerAdjustLockYawWidget();
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
	 * @param[in] val true 表示启用；停用后分组框整体置灰。
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
	 * @note 分指张开写 `*FingerAngleStretch`、握拳写 `*FingerAngleFist`、
	 *       拇指 X/Y/Z 写进 `angleAdditionEuler[KHS32_*HandThumb1][0..2]`，单位 度（°）。
	 */
	void applyUIToConfig(h2GloveMgr& suit) const;

	/**
	 * @brief 控制配置项左侧说明标签（如「张开」「握拳」）是否显示。
	 * @param[in] isShow true 表示显示。
	 */
	void setShowConfigItemHeader(bool isShow);

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

private slots:
	/** @brief 「重置偏航」按钮：把当前手的旋转恢复默认。 */
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
	 * @brief 握拳幅度滑杆拖动中回调。
	 * @param[in] value 滑杆新值，单位与滑杆刻度一致。
	 */
	void onSliderFigureAngleFistChanged(int value);
	/** @brief 握拳幅度滑杆松开：把当前值写回配置。 */
	void onSliderFigureAngleFistReleased();

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
protected:
	/**
	 * @brief 拦截滑杆上的滚轮事件，避免误滚改值。
	 * @param[in] obj 事件目标；仅对已安装过滤器的滑杆生效。
	 * @param[in] ev  事件对象，**不可为 nullptr**。
	 * @return true 表示事件被吞掉，false 表示继续默认处理。
	 */
	bool eventFilter(QObject* obj, QEvent* ev) override;

private:
	/**
	 * @brief 给分组框安装折叠器。
	 * @post 重复调用只在首次真正安装，之后直接返回。
	 */
	void ensureCollapserInstalled();

	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2FingerAdjustLockYawWidget ui;
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
	/** @brief 是否显示配置项标题标签（如「张开」「握拳」）。 */
	bool m_bShowConfigItemHeader = true;
	/** @brief 当前界面语言，取值 `"en"` 或 `"zh"`，默认中文。 */
	QString m_language = "zh";
private:
	/**
	 * @brief 取本模块所在目录，用于定位 Resources 下的图标。
	 * @return 目录绝对路径，结尾不带分隔符；非 Windows 平台返回空串。
	 */
	std::string GetLocalDllDirectory();

};

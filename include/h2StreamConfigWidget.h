/**
 * @file  h2StreamConfigWidget.h
 * @brief 串流配置控件的公开接口。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；含 Q_OBJECT，需 moc 生成元对象代码。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once
#include "libh2ui_global.h"

#include <QtWidgets/QWidget>
#include <memory>

namespace Ui { class h2StreamConfigWidget; }

/**
 * @brief 转发格式配置页：选择数据以何种格式转发出去。
 *
 * @details 界面由 uic 生成的 `ui_h2StreamConfigWidget.h` 提供；
 *          配置的读取与写回都通过 MotionGloveLLSDK 完成，
 *          控件本身不保存配置副本。
 *          「骨骼段数」分组把 `h2GloveStreamConfig::skeletonFormat`
 *          暴露成 32 段 / 42 段 / OpenXR 三选一，构造时按 SDK 当前值回填，
 *          `writeH2StreamConfig()` 时写回。
 */
class LIBH2UI_API h2StreamConfigWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件并从 SDK 读取当前配置回填界面。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2StreamConfigWidget(QWidget *parent = Q_NULLPTR);

	/** @brief 析构；unique_ptr 成员自动释放界面对象。 */
	~h2StreamConfigWidget();

	/**
	 * @brief 把界面上的选择写回 SDK 流配置。
	 * @post 调用返回后 SDK 中的流配置即为界面所示内容。
	 * @note 无返回值，写入失败不会反馈给调用方。
	 */
	void writeH2StreamConfig(void);

	/**
	 * @brief 订阅 LanguageManager 的语言切换信号，触发 retranslateUI()。
	 * @note 若应用里没有名为 `LanguageManager` 的子对象，则什么也不做。
	 */
	void setupLanguageChangeConnection();

private:
	/** @brief uic 生成的界面对象；用 unique_ptr 保证异常安全。 */
	std::unique_ptr<Ui::h2StreamConfigWidget> ui;
	/**
	 * @brief 从 SDK 读回流配置并刷新界面控件。
	 * @note 只读取、不写回，可与 writeH2StreamConfig() 安全地成对调用。
	 */
	void updateUIFromConfig(void);

	private slots:
		/** @brief 按当前语言刷新本页所有静态文案。 */
		void retranslateUI();

};

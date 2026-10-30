/**
 * @file  libH2SteamDriverUI.h
 * @brief Steam 驱动界面控件的公开接口。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once
#include "libh2ui_global.h"

#include <QtWidgets/QWidget>
#ifdef _WIN32

// #include "ui_libH2SteamDriverUI.h" // moved to .cpp
namespace Ui { class libH2SteamDriverUI; }

/**
 * @brief SteamVR 驱动管理界面：检测、安装与卸载本地 SteamVR 驱动。
 *
 * @details 界面由 uic 生成的 `ui_libH2SteamDriverUI.h` 提供（该头放在 .cpp 内包含，
 *          因此本头文件只做前置声明）。安装状态用绿色/红色样式表体现在标签上。
 *
 * @note 类体整体位于 `#ifdef _WIN32` 内，非 Windows 平台不提供该控件。
 */
class LIBH2UI_API  libH2SteamDriverUI : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件并立即检测一次驱动安装状态。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	libH2SteamDriverUI(QWidget *parent = Q_NULLPTR);

	/** @brief 析构；释放 uic 生成的界面对象。 */
	~libH2SteamDriverUI();

	/**
	 * @brief 检测本机 SteamVR 驱动是否已安装，并据此刷新状态标签。
	 * @post 标签文本与配色反映当前真实安装状态；本函数不修改任何文件。
	 * @note 只读取注册表/安装目录，耗时为毫秒级，可重复调用。
	 */
	void checkInstallation(void);

private:
	/** @brief uic 生成的界面对象，由析构函数负责 delete。 */
	Ui::libH2SteamDriverUI* ui = nullptr;

	/** @brief 表示「已安装」的标签样式表。 */
	QString lbl_green_styleSheet;
	/** @brief 表示「未安装」的标签样式表。 */
	QString lbl_red_styleSheet;

	public slots:
	/** @brief 「安装本地 SteamVR 驱动」按钮：拷贝驱动文件并注册。 */
	void on_PB_installSteamVRDriverLocal_clicked();
	/** @brief 「卸载本地 SteamVR 驱动」按钮：注销并移除驱动文件。 */
	void on_PB_uninstallSteamVRDriverLocal_clicked();
	/** @brief 「从官网安装」按钮：用浏览器打开 Steam 官方页面。 */
	void on_PB_installSteamFromWeb_clicked();

	private slots:
	/**
	 * @brief 勾选框状态变化时启用或停用驱动。
	 * @param[in] bCheck 勾选后的状态；true 表示启用驱动。
	 */
	void onCheck_CBX_steamVRDriverEnable(bool bCheck);
};

#endif

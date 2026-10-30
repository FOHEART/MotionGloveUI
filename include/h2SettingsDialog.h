/**
 * @file  h2SettingsDialog.h
 * @brief 设置对话框的公开接口。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include "libh2ui_global.h"

#include <QtWidgets/QDialog>
#include <QtGui/QCloseEvent>

namespace Ui { class h2SettingsDialog; }
class h2NetBroadcastWidget;
class h2StreamConfigWidget;

/**
 * @brief 设置对话框：用页签承载「数据转发」与「转发格式」两块配置。
 *
 * @details 打开时创建两个内嵌配置控件并挂到页签上，点「确定」时
 *          把两者的内容一起写回 MotionGloveLLSDK 的流配置，
 *          点「取消」或直接关闭窗口则丢弃。
 *
 * 只在 UI 线程使用；关闭时会主动释放内嵌控件。
 */
class LIBH2UI_API h2SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	/**
	 * @brief 构建设置对话框及其两个内嵌页签。
	 * @param[in] parent 父控件，**可为 nullptr**；为空时对话框独立显示。
	 */
	h2SettingsDialog(QWidget *parent = Q_NULLPTR);

	/** @brief 析构；释放 uic 生成的界面对象。 */
	~h2SettingsDialog();

protected:
	/**
	 * @brief 处理窗口关闭事件：先释放两个内嵌配置控件，再接受关闭。
	 * @param[in,out] event 关闭事件对象，**不可为 nullptr**；
	 *                      函数内部会调用 accept()，调用方无需再处理。
	 * @note 释放内嵌控件是为了让下次打开时重新从 SDK 读取配置，
	 *       避免沿用上一次的残留界面状态。
	 */
	void closeEvent(QCloseEvent *event) override;
private:
	/** @brief uic 生成的界面对象，由析构函数负责 delete。 */
	Ui::h2SettingsDialog* ui = nullptr;
	/** @brief 「数据转发」页签内容，实际类型为 h2NetBroadcastWidget。 */
	QWidget *m_netBroadcastWidget = 0;
	/** @brief 「转发格式」页签内容，实际类型为 h2StreamConfigWidget。 */
	QWidget *m_streamFormatWidget = 0;

	private slots:
		/** @brief 「取消」按钮：直接关闭，不写回任何配置。 */
		void on_PB_Cancle_clicked();
		/**
		 * @brief 「确定」按钮：把两个页签的内容写回 SDK 流配置后关闭。
		 * @warning 这里用 C 风格强制转换还原控件类型，两个成员为空时会崩溃。
		 */
		void on_PB_OK_clicked();
		/** @brief 按当前语言刷新窗口标题与子控件文案。 */
		void retranslateUI();
};

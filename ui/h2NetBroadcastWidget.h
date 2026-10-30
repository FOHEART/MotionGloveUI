/**
 * @file  h2NetBroadcastWidget.h
 * @brief UDP 广播地址配置控件的声明。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 多语言切换由 LanguageManager 的 languageChanged 信号驱动；含 Q_OBJECT，需 moc 生成元对象代码；依赖 uic 生成的 ui_*.h。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once

#include <QtWidgets/QWidget>
#include "ui_h2NetBroadcastWidget.h"

/**
 * @brief UDP 数据转发的目标地址配置控件。
 *
 * @details 允许添加多行「IP + 端口」，并把它们拼接成
 *          `ip:port,ip:port` 形式写入 SDK 的 BroadcastAddr 字段。
 *          同时提供发送帧率选择与本机地址下拉框。
 */
class h2NetBroadcastWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造控件：填充本机地址与帧率选项、从配置恢复地址表。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2NetBroadcastWidget(QWidget *parent = Q_NULLPTR);

	/** @brief 析构。界面对象按值持有，自动析构。 */
	~h2NetBroadcastWidget();

	/**
	 * @brief 把界面上的地址表写回 SDK 流配置。
	 * @post SDK 的 BroadcastAddr 与界面一致；地址表为空时写入空串（等价于关闭广播）。
	 * @note 缓冲区放不下时以「地址项」为单位截断，不会写出半截地址。
	 */
	void writeH2StreamConfig();

	/**
	 * @brief 按当前语言刷新全部静态文案。
	 * @post 按钮、标签与帧率下拉框文本均与当前语言一致。
	 */
	void retranslateUI();
	
	/**
	 * @brief 订阅 LanguageManager 的语言切换信号。
	 * @note 应用里没有名为 `LanguageManager` 的子对象时什么也不做；
	 *       需在控件创建后主动调用一次。
	 */
	void setupLanguageChangeConnection();

private:
	/** @brief uic 生成的界面对象（按值持有）。 */
	Ui::h2NetBroadcastWidget ui;
	
	/** @brief 依据当前语言重排发送帧率下拉框的文本（保留选中项）。 */
	void updateFramerateDisplay();
	/**
	 * @brief 枚举本机 IPv4 地址并填充下拉框，127.0.0.1 恒在首位。
	 * @note 只取到的地址不足两个时会再按主机名解析一次作为兜底。
	 */
	void fillLocalIPAddress();
	/**
	 * @brief 用固定帧率表填充发送帧率下拉框。
	 * @note 候选值为 60 / 30 / 15 Hz。
	 */
	void fillUDPTxFramerate();
	/**
	 * @brief 从 SDK 配置读出已保存的地址表并逐行加入界面。
	 * @note 配置为空时界面保持空白（不写入默认地址）。
	 */
	void loadLocalIPAddressFromConfigFile();
	/**
	 * @brief 向地址表追加一行，并在行尾挂一个删除按钮。
	 * @param[in] ip   地址文本，**不要传空串**（不做校验）。
	 * @param[in] port 端口文本，**不要传空串**（不做校验）。
	 */
	void addIPPortRow(const QString &ip, const QString &port);
	/** @brief 把本控件内所有标签的字色统一设为白色（深色主题适配）。 */
	void setAllLabelsWhite();

private slots:
	/** @brief 「添加地址」按钮：把下拉框选中的地址加入地址表。 */
	void on_addAddrBtn_clicked();
	/**
	 * @brief 删除按钮点击：移除按钮所在的那一行地址。
	 * @note 行指针通过按钮的 `_h2_rowItem` 属性传递。
	 */
	void on_deleteRowButton_clicked();
	/**
	 * @brief 语言切换回调。
	 * @param[in] langCode 新的语言代码，取值 `"en"` 或 `"zh"`。
	 */
	void onLanguageChanged(const QString& langCode);
};

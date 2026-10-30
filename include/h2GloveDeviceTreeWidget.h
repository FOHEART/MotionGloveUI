/**
 * @file  h2GloveDeviceTreeWidget.h
 * @brief 手套设备树控件的公开接口。
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

namespace Ui { class h2GloveDeviceTreeWidget; }

/**
 * @brief 设备列表里的单个手套设备卡片。
 *
 * @details 展示一台设备的图片、名称、左右手在线状态、电量与序列号，
 *          并处理鼠标悬停/点击（点击时发出 @ref suitSelectedLive 信号）。
 *          背景与边框配色随「悬停 / 选中」两种状态变化。
 *
 * 只在 UI 线程使用；内部持有 uic 生成的界面对象并在析构时释放。
 */
class LIBH2UI_API h2GloveDeviceTreeWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造设备卡片并应用默认图片与样式。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2GloveDeviceTreeWidget(QWidget *parent = Q_NULLPTR);

	/** @brief 析构；释放 uic 生成的界面对象。 */
	~h2GloveDeviceTreeWidget();

	/**
	 * @brief 从文件或 Qt 资源路径加载设备图片。
	 * @param[in] path 图片路径，不可为空串；Qt 资源写法形如 `:/images/xxx.png`。
	 *                 路径不存在时 QPixmap 为空，界面显示空白，不报错。
	 */
	void setDeviceImage(const QString &path);

	/**
	 * @brief 直接用现成的位图设置设备图片。
	 * @param[in] pixmap 位图，可为空（空位图等价于不显示图片）。
	 */
	void setDeviceImage(const QPixmap &pixmap);

	/**
	 * @brief 恢复内置默认图片。
	 * @details 优先使用 `:/images/Resources/img/usblink.png`，加载失败时回退到后备图标。
	 * @post 设备图片被替换为默认图，名称与状态文本不受影响。
	 */
	void setDefaultDeviceImage();

	/**
	 * @brief 设置设备名称文本并加粗显示。
	 * @param[in] name 设备名，可为空串（此时标签为空）。
	 */
	void setDeviceName(const QString &name);

	/**
	 * @brief 读取当前设备名称。
	 * @return 设备名；未设置过时为空串。
	 */
	QString deviceName() const;

	/**
	 * @brief 设置左手在线状态并刷新对应图标。
	 * @param[in] online true 表示在线。
	 * @post 左侧图标按在线/离线切换，并更新悬浮提示。
	 */
	void setLeftOnline(bool online);
	/** @brief 查询左手是否在线。@return true 表示在线。 */
	bool isLeftOnline() const;

	/**
	 * @brief 设置右手在线状态并刷新对应图标。
	 * @param[in] online true 表示在线。
	 * @post 右侧图标按在线/离线切换，并更新悬浮提示。
	 */
	void setRightOnline(bool online);
	/** @brief 查询右手是否在线。@return true 表示在线。 */
	bool isRightOnline() const;

	/**
	 * @brief 设置左手电量并刷新电池图标。
	 * @param[in] level 电量百分比，取值 [0, 100]；
	 *                  超出范围不做夹取，会直接存入成员并用于选择图标。
	 */
	void setLeftBattery(int level);
	/** @brief 查询左手电量。@return 电量百分比，默认 0。 */
	int leftBattery() const;

	/**
	 * @brief 设置右手电量并刷新电池图标。
	 * @param[in] level 电量百分比，取值 [0, 100]；超出范围不做夹取。
	 */
	void setRightBattery(int level);
	/** @brief 查询右手电量。@return 电量百分比，默认 0。 */
	int rightBattery() const;

	/**
	 * @brief 设置设备序列号，同时刷新悬浮提示。
	 * @param[in] sn 序列号原始值；提示里以 16 进制大写显示。
	 */
	void setSerialNumber(unsigned int  sn);
	/** @brief 查询设备序列号。@return 序列号原始值，默认 0。 */
	unsigned int serialNumber() const { return m_sn; }

	/**
	 * @brief 设置全局创建序号。
	 * @param[in] index 序号，取值 [-1, INT_MAX]；-1 表示尚未分配。
	 */
	void setCreateIndexGlobal(int index);
	/** @brief 查询全局创建序号。@return 序号，默认 -1 表示未分配。 */
	int createIndexGlobal() const { return m_createIndexGlobal; }

	/**
	 * @brief 设置选中状态。
	 * @param[in] selected true 表示选中；选中时边框使用高亮配色。
	 * @post 状态未变化时直接返回，不触发重绘。
	 */
	void setSelected(bool selected);
	/** @brief 查询是否处于选中状态。@return true 表示已选中。 */
	bool isSelected() const;

	/**
	 * @brief 设置电量标签的固定尺寸。
	 * @param[in] size 宽高，默认 32x16，单位像素；传 0 会导致标签不可见。
	 * @post 尺寸未变化时直接返回。
	 */
	void setBatteryLabelSize(const QSize &size);
	/** @brief 查询电量标签尺寸。@return 当前尺寸，默认 32x16。 */
	QSize batteryLabelSize() const { return m_batteryLabelSize; }

	/**
	 * @brief 设置设备名与左右手图标的尺寸。
	 * @param[in] size 宽高，默认 32x32，单位像素；左右手图标取其一半。
	 */
	void setIconSize(const QSize &size);
	/** @brief 查询图标尺寸。@return 当前尺寸，默认 32x32。 */
	QSize iconSize() const { return m_iconSize; }

	/**
	 * @brief 按当前语言刷新全部静态文案（左右手标签、悬浮提示）。
	 * @post 调用后界面文本与当前语言一致。
	 */
	void retranslateUI();

protected:
	/**
	 * @brief 拦截设备卡片容器的鼠标进入/离开/点击事件。
	 * @param[in] watched 被监视对象；仅处理安装过过滤器的 `ui->widget`。
	 * @param[in] event   事件对象，**不可为 nullptr**。
	 * @return true 表示已消费该事件，false 表示继续默认处理。
	 * @note 点击左键时发出 suitSelectedLive()，携带序列号与设备名。
	 */
	bool eventFilter(QObject *watched, QEvent *event) override;

private:
	/**
	 * @brief 按「选中 / 悬停 / 普通」三态应用卡片边框配色。
	 * @note 选中优先于悬停。
	 */
	void applyFrameStyle();

	/** @brief uic 生成的界面对象，由析构函数负责 delete。 */
	Ui::h2GloveDeviceTreeWidget* ui = nullptr;
	/** @brief 设备名称文本。 */
	QString m_deviceName;
	/** @brief 鼠标是否悬停在卡片上。 */
	bool m_isHovered = false;
	/** @brief 卡片是否被选中。 */
	bool m_isSelected = false;
	/** @brief 左手是否在线。 */
	bool m_isLeftOnline=false;
	/** @brief 右手是否在线。 */
	bool m_isRightOnline=false;
	/** @brief 左手电量百分比，取值 [0, 100]。 */
	int m_leftBattery=0;
	/** @brief 右手电量百分比，取值 [0, 100]。 */
	int m_rightBattery=0;
	/** @brief 设备序列号原始值。 */
	unsigned int  m_sn=0;
	/** @brief 全局创建序号，-1 表示未分配。 */
	int m_createIndexGlobal=-1;

	/** @brief 电量标签尺寸，单位像素，默认 32x16。 */
	QSize m_batteryLabelSize = QSize(32, 16);
	/** @brief 设备名与左右手图标尺寸，单位像素，默认 32x32。 */
	QSize m_iconSize = QSize(32, 32);

	/** @brief 依据 m_isLeftOnline 刷新左侧图标。 */
	void updateLeftImage();
	/** @brief 依据 m_isRightOnline 刷新右侧图标。 */
	void updateRightImage();

	/** @brief 依据 m_leftBattery 刷新左侧电池图标。 */
	void updateLeftBatteryImage();
	/** @brief 依据 m_rightBattery 刷新右侧电池图标。 */
	void updateRightBatteryImage();
	/** @brief 依据序列号与在线状态重建悬浮提示文本。 */
	void updateToolTip();

signals:
	/**
	 * @brief 用户点击本设备卡片时发出。
	 * @param[in] suitNum   被点击设备的序列号原始值。
	 * @param[in] actorName 设备名，UTF-8 编码的 std::string。
	 */
	void suitSelectedLive(const unsigned int& suitNum,
		const std::string& actorName);
};

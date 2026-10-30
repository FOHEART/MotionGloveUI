/**
 * @file  h2GloveDeviceTreeContainerWidget.h
 * @brief 手套设备树容器控件的公开接口。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 含 Q_OBJECT，需 moc 生成元对象代码。
 * 函数级参数与返回值说明见各函数自身的注释块。
 */

#pragma once
#include "libh2ui_global.h"

#include <QtCore/QObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QTimer>

#include <map>

namespace Ui { class h2GloveDeviceTreeContainerWidget; }

/**
 * @brief 设备卡片列表容器：负责增删卡片、定时扫描与单选互斥。
 *
 * @details 卡片（h2GloveDeviceTreeWidget）由外部创建后交给本容器托管，
 *          容器只保留指针并按序列号建索引；选中某个卡片时会取消其他卡片的选中态，
 *          并把该卡片的 suitSelectedLive 信号转发给 onDeviceWidgetSelected()。
 *
 * 增删操作会通过队列槽确保在 UI 线程执行。
 */
class LIBH2UI_API h2GloveDeviceTreeContainerWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief 构造容器并安装内部竖排布局。
	 * @param[in] parent 父控件，**可为 nullptr**。
	 */
	h2GloveDeviceTreeContainerWidget(QWidget *parent = Q_NULLPTR);

	/** @brief 析构；释放布局与扫描定时器，托管的卡片按 Qt 父子关系回收。 */
	~h2GloveDeviceTreeContainerWidget();

	/**
	 * @brief 把一张设备卡片加入容器。
	 * @param[in] widget 卡片控件指针，**不可为 nullptr**；本容器会接管其父子关系。
	 * @note 实际插入通过队列槽完成，调用返回时卡片可能尚未出现在布局中。
	 */
	void addDeviceTreeWidget(QWidget *widget);

	/**
	 * @brief 从容器移除一张设备卡片。
	 * @param[in] widget       卡片控件指针，**不可为 nullptr**；不存在时返回 false。
	 * @param[in] deleteWidget true 表示移除后立即 delete，false 表示仅摘除、由调用方处理生命周期。
	 * @return true 表示卡片确实在容器内且已被移除；false 表示未找到。
	 * @post 若被移除的是当前选中项，选中状态会被清空。
	 */
	bool removeDeviceTreeWidget(QWidget *widget, bool deleteWidget = true);

	/**
	 * @brief 清空容器内的全部设备卡片。
	 * @param[in] deleteWidgets true 表示逐个 delete，false 表示仅从布局摘除。
	 */
	void clearDeviceTreeWidgets(bool deleteWidgets = true);

	/** @brief 查询当前托管的卡片数量。@return 卡片个数，取值 [0, INT_MAX]。 */
	int deviceTreeWidgetCount() const;

	/**
	 * @brief 启动周期扫描。
	 * @param[in] intervalMs 扫描间隔，单位毫秒，取值 [1, INT_MAX]；传 0 会被 Qt 当作尽快触发。
	 * @post 重复调用会以新的间隔重新启动定时器。
	 */
	void startDeviceScan(int intervalMs = 1000);

	/**
	 * @brief 停止周期扫描。
	 * @post 定时器停止；已存在的卡片不受影响。
	 */
	void stopDeviceScan();

	/**
	 * @brief 应用容器默认视觉样式。
	 * @details 整体底色 #3b3b3b，顶部区域 #2e2e2e，与全局深色主题一致。
	 */
	void applyContainerStyle();

	/**
	 * @brief 按当前语言刷新容器内所有卡片的文案。
	 * @post 逐个对托管卡片调用其 retranslateUI()。
	 */
	void retranslateUI();

private slots:
	/** @brief 扫描定时器到期：触发一次设备清单刷新。 */
	void onScanTimerTimeout();
	/** @brief 队列槽：在 UI 线程真正完成卡片插入。@param[in] widget 待插入卡片，可为 nullptr（被忽略）。 */
	void slotAddDeviceTreeWidget(QWidget *widget);
	/** @brief 队列槽：在 UI 线程真正完成卡片移除。@param[in] widget 待移除卡片，可为 nullptr（被忽略）。 */
	void slotRemoveDeviceTreeWidget(QWidget *widget);
	/**
	 * @brief 处理子卡片的选中事件，做单选互斥并向外转发。
	 * @param[in] suitNum   被选中设备的序列号。
	 * @param[in] actorName 设备名（UTF-8）。
	 */
	void onDeviceWidgetSelected(const unsigned int& suitNum, const std::string& actorName);

private:
	/**
	 * @brief 把指定卡片设为唯一选中项。
	 * @param[in] widget 目标卡片，**可为 nullptr**；为空时仅清空当前选中。
	 * @post 其他卡片的选中态被清除。
	 */
	void selectDeviceTreeWidget(class h2GloveDeviceTreeWidget* widget);

	/** @brief uic 生成的界面对象，由析构函数负责 delete。 */
	Ui::h2GloveDeviceTreeContainerWidget* ui = nullptr;
	/** @brief 卡片竖向排列所用的布局。 */
	QVBoxLayout *m_layout = nullptr;
	/** @brief 周期扫描定时器。 */
	QTimer *m_scanTimer = nullptr;
	/** @brief 当前被选中的卡片，nullptr 表示无选中。 */
	class h2GloveDeviceTreeWidget* m_selectedWidget = nullptr;
	/** @brief 序列号到卡片的索引，用于按设备号快速定位。 */
	std::map<unsigned int, class h2GloveDeviceTreeWidget*> m_deviceMap;

signals:
	/** @brief 有卡片被加入容器时发出。@param[in] widget 新加入的卡片。 */
	void deviceWidgetAdded(class h2GloveDeviceTreeWidget* widget);
	/** @brief 有卡片从容器移除时发出。@param[in] widget 被移除的卡片。 */
	void deviceWidgetRemoved(class h2GloveDeviceTreeWidget* widget);
};

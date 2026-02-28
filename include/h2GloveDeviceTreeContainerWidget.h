#pragma once
#include "libh2ui_global.h"

#include <QtCore/QObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QTimer>

#include <map>

namespace Ui { class h2GloveDeviceTreeContainerWidget; }

class LIBH2UI_API h2GloveDeviceTreeContainerWidget : public QWidget
{
	Q_OBJECT

public:
	h2GloveDeviceTreeContainerWidget(QWidget *parent = Q_NULLPTR);
	~h2GloveDeviceTreeContainerWidget();

	// Add a device tree widget (caller owns the concrete type; container will reparent it)
	void addDeviceTreeWidget(QWidget *widget);
	// Remove a widget from the container; if deleteWidget is true, the widget will be deleted
	bool removeDeviceTreeWidget(QWidget *widget, bool deleteWidget = true);
	void clearDeviceTreeWidgets(bool deleteWidgets = true);

	int deviceTreeWidgetCount() const;

	// Start/stop automatic device scanning
	void startDeviceScan(int intervalMs = 1000);
	void stopDeviceScan();

	// Apply the default visual style for this container.
	// Sets the overall background color to #3b3b3b and the top area color to #2e2e2e.
	void applyContainerStyle();

private slots:
	void onScanTimerTimeout();
	// queued helpers to ensure UI modifications happen on the main thread
	void slotAddDeviceTreeWidget(QWidget *widget);
	void slotRemoveDeviceTreeWidget(QWidget *widget);

private:
	Ui::h2GloveDeviceTreeContainerWidget* ui = nullptr;
	QVBoxLayout *m_layout = nullptr;
	QTimer *m_scanTimer = nullptr;
	// map from serial number to widget pointer
	std::map<unsigned int, class h2GloveDeviceTreeWidget*> m_deviceMap;

signals:
	// Emitted when a device widget is added/removed so external code can connect to it
	void deviceWidgetAdded(class h2GloveDeviceTreeWidget* widget);
	void deviceWidgetRemoved(class h2GloveDeviceTreeWidget* widget);
};

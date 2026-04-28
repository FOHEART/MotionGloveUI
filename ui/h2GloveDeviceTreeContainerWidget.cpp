#include "../include/h2GloveDeviceTreeContainerWidget.h"
#include "ui_h2GloveDeviceTreeContainerWidget.h"
#include "../include/h2GloveDeviceTreeWidget.h"
#include <QtWidgets/QLayoutItem>
#include <QtCore/QTimer>
#include <QtCore/QMetaObject>

#include "h2sdk.h"

h2GloveDeviceTreeContainerWidget::h2GloveDeviceTreeContainerWidget(QWidget *parent)
	: QWidget(parent, Qt::Dialog)
{
	ui = new Ui::h2GloveDeviceTreeContainerWidget();
	ui->setupUi(this);

	m_layout = new QVBoxLayout();
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(4);
	setLayout(m_layout);

	m_scanTimer = new QTimer(this);
	connect(m_scanTimer, &QTimer::timeout, this, &h2GloveDeviceTreeContainerWidget::onScanTimerTimeout);

	m_layout->addStretch(1);

	// apply visual style
	applyContainerStyle();

	startDeviceScan(1000);
}

h2GloveDeviceTreeContainerWidget::~h2GloveDeviceTreeContainerWidget()
{
	stopDeviceScan();
	clearDeviceTreeWidgets(true);
	delete ui;
	ui = nullptr;
}

void h2GloveDeviceTreeContainerWidget::addDeviceTreeWidget(QWidget *widget)
{
	if (!widget)
		return;

	// Ensure the bottom spacer is removed so the new widget is placed before it,
	// then re-add the spacer so it remains at the bottom.
	if (m_layout->count() > 0) {
		QLayoutItem* last = m_layout->itemAt(m_layout->count() - 1);
		if (last && last->spacerItem()) {
			QLayoutItem* removed = m_layout->takeAt(m_layout->count() - 1);
			delete removed;
		}
	}

	widget->setParent(this);
	m_layout->addWidget(widget);
	// if the widget is a h2GloveDeviceTreeWidget, emit deviceWidgetAdded
	h2GloveDeviceTreeWidget* w = qobject_cast<h2GloveDeviceTreeWidget*>(widget);
	if (w) {
		connect(w, &h2GloveDeviceTreeWidget::suitSelectedLive,
			this, &h2GloveDeviceTreeContainerWidget::onDeviceWidgetSelected);
		emit deviceWidgetAdded(w);
	}

	// restore bottom spacer
	m_layout->addStretch(1);
}

bool h2GloveDeviceTreeContainerWidget::removeDeviceTreeWidget(QWidget *widget, bool deleteWidget)
{
	if (!widget)
		return false;
	if (!m_layout)
		return false;

	const int idx = m_layout->indexOf(widget);
	if (idx < 0)
		return false;

	QLayoutItem *item = m_layout->takeAt(idx);
	QWidget *w = item ? item->widget() : nullptr;
	delete item;

	if (w)
	{
		// emit removed
		h2GloveDeviceTreeWidget* gw = qobject_cast<h2GloveDeviceTreeWidget*>(w);
		if (gw) {
			if (m_selectedWidget == gw) {
				m_selectedWidget = nullptr;
			}
			disconnect(gw, &h2GloveDeviceTreeWidget::suitSelectedLive,
				this, &h2GloveDeviceTreeContainerWidget::onDeviceWidgetSelected);
			emit deviceWidgetRemoved(gw);
		}

		w->setParent(nullptr);
		if (deleteWidget)
			w->deleteLater();
	}

	// If the last item is not a spacer, ensure a bottom spacer exists
	if (m_layout->count() == 0) {
		m_layout->addStretch(1);
	} else {
		QLayoutItem* last = m_layout->itemAt(m_layout->count() - 1);
		if (!(last && last->spacerItem())) {
			m_layout->addStretch(1);
		}
	}

	return true;
}

void h2GloveDeviceTreeContainerWidget::clearDeviceTreeWidgets(bool deleteWidgets)
{
	if (!m_layout)
		return;

	while (m_layout->count() > 0)
	{
		QLayoutItem *item = m_layout->takeAt(0);
		QWidget *w = item ? item->widget() : nullptr;
		delete item;
		if (w)
		{
			// emit removed
			h2GloveDeviceTreeWidget* gw = qobject_cast<h2GloveDeviceTreeWidget*>(w);
			if (gw) {
				if (m_selectedWidget == gw) {
					m_selectedWidget = nullptr;
				}
				disconnect(gw, &h2GloveDeviceTreeWidget::suitSelectedLive,
					this, &h2GloveDeviceTreeContainerWidget::onDeviceWidgetSelected);
				emit deviceWidgetRemoved(gw);
			}

			w->setParent(nullptr);
			if (deleteWidgets)
				w->deleteLater();
		}
	}
	m_deviceMap.clear();

	// Ensure there's always a bottom spacer so the UI keeps space at the bottom
	m_layout->addStretch(1);
}

int h2GloveDeviceTreeContainerWidget::deviceTreeWidgetCount() const
{
	return m_layout ? m_layout->count() : 0;
}

void h2GloveDeviceTreeContainerWidget::startDeviceScan(int intervalMs)
{
	if (m_scanTimer->isActive())
		m_scanTimer->stop();
	m_scanTimer->start(intervalMs);
}

void h2GloveDeviceTreeContainerWidget::stopDeviceScan()
{
	if (m_scanTimer->isActive())
		m_scanTimer->stop();
}

void h2GloveDeviceTreeContainerWidget::onScanTimerTimeout()
{
	// Query SDK for connected glove serial numbers
	unsigned int snArray[64] = {0};
	int count = H2_SDK_getGlovesSerialNumber(snArray);

	// build a set of current serials
	std::map<unsigned int, bool> current;
	for (int i = 0; i < count; ++i) {
		current[snArray[i]] = true;
	}

	// Add new devices
	for (int i = 0; i < count; ++i) {
		unsigned int sn = snArray[i];
		if (m_deviceMap.find(sn) == m_deviceMap.end()) {
			// create widget for new device
			h2GloveDeviceTreeWidget *w = new h2GloveDeviceTreeWidget(this);
			w->setSerialNumber(sn);
			
			char nameBuf[32] = { 0 };
			int getNameRet = H2_SDK_getGloveName(sn, nameBuf);
			if (getNameRet == 0 && nameBuf[0] != '\0') {
				std::string gloveName(nameBuf);
				w->setDeviceName(QString::fromStdString(gloveName));
			}
			else
			{
			
				w->setDeviceName(QString("Glove") + QString(" ") + QString::number(sn));
			}
			// set widget stylesheet: black border and 10px rounded corners
			w->setStyleSheet("border: 1px solid black; border-radius: 10px;");
			// ensure UI changes happen on main thread by invoking slot
			QMetaObject::invokeMethod(this, "slotAddDeviceTreeWidget", Qt::QueuedConnection, Q_ARG(QWidget*, w));
			m_deviceMap[sn] = w;
		}
	}

	// Update status for existing devices
	for (auto &pair : m_deviceMap) {
		unsigned int sn = pair.first;
		h2GloveDeviceTreeWidget *w = pair.second;
		if (!w)
			continue;

		unsigned int isLeftOnline = 0, isRightOnline = 0, leftBattery = 0, rightBattery = 0;
		int statusRet = H2_SDK_getStatus(sn, &isLeftOnline, &isRightOnline, &leftBattery, &rightBattery, nullptr, nullptr);
		if (statusRet == 0) {
			w->setLeftOnline(isLeftOnline != 0);
			w->setRightOnline(isRightOnline != 0);
			w->setLeftBattery((int)leftBattery);
			w->setRightBattery((int)rightBattery);
		}
	}

	// Remove disconnected devices
	for (auto it = m_deviceMap.begin(); it != m_deviceMap.end();) {
		unsigned int sn = it->first;
		if (current.find(sn) == current.end()) {
			// remove widget using queued slot
			h2GloveDeviceTreeWidget *w = it->second;
			QMetaObject::invokeMethod(this, "slotRemoveDeviceTreeWidget", Qt::QueuedConnection, Q_ARG(QWidget*, w));
			it = m_deviceMap.erase(it);
		} else {
			++it;
		}
	}
}

void h2GloveDeviceTreeContainerWidget::slotAddDeviceTreeWidget(QWidget *widget)
{
	addDeviceTreeWidget(widget);
}

void h2GloveDeviceTreeContainerWidget::slotRemoveDeviceTreeWidget(QWidget *widget)
{
	removeDeviceTreeWidget(widget, true);
}

void h2GloveDeviceTreeContainerWidget::onDeviceWidgetSelected(
	const unsigned int& suitNum, const std::string& actorName)
{
	Q_UNUSED(suitNum);
	Q_UNUSED(actorName);

	h2GloveDeviceTreeWidget* widget = qobject_cast<h2GloveDeviceTreeWidget*>(sender());
	selectDeviceTreeWidget(widget);
}

void h2GloveDeviceTreeContainerWidget::selectDeviceTreeWidget(h2GloveDeviceTreeWidget* widget)
{
	if (!widget) {
		return;
	}

	for (auto& pair : m_deviceMap) {
		h2GloveDeviceTreeWidget* current = pair.second;
		if (!current) {
			continue;
		}

		current->setSelected(current == widget);
	}

	m_selectedWidget = widget;
}

void h2GloveDeviceTreeContainerWidget::applyContainerStyle()
{
    // Overall background
    //QString style = "QWidget#" + this->objectName() + " { background-color: #3b3b3b; }";
	QString style =  " { background-color: #3b3b3b; }";
    // Top area: assume the UI form has a top frame named 'topFrame' in the .ui
   // style += " QFrame#topFrame { background-color: #2e2e2e; }";
    this->setStyleSheet(style);
}

void h2GloveDeviceTreeContainerWidget::retranslateUI()
{
	// Retranslate all contained device tree widgets
	for (auto& pair : m_deviceMap) {
		h2GloveDeviceTreeWidget* widget = pair.second;
		if (widget) {
			widget->retranslateUI();
		}
	}
}

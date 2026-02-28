#pragma once
#include "libh2ui_global.h"
#include <QtWidgets/QWidget>

namespace Ui { class h2GloveDeviceTreeWidget; }

class LIBH2UI_API h2GloveDeviceTreeWidget : public QWidget
{
	Q_OBJECT

public:
	h2GloveDeviceTreeWidget(QWidget *parent = Q_NULLPTR);
	~h2GloveDeviceTreeWidget();

	// Set the device image from a file/resource path
	void setDeviceImage(const QString &path);
	// Set the device image from an existing pixmap
	void setDeviceImage(const QPixmap &pixmap);
	// Set the default device image (uses ":/images/Resources/img/usblink.png" then fallback)
	void setDefaultDeviceImage();

	// Set the device name text and make the font bold
	void setDeviceName(const QString &name);
	// Get the device name
	QString deviceName() const;

	// Control and query left-hand online state; updates LBL_leftImg accordingly
	void setLeftOnline(bool online);
	bool isLeftOnline() const;

	// Control and query right-hand online state; updates LBL_rightImg accordingly
	void setRightOnline(bool online);
	bool isRightOnline() const;

	// Set and get left battery level (0-100)
	void setLeftBattery(int level);
	int leftBattery() const;

	// Set and get right battery level (0-100)
	void setRightBattery(int level);
	int rightBattery() const;

	void setSerialNumber(unsigned int  sn) { m_sn = sn; }
	unsigned int serialNumber() const { return m_sn; }

	void setCreateIndexGlobal(int index);
	int createIndexGlobal() const { return m_createIndexGlobal; }

	// Global selection flag accessors (manipulate a module-level global selection state)
	void setSelected(bool selected);
	bool isSelected() const;

	// battery label size control
	void setBatteryLabelSize(const QSize &size);
	QSize batteryLabelSize() const { return m_batteryLabelSize; }

	// icon size control for dev name and side icons (dev image, left/right icons)
	void setIconSize(const QSize &size);
	QSize iconSize() const { return m_iconSize; }

protected:
	// event filter to handle mouse enter/leave/click on ui.widget
	bool eventFilter(QObject *watched, QEvent *event) override;

private:
	Ui::h2GloveDeviceTreeWidget* ui = nullptr;
	QString m_deviceName;
	bool m_isLeftOnline=false;
	bool m_isRightOnline=false;
	int m_leftBattery=0;
	int m_rightBattery=0;
	unsigned int  m_sn=0;
	int m_createIndexGlobal=-1;

	// battery label size (width, height)
	QSize m_batteryLabelSize = QSize(32, 16);
	// icon size for dev name and left/right icons
	QSize m_iconSize = QSize(32, 32);

	// update the left/right image widget according to m_isLeftOnline/m_isRightOnline
	void updateLeftImage();
	void updateRightImage();

	// update left/right battery image based on m_leftBattery/m_rightBattery
	void updateLeftBatteryImage();
	void updateRightBatteryImage();

signals:
	void suitSelectedLive(const unsigned int& suitNum,
		const std::string& actorName);
};

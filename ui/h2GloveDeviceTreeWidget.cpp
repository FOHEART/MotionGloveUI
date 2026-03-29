#include "../include/h2GloveDeviceTreeWidget.h"
#include "ui_h2GloveDeviceTreeWidget.h"

#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>

h2GloveDeviceTreeWidget::h2GloveDeviceTreeWidget(QWidget *parent)
	: QWidget(parent),
	m_deviceName(), 
	m_isLeftOnline(false),
	m_isRightOnline(false),
	m_leftBattery(100),
	m_rightBattery(100)
{
	ui = new Ui::h2GloveDeviceTreeWidget();
	ui->setupUi(this);

	// ensure resources from libh2UI/img.qrc are initialized
	//Q_INIT_RESOURCE(img);
	setIconSize(m_iconSize);
	// set a themed border with rounded corners on the container widget only (dark theme)
	if (ui->widget) {
		ui->widget->setObjectName("h2GloveFrame");
		// install event filter to catch hover and click
		ui->widget->installEventFilter(this);
	}

	// Configure the label size and alignment
	if (ui->LBL_devNameImg) {
		ui->LBL_devNameImg->setFixedSize(m_iconSize);
		ui->LBL_devNameImg->setAlignment(Qt::AlignCenter);
		// ensure child label has no border so parent frame remains sole border
		ui->LBL_devNameImg->setStyleSheet("border: none; background: transparent;");
	}

	// compute half icon size explicitly
	QSize halfIconSize(m_iconSize.width() / 2, m_iconSize.height() / 2);

	// configure left image label
	if (ui->LBL_leftImg) {
		ui->LBL_leftImg->setFixedSize(halfIconSize);
		ui->LBL_leftImg->setAlignment(Qt::AlignCenter);
		ui->LBL_leftImg->setStyleSheet("border: none; background: transparent;");
	}

	// configure right image label
	if (ui->LBL_rightImg) {
		ui->LBL_rightImg->setFixedSize(halfIconSize);
		ui->LBL_rightImg->setAlignment(Qt::AlignCenter);
		ui->LBL_rightImg->setStyleSheet("border: none; background: transparent;");
	}

	// configure left battery label using m_batteryLabelSize
	if (ui->LBL_leftBattery) {
		ui->LBL_leftBattery->setFixedSize(m_batteryLabelSize);
		ui->LBL_leftBattery->setAlignment(Qt::AlignCenter);
		ui->LBL_leftBattery->setStyleSheet("border: none; background: transparent;");
	}

	// configure right battery label using m_batteryLabelSize
	if (ui->LBL_rightBattery) {
		ui->LBL_rightBattery->setFixedSize(m_batteryLabelSize);
		ui->LBL_rightBattery->setAlignment(Qt::AlignCenter);
		ui->LBL_rightBattery->setStyleSheet("border: none; background: transparent;");
	}

	if (ui->LBL_devName) {
		ui->LBL_devName->setStyleSheet(QString("border: none; background: transparent; color: #DCDCDC; font: bold %1px;").arg(m_iconSize.height()));
		ui->LBL_devName->setFixedHeight(m_iconSize.height());
	}
	if (ui->LBL_left) {
		ui->LBL_left->setStyleSheet("border: none; background: transparent; color: #DCDCDC;");
	}
	if (ui->LBL_right) {
		ui->LBL_right->setStyleSheet("border: none; background: transparent; color: #DCDCDC;");
	}

	
	// Load default image
	setDefaultDeviceImage();
	setCreateIndexGlobal(2);

	// initialize left/right images and battery
	updateLeftImage();
	updateRightImage();
	updateLeftBatteryImage();
	updateRightBatteryImage();
	updateToolTip();
	applyFrameStyle();
}

h2GloveDeviceTreeWidget::~h2GloveDeviceTreeWidget()
{
	delete ui;
	ui = nullptr;
}

void h2GloveDeviceTreeWidget::setSerialNumber(unsigned int sn)
{
	m_sn = sn;
	updateToolTip();
}

void h2GloveDeviceTreeWidget::setIconSize(const QSize &size)
{
	//if (m_iconSize == size)
	//	return;
	m_iconSize = size;
	if (ui->LBL_devNameImg) ui->LBL_devNameImg->setFixedSize(m_iconSize);
	QSize halfIconSize(m_iconSize.width() / 2, m_iconSize.height() / 2);
	if (ui->LBL_leftImg) ui->LBL_leftImg->setFixedSize(halfIconSize);
	if (ui->LBL_rightImg) ui->LBL_rightImg->setFixedSize(halfIconSize);

	

	updateLeftImage();
	updateRightImage();
	setDefaultDeviceImage();
}

bool h2GloveDeviceTreeWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == ui->widget) {
		switch (event->type()) {
		case QEvent::Enter:
			m_isHovered = true;
			applyFrameStyle();
			return true;
		case QEvent::Leave:
			m_isHovered = false;
			applyFrameStyle();
			return true;
		case QEvent::MouseButtonPress:
		{
			QMouseEvent *me = static_cast<QMouseEvent*>(event);
			if (me->button() == Qt::LeftButton) {
				// emit selection signal with relevant info
				unsigned int suitNum = m_sn;
				QString actorName = m_deviceName;
				emit suitSelectedLive(suitNum, actorName.toStdString());
			}
			return true;
		}
		default:
			break;
		}
	}
	return QWidget::eventFilter(watched, event);
}

void h2GloveDeviceTreeWidget::setDeviceImage(const QString &path)
{
	if (!ui->LBL_devNameImg)
		return;

	QPixmap pix;
	pix.load(path);
	if (!pix.isNull()) {
		ui->LBL_devNameImg->setPixmap(pix.scaled(m_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		ui->LBL_devNameImg->setScaledContents(false);
	} else {
		ui->LBL_devNameImg->clear();
	}
}

void h2GloveDeviceTreeWidget::applyFrameStyle()
{
	if (!ui || !ui->widget)
	{
		return;
	}

	QString background = "rgb(43,46,61)";
	if (m_isSelected)
	{
		background = "rgba(0,158,232,0.6)";
	}
	else if (m_isHovered)
	{
		background = "#58616B";
	}

	ui->widget->setStyleSheet(
		QString("#h2GloveFrame { border: 1px solid #818996; border-radius: 10px; background-color: %1; }")
			.arg(background));
}

void h2GloveDeviceTreeWidget::setDeviceImage(const QPixmap &pixmap)
{
	if (!ui->LBL_devNameImg)
		return;

	if (!pixmap.isNull()) {
		ui->LBL_devNameImg->setPixmap(pixmap.scaled(m_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		ui->LBL_devNameImg->setScaledContents(false);
	} else {
		ui->LBL_devNameImg->clear();
	}
}

void h2GloveDeviceTreeWidget::updateToolTip()
{
	const QString tooltip = QString::fromUtf8("序列号：%1")
		.arg(QString::number(m_sn, 16).toUpper());

	setToolTip(tooltip);
	if (ui && ui->widget)
	{
		ui->widget->setToolTip(tooltip);
	
	}
}

void h2GloveDeviceTreeWidget::setDefaultDeviceImage()
{
	// Try resource path first, then fallback to the provided path
	QPixmap pix;
	pix.load(":/images/Resources/img/usblink.png");
	
	setDeviceImage(pix);
}

void h2GloveDeviceTreeWidget::setDeviceName(const QString &name)
{
	if (m_deviceName == name)
		return;
	m_deviceName = name;

	if (!ui->LBL_devName)
		return;

	ui->LBL_devName->setText(m_deviceName);

	// update dev name label height and font to match new icon size
	if (ui->LBL_devName)
	{
		ui->LBL_devName->setFixedHeight(m_iconSize.height() / 2);
		QFont f = ui->LBL_devName->font();
		f.setBold(true);
		f.setPixelSize(m_iconSize.height() / 2);
		ui->LBL_devName->setFont(f);
		// also set stylesheet font to ensure application-level qss doesn't override
		ui->LBL_devName->setStyleSheet(QString("border: none; background: transparent; color: #DCDCDC; font: bold %1px;").arg(m_iconSize.height() / 2));
	}
}

QString h2GloveDeviceTreeWidget::deviceName() const
{
	return m_deviceName;
}

void h2GloveDeviceTreeWidget::setLeftOnline(bool online)
{
	if (m_isLeftOnline == online)
		return;
	m_isLeftOnline = online;
	updateLeftImage();
}

bool h2GloveDeviceTreeWidget::isLeftOnline() const
{
	return m_isLeftOnline;
}

void h2GloveDeviceTreeWidget::updateLeftImage()
{
	if (!ui->LBL_leftImg)
		return;

	const char *onlinePath = ":/images/Resources/img/lefthand_online.png";
	const char *offlinePath = ":/images/Resources/img/lefthand_offline.png";

	QPixmap pix;
	if (m_isLeftOnline) {
		pix.load(onlinePath);
	} else {
		pix.load(offlinePath);
	}

	if (!pix.isNull()) {
		QSize halfIconSize(m_iconSize.width() / 2, m_iconSize.height() / 2);
		ui->LBL_leftImg->setPixmap(pix.scaled(halfIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		ui->LBL_leftImg->setScaledContents(false);
	} else {
		ui->LBL_leftImg->clear();
	}
}

void h2GloveDeviceTreeWidget::setRightOnline(bool online)
{
	if (m_isRightOnline == online)
		return;
	m_isRightOnline = online;
	updateRightImage();
}

bool h2GloveDeviceTreeWidget::isRightOnline() const
{
	return m_isRightOnline;
}

void h2GloveDeviceTreeWidget::updateRightImage()
{
	if (!ui->LBL_rightImg)
		return;

	const char *onlinePath = ":/images/Resources/img/righthand_online.png";
	const char *offlinePath = ":/images/Resources/img/righthand_offline.png";

	QPixmap pix;
	if (m_isRightOnline) {
		pix.load(onlinePath);
	} else {
		pix.load(offlinePath);
	}

	if (!pix.isNull()) {
		QSize halfIconSize(m_iconSize.width() / 2, m_iconSize.height() / 2);
		ui->LBL_rightImg->setPixmap(pix.scaled(halfIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		ui->LBL_rightImg->setScaledContents(false);
	} else {
		ui->LBL_rightImg->clear();
	}
}

void h2GloveDeviceTreeWidget::setLeftBattery(int level)
{
	if (level < 0)
		level = 0;
	if (level > 100)
		level = 100;
// 	if(m_leftBattery == level)
// 		return;

	m_leftBattery = level;
	updateLeftBatteryImage();
}

int h2GloveDeviceTreeWidget::leftBattery() const
{
	return m_leftBattery;
}

void h2GloveDeviceTreeWidget::updateLeftBatteryImage()
{
	if (!ui->LBL_leftBattery)
		return;

	const char* imgOffline = ":/images/Resources/img/battery_unconnect.png";
	const char *img1 = ":/images/Resources/img/battery1.png"; // 0-25
	const char *img2 = ":/images/Resources/img/battery2.png"; // 25-50
	const char *img3 = ":/images/Resources/img/battery3.png"; // 50-75
	const char *img4 = ":/images/Resources/img/battery4.png"; // 75-100

	const char *path = img1;
	if(m_isLeftOnline)
	{
		if (m_leftBattery <= 25)
		{
			path = img1;
		}
		else if (m_leftBattery <= 50)
		{
			path = img2;
		}
		else if (m_leftBattery <= 75)
		{
			path = img3;
		}
		else
		{
			path = img4;
		}
	}else
		{
		path = imgOffline;
	}


	QPixmap pix;
	pix.load(path);
	if (!pix.isNull()) {
		ui->LBL_leftBattery->setPixmap(pix.scaled(m_batteryLabelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		ui->LBL_leftBattery->setScaledContents(false);
	} else {
		ui->LBL_leftBattery->clear();
	}
}

void h2GloveDeviceTreeWidget::setRightBattery(int level)
{
	if (level < 0)
		level = 0;
	if (level > 100)
		level = 100;

// 	if (m_rightBattery == level)
// 		return;

	m_rightBattery = level;
	updateRightBatteryImage();
}

int h2GloveDeviceTreeWidget::rightBattery() const
{
	return m_rightBattery;
}

void h2GloveDeviceTreeWidget::setCreateIndexGlobal(int index)
{
	m_createIndexGlobal = index;
	setDeviceName(QString("Glove%1").arg(m_createIndexGlobal + 1));
}

void h2GloveDeviceTreeWidget::updateRightBatteryImage()
{
	if (!ui->LBL_rightBattery)
		return;

	const char* imgOffline = ":/images/Resources/img/battery_unconnect.png";
	const char *img1 = ":/images/Resources/img/battery1.png"; // 0-25
	const char *img2 = ":/images/Resources/img/battery2.png"; // 25-50
	const char* img3 = ":/images/Resources/img/battery3.png"; // 50-75
	const char* img4 = ":/images/Resources/img/battery4.png"; // 75-100

	const char* path = img1;
	if (m_isRightOnline)
	{
		if (m_rightBattery <= 25)
		{
			path = img1;
		}
		else if (m_rightBattery <= 50)
		{
			path = img2;
		}
		else if (m_rightBattery <= 75)
		{
			path = img3;
		}
		else
		{
			path = img4;
		}
	}
	else
	{
		path = imgOffline;
	}
	

	QPixmap pix;
	pix.load(path);
	if (!pix.isNull()) {
		ui->LBL_rightBattery->setPixmap(pix.scaled(m_batteryLabelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		ui->LBL_rightBattery->setScaledContents(false);
	} else {
		ui->LBL_rightBattery->clear();
	}
}

// implement selection accessors
void h2GloveDeviceTreeWidget::setSelected(bool selected)
{
	if (m_isSelected == selected)
		return;

	m_isSelected = selected;
	applyFrameStyle();
}

bool h2GloveDeviceTreeWidget::isSelected() const
{
	return m_isSelected;
}

void h2GloveDeviceTreeWidget::setBatteryLabelSize(const QSize& size)
{
	if (m_batteryLabelSize == size)
		return;

	m_batteryLabelSize = size;

	if (ui->LBL_leftBattery) {
		ui->LBL_leftBattery->setFixedSize(m_batteryLabelSize);
	}

	if (ui->LBL_rightBattery) {
		ui->LBL_rightBattery->setFixedSize(m_batteryLabelSize);
	}

	// Update the battery images to match the new size
	updateLeftBatteryImage();
	updateRightBatteryImage();
}

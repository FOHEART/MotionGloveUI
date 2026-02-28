#include "h2NetBroadcastWidget.h"
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHostInfo>

#include <QtCore/QStringList>
#include <QtWidgets/QMenu>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#include "CarbonUIClass.h"
#include "groupboxcollapser.h"

#include "h2sdk.h"


h2NetBroadcastWidget::h2NetBroadcastWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// set all labels to white
	CarbonUIClass::setWidgetLabelsStyleRecursively(this);

	CarbonUIClass::applyButtonStyle(ui.addAddrBtn, CarbonUIClass::Shape_RoundAll);
	CarbonUIClass::applyLineEditStyle_numberInput(ui.portLineEdit, 100);
	CarbonUIClass::setupComboBoxStyle(ui.CBX_IPAddress);
	CarbonUIClass::setupComboBoxStyle(ui.CBX_txFramerate);
	CarbonUIClass::applyTreeWidgetStyle(ui.IPPortListWidget);

	

	QRegExp regIp("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
	QRegExp regPort("((6553[0-5])|[655[0-2][0-9]|65[0-4][0-9]{2}|6[0-4][0-9]{3}|[1-5][0-9]{4}|[1-9][0-9]{3}|[1-9][0-9]{2}|[1-9][0-9]|[0-9])");
	QRegExpValidator *pRegIP = new QRegExpValidator(regIp, this);
	QRegExpValidator *pRegPort = new QRegExpValidator(regPort, this);
	ui.portLineEdit->setValidator(pRegPort);
	ui.CBX_IPAddress->setValidator(pRegIP);


	fillLocalIPAddress();
	fillUDPTxFramerate();

	// Ensure 3 columns: IP / Port / Action
	ui.IPPortListWidget->setColumnCount(3);
	ui.IPPortListWidget->setHeaderLabels(QStringList() << "IP" << "Port" << "");
	ui.IPPortListWidget->setColumnWidth(0, 170);
	ui.IPPortListWidget->setColumnWidth(1, 170);
	//ui.IPPortListWidget->setColumnWidth(2, 100);

	

	loadLocalIPAddressFromConfigFile();
}

h2NetBroadcastWidget::~h2NetBroadcastWidget()
{
}

void h2NetBroadcastWidget::writeH2StreamConfig()
{
	QStringList addrList;
	for (int i = 0; i < ui.IPPortListWidget->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = ui.IPPortListWidget->topLevelItem(i);
		if (!item)
			continue;

		const QString ip = item->text(0).trimmed();
		const QString port = item->text(1).trimmed();
		if (ip.isEmpty() || port.isEmpty())
			continue;

		addrList << (ip + ":" + port);
	}

	const QString broadcastAddr = addrList.join(",");

	h2GloveStreamConfig cfg;
	H2_SDK_getGloveStreamConfig(&cfg);

	memset(cfg.BroadcastAddr, 0, sizeof(cfg.BroadcastAddr));
	const QByteArray addrBytes = broadcastAddr.toUtf8();
	const int copyLen = qMin<int>((int)sizeof(cfg.BroadcastAddr) - 1, addrBytes.size());
	if (copyLen > 0)
	{
		memcpy(cfg.BroadcastAddr, addrBytes.constData(), (size_t)copyLen);
	}

	H2_SDK_setGloveStreamConfig(cfg);
}

/*
获取本机当前所有的ip地址
*/
void h2NetBroadcastWidget::fillLocalIPAddress()
{
    ui.CBX_IPAddress->clear();

    // Always include loopback
    ui.CBX_IPAddress->addItem(QString("127.0.0.1"));

    // Use QNetworkInterface::allAddresses which reliably returns all addresses
    // on Linux (and other platforms). Filter for IPv4 and non-loopback.
    QSet<QString> seen;
    seen.insert(QString("127.0.0.1"));

    const QList<QHostAddress> allAddrs = QNetworkInterface::allAddresses();
    qDebug() << "all network addresses:" << allAddrs;

    for (const QHostAddress &address : allAddrs)
    {
        if (address.protocol() != QAbstractSocket::IPv4Protocol)
            continue;

        const QString s = address.toString();
        // skip duplicates and localhost (already added)
        if (seen.contains(s))
            continue;

        // skip unspecified or broadcast addresses
        if (s.isEmpty() || s == QLatin1String("0.0.0.0") || s == QLatin1String("255.255.255.255"))
            continue;

        // Optionally ignore loopback again
        if (address.isLoopback())
            continue;

        ui.CBX_IPAddress->addItem(s);
        seen.insert(s);
    }

    // Fallback: if only loopback present and we can resolve hostname to other addrs,
    // attempt name resolution (keeps previous behavior as fallback)
    if (ui.CBX_IPAddress->count() <= 1)
    {
        QString localHostName = QHostInfo::localHostName();
        qDebug() << "fallback localHostName:" << localHostName;
        QHostInfo info = QHostInfo::fromName(localHostName);
        qDebug() << "resolved addresses:" << info.addresses();
        for (const QHostAddress &address : info.addresses())
        {
            if (address.protocol() != QAbstractSocket::IPv4Protocol)
                continue;
            const QString s = address.toString();
            if (seen.contains(s))
                continue;
            ui.CBX_IPAddress->addItem(s);
            seen.insert(s);
        }
    }
}
#define H2_NETUDP_TXFRAMERATE_KEY "NetUDP/txFrameRate"
#define H2_NETUDP_TXFRAMERATE_COUNT (3)
const int h2_netudp_txFrameRate[H2_NETUDP_TXFRAMERATE_COUNT] = {
	60,
	60 / 2,
	60 / 4
};
///*发送帧率*/
void h2NetBroadcastWidget::fillUDPTxFramerate()
{
	for (int i = 0; i < H2_NETUDP_TXFRAMERATE_COUNT; i++)
	{
		QString it = QString::number(h2_netudp_txFrameRate[i]);
		it += "Hz";
		ui.CBX_txFramerate->addItem(it);
	}
}

void h2NetBroadcastWidget::addIPPortRow(const QString &ip, const QString &port)
{
	QStringList itemList;
	itemList << ip << port << "";

	QTreeWidgetItem *rowItem = new QTreeWidgetItem(itemList);
	ui.IPPortListWidget->addTopLevelItem(rowItem);

	QPushButton *btnDelete = new QPushButton(QString::fromLocal8Bit("删除"), ui.IPPortListWidget);
	CarbonUIClass::applyButtonStyle(btnDelete, CarbonUIClass::Shape_RoundAll);

	// set button height to be 2 pixels smaller than the row/container height
	int idx = ui.IPPortListWidget->indexOfTopLevelItem(rowItem);
	int rowH = -1;
	if (idx >= 0) {
		rowH = ui.IPPortListWidget->sizeHintForRow(idx);
	}
	if (rowH <= 0) {
		// fallback: try default item height using font metrics or a reasonable default
		rowH = ui.IPPortListWidget->fontMetrics().height() + 8; // padding estimate
	}
	btnDelete->setFixedHeight(qMax(1, rowH - 10));

	btnDelete->setProperty("_h2_rowItem", QVariant::fromValue<quintptr>((quintptr)rowItem));
	connect(btnDelete, &QPushButton::clicked, this, &h2NetBroadcastWidget::on_deleteRowButton_clicked);
	ui.IPPortListWidget->setItemWidget(rowItem, 2, btnDelete);
}

void h2NetBroadcastWidget::loadLocalIPAddressFromConfigFile()
{
	h2GloveStreamConfig cfg;
	H2_SDK_getGloveStreamConfig(&cfg);

	//print cfg for debug
	printf("[DEBUG]Current BroadcastAddr in config: %s\r\n",
		QString::fromUtf8(reinterpret_cast<const char*>(cfg.BroadcastAddr)).toUtf8().constData());

	const QString broadcastAddr = QString::fromUtf8(reinterpret_cast<const char*>(cfg.BroadcastAddr)).trimmed();
	if (broadcastAddr.isEmpty())
		return;

	const QStringList pairs = broadcastAddr.split(",", Qt::SkipEmptyParts);
	for (const QString &pairRaw : pairs)
	{
		const QString pair = pairRaw.trimmed();
		if (pair.isEmpty())
			continue;

		const int sepIdx = pair.lastIndexOf(":");
		if (sepIdx <= 0 || sepIdx >= pair.length() - 1)
			continue;

		const QString ip = pair.left(sepIdx).trimmed();
		const QString port = pair.mid(sepIdx + 1).trimmed();
		if (ip.isEmpty() || port.isEmpty())
			continue;

		bool duplicate = false;
		for (int i = 0; i < ui.IPPortListWidget->topLevelItemCount(); i++)
		{
			QTreeWidgetItem *item = ui.IPPortListWidget->topLevelItem(i);
			if (!item)
				continue;
			if (item->text(0).trimmed() == ip && item->text(1).trimmed() == port)
			{
				duplicate = true;
				break;
			}
		}
		if (duplicate)
		{
			printf("[WARNING]Duplicate IP:Port found in config file, skipping: %s:%s\r\n", ip.toUtf8().constData(), port.toUtf8().constData());
			continue;
		}else
		{
			printf("[INFO]Loaded IP:Port from config file: %s:%s\r\n", ip.toUtf8().constData(), port.toUtf8().constData());
		}

		addIPPortRow(ip, port);
	}
}

void h2NetBroadcastWidget::on_addAddrBtn_clicked()
{
	/*最多允许添加10条*/
	if (ui.IPPortListWidget->topLevelItemCount() >= 10)
	{
		printf("[ERROR]IP address list is full (max 10).\r\n");
		return;
	}

	/*如果有一项为空，则直接返回*/
	if (ui.CBX_IPAddress->currentText().isEmpty() || ui.portLineEdit->text().isEmpty())
	{
		printf("[ERROR]IP address or port number is empty!\r\n");
		return;
	}

	QString strIP = ui.CBX_IPAddress->currentText();
	QString strPort = ui.portLineEdit->text();

	/*如果不为空，还需要检验IP地址合法性，不合法还是直接返回*/
	QHostAddress addressValidator;
	if (!addressValidator.setAddress(strIP))
	{
		printf("[ERROR]IP Address is not correct!\r\n");
		return;
	}

	/*如果合法，还需要查找是否重复添加*/
	bool match = false;
	for (int i = 0; i < ui.IPPortListWidget->topLevelItemCount(); i++)
	{
		QTreeWidgetItem *item = ui.IPPortListWidget->topLevelItem(i);
		QString strIPFind = item->text(0);
		QString strPortFind = item->text(1);
		if (!strIP.compare(strIPFind) && !strPort.compare(strPortFind))
		{
			match = true;
		}
	}
	if (match)
	{
		printf("[ERROR]Already exist ip address!\r\n");
		return;
	}

	addIPPortRow(strIP, strPort);
}

void h2NetBroadcastWidget::on_deleteRowButton_clicked()
{
	QPushButton *btn = qobject_cast<QPushButton*>(sender());
	if (!btn)
		return;

	const quintptr ptr = btn->property("_h2_rowItem").value<quintptr>();
	QTreeWidgetItem *item = reinterpret_cast<QTreeWidgetItem*>(ptr);
	if (!item)
		return;

	const int idx = ui.IPPortListWidget->indexOfTopLevelItem(item);
	if (idx < 0)
		return;

	delete ui.IPPortListWidget->takeTopLevelItem(idx);
}

// void h2NetBroadcastWidget::on_IPPortListWidget_customContextMenuRequested(const QPoint &pos)
// {
// 	QTreeWidgetItem *item = ui.IPPortListWidget->itemAt(pos);
// 	if (item)
// 		ui.IPPortListWidget->setCurrentItem(item);
// 
// 	QTreeWidgetItem *current = ui.IPPortListWidget->currentItem();
// 	if (!current)
// 		return;
// 
// 	QMenu *menu = new QMenu(ui.IPPortListWidget);
// 	menu->setAttribute(Qt::WA_DeleteOnClose, true);
// 
// 	QAction *deleteAction = menu->addAction("Delete");
// 	QObject::connect(deleteAction, &QAction::triggered, this, [this]() {
// 		QTreeWidgetItem *cur = ui.IPPortListWidget->currentItem();
// 		if (!cur)
// 			return;
// 		const int idx = ui.IPPortListWidget->indexOfTopLevelItem(cur);
// 		if (idx >= 0)
// 			delete ui.IPPortListWidget->takeTopLevelItem(idx);
// 	});
// 
// 	menu->popup(ui.IPPortListWidget->viewport()->mapToGlobal(pos));
// }

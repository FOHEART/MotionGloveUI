#include "h2UnityWindowConfig.h"
//#include <QtNetwork/QUdpSocket>
#include <QtGui/QRegExpValidator>
#include "h2sdk.h"

h2UnityWindowConfig::h2UnityWindowConfig(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	/*
	在软件中预览unity窗口 默认使用本机的ip和5009端口号
	*/
	{
		// 正则表达式：表示一个0到65535之间的数字
		QRegExp portRegex("^(6553[0-5]|655[0-2]\\d|65[0-4]\\d\\d|6[0-4]\\d{3}|[1-5]\\d{4}|[1-9]\\d{0,3}|0)$");
		QRegExpValidator* validator = new QRegExpValidator(portRegex, this);
		// 设置验证器给QLineEdit
		ui.LE_port->setValidator(validator);
	}

	{
		QRegExp ipRegex("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
		QRegExpValidator* validator = new QRegExpValidator(ipRegex, ui.LE_addr);
		// 设置验证器
		ui.LE_addr->setValidator(validator);
	}


	setWindowFlags(windowFlags() & ~Qt::WindowTitleHint & ~Qt::WindowSystemMenuHint);

	//自动打开unity窗口
	QObject::connect(ui.CBX_autoOpenUnityWindow, SIGNAL(toggled(bool)), this, SLOT(onCBX_autoOpenUnityWindow(bool)));

	//弹出unity窗口
	QObject::connect(ui.CBX_popoutUnityWindow, SIGNAL(toggled(bool)), this, SLOT(onCBX_popoutUnityWindow(bool)));

	QObject::connect(ui.RB_lowQuality, SIGNAL(toggled(bool)), this, SLOT(onRB_unityQualityToggled(bool)));
	QObject::connect(ui.RB_highQuality, SIGNAL(toggled(bool)), this, SLOT(onRB_unityQualityToggled(bool)));
}

h2UnityWindowConfig::~h2UnityWindowConfig()
{

}

void h2UnityWindowConfig::setGloveSN(unsigned int val)
{
	m_gloveSN = val;
	int m_sn = m_gloveSN;
	/*已经有了信息 更新当前套装的配置信息*/
	h2GloveMgr newX2Suit;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(m_sn, newX2Suit);
	if (isFind)
	{
		onCBX_autoOpenUnityWindow(newX2Suit.AutoOpenUnityWindow);
		onCBX_popoutUnityWindow(newX2Suit.PopoutUnityWindow);

		ui.LE_addr->setText(QString::fromStdString(newX2Suit.PopoutUnityWindowIP));
		ui.LE_port->setText(QString::number(newX2Suit.PopoutUnityWindowPort));

		if(newX2Suit.UnityDisplayQuality==0)
		{
			ui.RB_lowQuality->setChecked(true);

		}
		else if (newX2Suit.UnityDisplayQuality == 1)
		{
			ui.RB_highQuality->setChecked(true);
		}
	}
	else
	{
		//没找到 不可能出现这种情况
		printf("");
	}
}

void h2UnityWindowConfig::on_PB_OK_clicked()
{
	int m_sn = m_gloveSN;

	/*保存套装的配置文件*/
	/*已经有了信息 更新当前套装的配置信息*/
	h2GloveMgr newX2Suit;
	bool isFind = H2_SDK_getCurrentX2SuitBySn(m_sn, newX2Suit);
	if (isFind)
	{
		bool changed = false;

		if (m_autoOpenUnityPort != newX2Suit.AutoOpenUnityWindow)
		{
			newX2Suit.AutoOpenUnityWindow = m_autoOpenUnityPort;
			changed = true;
		}

		if (m_popoutUnityWindow != newX2Suit.PopoutUnityWindow)
		{
			newX2Suit.PopoutUnityWindow = m_popoutUnityWindow;
			changed = true;
		}

		if(m_unityDisplayQuality!=newX2Suit.UnityDisplayQuality)
		{
			newX2Suit.UnityDisplayQuality = m_unityDisplayQuality;
			changed = true;
		}

		{
			std::string ip = ui.LE_addr->text().toStdString();
			unsigned int port = ui.LE_port->text().toUInt();

			if (ip != newX2Suit.PopoutUnityWindowIP)
			{
				newX2Suit.PopoutUnityWindowIP = ip;
				changed = true;
			}
			if (port != newX2Suit.PopoutUnityWindowPort)
			{
				newX2Suit.PopoutUnityWindowPort = port;
				changed = true;
			}
			
			
		}

		if (changed)
		{
			H2_SDK_updateX2SuitMgrVec(m_sn, newX2Suit);
		}
	}
	else
	{
		//没找到 不可能出现这种情况
		printf("");
	}

	close();
	//hide();
}

void h2UnityWindowConfig::on_PB_Cancel_clicked()
{
	close();
	//hide();
}

void h2UnityWindowConfig::onCBX_autoOpenUnityWindow(bool bCheck)
{
	m_autoOpenUnityPort = bCheck;
	ui.CBX_autoOpenUnityWindow->setChecked(bCheck);
}

void h2UnityWindowConfig::onCBX_popoutUnityWindow(bool bCheck)
{
	m_popoutUnityWindow = bCheck;
	ui.CBX_popoutUnityWindow->setChecked(bCheck);
}

void h2UnityWindowConfig::onRB_unityQualityToggled(bool bCheck)
{
	QObject* sender = this->sender();
	if (ui.RB_lowQuality == qobject_cast<QRadioButton*>(sender))
	{
		// 这里可以处理特定按钮的逻辑
		if(ui.RB_lowQuality->isChecked())
		{
			m_unityDisplayQuality = 0;
		}
	}
	else if (ui.RB_highQuality == qobject_cast<QRadioButton*>(sender))
	{
		// 这里可以处理特定按钮的逻辑
		if (ui.RB_highQuality->isChecked())
		{
			m_unityDisplayQuality = 1;
		}
	}
}

//bool h2UnityWindowConfig::isUdpPortAvailable(quint16 port)
//{
	//QUdpSocket udpSocket;

	//// 尝试绑定到所有网络接口上的指定端口
	//if (!udpSocket.bind(port, QUdpSocket::ShareAddress))
	//{
	//	// 如果绑定失败，打印错误信息并返回false
	//	qDebug() << "无法绑定到端口" << port << ":" << udpSocket.errorString();

	//	// 检查具体的错误类型是否为地址已被使用
	//	if (udpSocket.error() == QAbstractSocket::AddressInUseError)
	//	{
	//		qDebug() << "端口" << port << "已被占用";
	//		return false;
	//	}

	//	return false;
	//}
	//else
	//{
	//	// 成功绑定，端口未被占用
	//	qDebug() << "端口" << port << "可用";
	//	return true;
	//}
//}

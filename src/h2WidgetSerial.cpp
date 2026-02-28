//#include "stdafx.h"
#include "../include/h2Widget.h"


void h2Widget::updateUartPorts(void)
{
#if 0
	ui.CBX_uart->clear();
	//查找可用的串口
	foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		QSerialPort serial;
		serial.setPort(info);
		if (serial.open(QIODevice::ReadWrite))
		{
			ui.CBX_uart->addItem(serial.portName());
			serial.close();
		}
	}
	//设置波特率下拉菜单默认显示第0项
	ui.CBX_uartBaudrate->setCurrentIndex(0);
#endif
#if 0
	ui.CBX_uart->clear();

	char s[256][8] = { 0 };
	unsigned int sLen = 0;
	H2_SDK_getSystemComPorts(&sLen, &s[0][0]);
	printf("");
	for (int i = 0; i < sLen; i++)
	{
		std::string uartName(&s[i][0]);
		ui.CBX_uart->addItem(QString::fromStdString(uartName));
	}
	ui.CBX_uartBaudrate->setCurrentIndex(0);
#endif
}

void h2Widget::on_PB_uartOpen_clicked()
{
#if 0
	if (uartOpenStatus)
	{
		//已经打开了
		//serial->close();
		//ui.PB_uartOpen->setText(tr("打开串口"));
		std::string comPortName = ui.CBX_uart->currentText().toStdString();
		H2_SDK_closeComPort(comPortName.c_str());

		ui.PB_uartOpen->setText("打开");
		uartOpenStatus = 0;

		ui.CBX_uart->setEnabled(true);
		ui.CBX_uartBaudrate->setEnabled(true);
		ui.PB_uartDetect->setEnabled(true);
	}
	else
	{
		//没打开
#if 0
		if (serial == 0)
		{
			serial = new QSerialPort;
		}
		//设置串口名
		serial->setPortName(ui.CBX_uart->currentText());
		//打开串口
		serial->open(QIODevice::ReadWrite);
		//设置波特率
		serial->setBaudRate(QSerialPort::Baud115200);//设置波特率为115200

		unsigned int br = std::stoi(ui.CBX_uartBaudrate->currentText().toStdString());
		//设置波特率
		serial->setBaudRate(br);//设置波特率为115200
		serial->setDataBits(QSerialPort::Data8);//设置数据位8
		serial->setParity(QSerialPort::NoParity);
		serial->setStopBits(QSerialPort::OneStop);//停止位设置为1
		serial->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制
		ui.PB_uartOpen->setText(tr("关闭串口"));
#endif
		unsigned int br = std::stoi(ui.CBX_uartBaudrate->currentText().toStdString());
		std::string comPortName = ui.CBX_uart->currentText().toStdString();
		if (0 == H2_SDK_openComPort(comPortName.c_str(), br))
		{
			H2_SDK_setBindedRawComPort(m_sn, comPortName);
			uartOpenStatus = 1;
			ui.PB_uartOpen->setText("关闭");
			ui.CBX_uart->setEnabled(false);
			ui.CBX_uartBaudrate->setEnabled(false);
			ui.PB_uartDetect->setEnabled(false);

		}


	}
#endif
}

void h2Widget::on_PB_uartDetect_clicked()
{
	updateUartPorts();
}
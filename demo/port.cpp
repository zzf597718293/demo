#include "port.h"
#include<QDebug>
Port::Port(QWidget *parent) : QWidget(parent)
{
    m_serialPort = new QSerialPort();

}
Port::~Port()
{
    delete m_serialPort;
}
bool Port::openCom()
{
    //bool bTrue = false;
    if (m_serialPort->isOpen())
    {
        m_serialPort->clear();
        m_serialPort->close();
    }
    m_serialPort->setPortName("COM5");
    if(!m_serialPort->open(QIODevice::ReadWrite))
    {
        return false;
    }

    m_serialPort->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
}

bool Port::sendComm(int value)
{
    //QByteArray Data_1;
    QString temp = QString::number(value, 16);
    if (temp.length() == 1) {
        temp = "0" + temp;
    }
    temp = "4C 3A "+temp+" 0D";

    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = temp.length();
    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;) {
        hstr = temp.at(i).toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }

        i++;
        if (i >= len) {
            break;
        }

        lstr = temp.at(i).toLatin1();
        hexdata = hexStrToChar(hstr);
        lowhexdata = hexStrToChar(lstr);

        if ((hexdata == 16) || (lowhexdata == 16)) {
            break;
        } else {
            hexdata = hexdata * 16 + lowhexdata;
        }

        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }

    senddata.resize(hexdatalen);

    m_serialPort->write(senddata);
}

char Port::hexStrToChar(char data)
{
    if ((data >= '0') && (data <= '9')) {
        return data - 0x30;
    } else if ((data >= 'A') && (data <= 'F')) {
        return data - 'A' + 10;
    } else if ((data >= 'a') && (data <= 'f')) {
        return data - 'a' + 10;
    } else {
        return (-1);
    }
}

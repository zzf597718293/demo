#include "port.h"
#include<QDebug>
Port::Port(QObject *parent) : QObject(parent)
{
    m_serialPort = new QSerialPort();
    connect(m_serialPort,&QSerialPort::readyRead,this,&Port::receiveData);
}
Port::~Port()
{
    m_serialPort->close();
    delete m_serialPort;
}
bool Port::openCom(QString com)
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->clear();
        m_serialPort->close();
    }
    m_serialPort->setPortName(com);
    if(!m_serialPort->open(QIODevice::ReadWrite))
    {
        return false;
    }

    m_serialPort->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setDataTerminalReady(true);
    return true;
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

bool Port::sendTime(QString time)
{
    QString str;

    str = "54 3A "+time+" 0D";
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;) {
        hstr = str.at(i).toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }

        i++;
        if (i >= len) {
            break;
        }

        lstr = str.at(i).toLatin1();
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

void Port::receiveData()
{
    //m_serialPort->waitForReadyRead(30);
    QByteArray byte_data = m_serialPort->readAll();
    if(byte_data.size()==4){
        buffer = byte_data;
        m_serialPort->waitForReadyRead(30);
    }
    buffer = buffer.append(byte_data);
    QString string_data = buffer.toHex().toUpper();
    buffer.clear();
    emit sendData(string_data);

}

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include<QSerialPort>
#include<QSerialPortInfo>
#include<QList>
class Port : public QObject
{
    Q_OBJECT
public:
    explicit Port(QObject *parent = nullptr);
    ~Port();
    bool openCom();
    bool sendComm(int);
    char hexStrToChar(char);
signals:

private:
    QStringList *m_serialPortName;
    QSerialPort *m_serialPort;
};


#endif // SERIALPORT_H

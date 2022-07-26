#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QWidget>
#include<QSerialPort>
#include<QSerialPortInfo>
#include<QList>
class Port : public QWidget
{
    Q_OBJECT
public:
    explicit Port(QWidget *parent = nullptr);
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

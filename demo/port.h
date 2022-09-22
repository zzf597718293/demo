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
    bool openCom(QString);
    bool sendComm(int);
    bool sendTime(QString);
    char hexStrToChar(char);
    QStringList *m_serialPortName;
    QSerialPort *m_serialPort;
signals:
    void sendData(QString string_data);
private:
    QByteArray buffer;
    void receiveData();

};


#endif // SERIALPORT_H

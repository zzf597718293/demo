#ifndef DBPAGE_H
#define DBPAGE_H
#include <QtGui>
#include <QtSql>
#include <QtWidgets>
#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>
#include<QSqlQueryModel>
#include <QObject>
#include<QDebug>

class DbPage : public QObject
{
    Q_OBJECT
public:
    explicit DbPage(QObject *parent = nullptr);
    ~DbPage();

    void saveData();
    void saveSystem(QString,QString);
    void saveImage();
    void saveVedioStart();
    void saveVedioEnd();
    void showSystem();
    void selectData(QString);
    void selectData(int);
    void selectData(QString,int,int);
    void setTitle();
    void selectAttend(QString);
    void selectAssistant(QString);
    QList<QString> showAttend();
    QList<QString> showAssistant();
    QSqlQueryModel *qmodel;
    QString imageName;
    QString imagePath;      //图片路径
    QString vedioName;
    QString vedioPath;      //视频路径
    QString startTime;
    QString endTime;
private:
    QSqlDatabase database;



    QList<QString> columnNames; //列名集合
    QList<int> columnWidths;    //列宽集合
    QTableView *tableView;

    int chartNum;           //病历号
    QString serialNum;      //流水号
    QString bunkNum;        //床位号
    QString name;           //姓名
    int gender;             //性别
    int age;                //年龄
    QString attendName;     //主治医师
    QString assistantName;  //助理医师
    QString remark;         //备注


signals:

public slots:
    void setChartnum(const int&);
    void setSerialnum(const QString&);
    void setBunknum(const QString&);
    void setName(const QString&);
    void setGender(const int&);
    void setAge(const int&);
    void setAttendName(const QString&);
    void setAssistantName(const QString&);
    void setRemark(const QString&);
    void setColumnNames(const QList<QString> &columnNames);
    //设置列宽度集合
    void setColumnWidths(const QList<int> &columnWidths);
};

#endif // DBPAGE_H

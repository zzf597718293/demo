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
    void saveSystem(QString,QString,int,int,int,int);
    void saveImage();
    void saveVideoStart();
    void saveVideoEnd();
    void showSystem();
    void selectVideo(QString,QString,int);
    void selectImg(QString,QString,int);
    void setTitle(int);
    void selectAttend(QString);
    void selectAssistant(QString);
    void upDatePatien(QString, QString, QString, QString, QString, QString, int, QString, QString, QString);
    void deletePatien(QString);
    void deleteImage(QString);
    QString getImagePath(QString);
    QList<QString> showAttend();
    QList<QString> showAssistant();
    int getSerialAmount(QString); //接收当前日期YYYYMMhh在数据库中模糊查询个数并返回
    QList<QString> getImageList();
    QSqlQueryModel *qmodel;
    QString imageName;
    QString imagePath;      //图片路径
    int bright;
    int contrast;
    int chroma;
    int saturation;
    QString videoName;
    QString videoPath;      //视频路径
    QString startTime;
    QString endTime;
private:
    QSqlDatabase database;
    QList<QString> columnNames; //列名集合
    QList<int> columnWidths;    //列宽集合
    QTableView *tableView;
    QString chartNum;       //病历号
    QString serialNum;      //流水号
    QString bunkNum;        //床位号
    QString name;           //姓名
    QString gender;             //性别
    int age;                //年龄
    QString attendName;     //主治医师
    QString assistantName;  //助理医师
    QString remark;         //备注


signals:

public slots:
    void setChartnum(const QString&);
    void setSerialnum(const QString&);
    void setBunknum(const QString&);
    void setName(const QString&);
    void setGender(const QString&);
    void setAge(const int&);
    void setAttendName(const QString&);
    void setAssistantName(const QString&);
    void setRemark(const QString&);
    void setColumnNames(const QList<QString> &columnNames);
    //设置列宽度集合
    void setColumnWidths(const QList<int> &columnWidths);
};

#endif // DBPAGE_H

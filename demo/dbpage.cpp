#include "dbpage.h"

DbPage::DbPage(QObject *parent) : QObject(parent)
{
    qmodel = new QSqlQueryModel;
//    QString name;
database = QSqlDatabase::addDatabase("QSQLITE");
database.setDatabaseName("MyDataBase.db");
//    if (QSqlDatabase::contains("qt_sql_default_connection"))
//    {
//        database = QSqlDatabase::database("qt_sql_default_connection");
//    }
//    else
//    {

//        database = QSqlDatabase::addDatabase("QSQLITE");

//        database.setDatabaseName("MyDataBase.db");

//    }

    if (!database.open())
    {
        qDebug() << "" << database.lastError();
    }

    QSqlQuery sqlQuery;

    QString createSql = QString("CREATE TABLE patient (\
                                id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
                                chartnum VARCHAR(100) UNIQUE NOT NULL,\
                                name VARCHAR(10) NOT NULL,\
                                age INTEGER NOT NULL,\
                                gender INTEGER NOT NULL \
                                )");
    sqlQuery.prepare(createSql);

    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    sqlQuery.clear();
    createSql = QString("CREATE TABLE serial (\
                         id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
                         serialnum VARCHAR(100) ,\
                         bunknum VARCHAR(10) NOT NULL,\
                         attend VARCHAR(10) NOT NULL,\
                         assistant VARCHAR(10) NOT NULL,\
                         remark TEXT,\
                         chartnum VARCHAR(100) UNIQUE NOT NULL \
                         )");
    sqlQuery.prepare(createSql);

    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    sqlQuery.clear();
    createSql = QString("CREATE TABLE video (\
                         videoid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
                         videoname VARCHAR(20) NOT NULL,\
                         starttime DATETIME,\
                         endtime DATETIME,\
                         path VARCHAR(100)NOT NULL,\
                         chartnum VARCHAR(100)NOT NULL,\
                         serialnum VARCHAR(100) NOT NULL \
                         )");
    sqlQuery.prepare(createSql);

    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    sqlQuery.clear();
    createSql = QString("CREATE TABLE image (\
                         imageid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
                         imagename VARCHAR(20) NOT NULL,\
                         path VARCHAR(100),\
                         chartnum VARCHAR(100) NOT NULL,\
                         serialnum VARCHAR(100) NOT NULL \
                         )");
    sqlQuery.prepare(createSql);

    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }

    sqlQuery.clear();
    createSql = QString("CREATE TABLE system (\
                         id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
                         imagepath VARCHAR(255),\
                         videopath VARCHAR(255),\
                         bright INTEGER,\
                         contrast INTEGER,\
                         chroma INTEGER,\
                         saturation INTEGER \
                         )");
    sqlQuery.prepare(createSql);

    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    sqlQuery.clear();
    createSql = QString("CREATE TABLE attend (\
                         id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
                         attendname VARCHAR(10)\
                         )");
    sqlQuery.prepare(createSql);

    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }
    sqlQuery.clear();
    createSql = QString("CREATE TABLE assistant (\
                         id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,\
                         assistantname VARCHAR(10)\
                         )");
    sqlQuery.prepare(createSql);

    if(!sqlQuery.exec())
    {
        qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }

//    qmodel->setQuery("SELECT * FROM patient AS p "
//                     "INNER JOIN serial AS s "
//                     "ON p.chartnum = s.chartnum "
//                     "INNER JOIN video AS v "
//                     "ON v.chartnum = s.chartnum "
//                     "INNER JOIN image AS i "
//                     "ON s.chartnum = i.chartnum ");

//    setTitle();
}

DbPage::~DbPage()
{
   delete qmodel;
}

void DbPage::saveData()

{
    bool canBeSave = true;
//    if (QSqlDatabase::contains("qt_sqldefault_connection"))
//    {
//        database = QSqlDatabase::database("qt_sql_default_connection");
//    }
//    else
//    {
//        database = QSqlDatabase::addDatabase("QSQLITE");

//        database.setDatabaseName("MyDataBase.db");
//    }

//    if (!database.open())
//    {
//        QMessageBox warnBox(QMessageBox::Warning,"警告","数据库打开失败",QMessageBox::Yes);
//        warnBox.exec();
//        return;
//    }

    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO patient (chartnum,name,age,gender)VALUES(:chartnum,:name,:age,:gender)");
    sqlQuery.bindValue(":chartnum",chartNum);
    sqlQuery.bindValue(":name",name);
    sqlQuery.bindValue(":age",age);
    sqlQuery.bindValue(":gender",gender);
    if(!sqlQuery.exec())
    {
        qDebug() <<  sqlQuery.lastError();
        QMessageBox warnBox(QMessageBox::Warning,"警告","patient表添加失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }

    sqlQuery.clear();
    sqlQuery.prepare("INSERT INTO serial (serialnum,bunknum,attend,assistant,remark,chartnum)VALUES(:serialnum,:bunknum,:attend,:assistant,:remark,:chartnum)");
    sqlQuery.bindValue(":serialnum",serialNum);
    sqlQuery.bindValue(":bunknum",bunkNum);
    sqlQuery.bindValue(":attend",attendName);
    sqlQuery.bindValue(":assistant",assistantName);
    sqlQuery.bindValue(":remark",remark);
    sqlQuery.bindValue(":chartnum",chartNum);

    if(!sqlQuery.exec())
    {
        qDebug() <<  sqlQuery.lastError();
        QMessageBox warnBox(QMessageBox::Warning,"警告","serial表添加失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }

    /*sqlQuery.clear();
    sqlQuery.prepare("INSERT INTO image VALUES(:imagename,:path,:chartnum,:serialnum)");
    sqlQuery.bindValue(":imagename","");
    sqlQuery.bindValue(":path","");
    sqlQuery.bindValue(":chartnum",chartNum);
    sqlQuery.bindValue(":serialnum",serialNum);


    if(!sqlQuery.exec())
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","image表添加失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
    */
}

void DbPage::saveSystem(QString imagepath,QString videopath,int bright,int contrast,int chroma,int saturation)
{
//    if (QSqlDatabase::contains("qt_sqldefault_connection"))
//    {
//        database = QSqlDatabase::database("qt_sql_default_connection");
//    }
//    else
//    {
//        database = QSqlDatabase::addDatabase("QSQLITE");

//        database.setDatabaseName("MyDataBase.db");
//    }

//    if (!database.open())
//    {
//        QMessageBox warnBox(QMessageBox::Warning,"警告","数据库打开失败",QMessageBox::Yes);
//        warnBox.exec();
//        return;
//    }
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT id FROM system WHERE id=1");
    if(!sqlQuery.next()){
        sqlQuery.prepare("INSERT INTO system VALUES(:id,:imagepath,:videopath,:bright,:contrast,:chroma,:saturation)");
        sqlQuery.bindValue(":id",1);
        sqlQuery.bindValue(":imagepath",imagepath);
        sqlQuery.bindValue(":videopath",videopath);
        sqlQuery.bindValue(":bright",bright);
        sqlQuery.bindValue(":contrast",contrast);
        sqlQuery.bindValue(":chroma",chroma);
        sqlQuery.bindValue(":saturation",saturation);
        if(!sqlQuery.exec())
        {
            qDebug() <<  sqlQuery.lastError();
        }
    }
    else
    {
        sqlQuery.prepare("UPDATE system SET id=:id,imagepath=:imagepath,videopath=:videopath,bright=:bright,contrast=:contrast,chroma=:chroma,saturation=:saturation");
        sqlQuery.bindValue(":id",1);
        sqlQuery.bindValue(":imagepath",imagepath);
        sqlQuery.bindValue(":videopath",videopath);
        sqlQuery.bindValue(":bright",bright);
        sqlQuery.bindValue(":contrast",contrast);
        sqlQuery.bindValue(":chroma",chroma);
        sqlQuery.bindValue(":saturation",saturation);
            if(!sqlQuery.exec())
            {
                qDebug() <<  sqlQuery.lastError();
                QMessageBox warnBox(QMessageBox::Warning,"警告","内容",QMessageBox::Yes);
                warnBox.exec();
                return;
            }
    }

    sqlQuery.clear();
}

void DbPage::saveImage()
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO image (imagename,path,chartnum,serialnum) VALUES(:imagename,:path,:chartnum,:serialnum)");
    sqlQuery.bindValue(":imagename",imageName);
    sqlQuery.bindValue(":path",imagePath);
    sqlQuery.bindValue(":chartnum",chartNum);
    sqlQuery.bindValue(":serialnum",serialNum);
    if(!sqlQuery.exec())
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","image表添加失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
}

void DbPage::saveVideoStart()
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO video (videoname,starttime,path,chartnum,serialnum) VALUES(:videoname,:starttime,:path,:chartnum,:serialnum)");
    sqlQuery.bindValue(":videoname",videoName);
    sqlQuery.bindValue(":starttime",startTime);
    sqlQuery.bindValue(":path",videoPath);
    sqlQuery.bindValue(":chartnum",chartNum);
    sqlQuery.bindValue(":serialnum",serialNum);
    if(!sqlQuery.exec())
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","video表添加失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
}

void DbPage::saveVideoEnd()
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("UPDATE video SET endtime=:endtime WHERE videoname = "+videoName+"");
    sqlQuery.bindValue(":endtime",endTime);
    if(!sqlQuery.exec())
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","video表添加失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
}

void DbPage::showSystem()
{
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT * FROM system WHERE id=1");
    sqlQuery.next();
    imagePath = sqlQuery.value(1).toString();
    videoPath = sqlQuery.value(2).toString();
    bright = sqlQuery.value(3).toInt();
    contrast = sqlQuery.value(4).toInt();
    chroma = sqlQuery.value(5).toInt();
    saturation = sqlQuery.value(6).toInt();
}

void DbPage::selectVideo(QString str,QString type,int videoOrImg)
{
    if(type=="Chart"){
       qmodel->setQuery("SELECT * FROM patient AS p "
                        "INNER JOIN serial AS s "
                        "ON p.chartnum = s.chartnum "
                        "INNER JOIN video AS v "
                        "ON v.chartnum = s.chartnum "
                        " WHERE p.chartnum = '"+str+"'");
    }
    if(type=="Serial"){
        qmodel->setQuery("SELECT * FROM patient AS p "
                         "INNER JOIN serial AS s "
                         "ON p.chartnum = s.chartnum "
                         "INNER JOIN video AS v "
                         "ON v.chartnum = s.chartnum "
                         " WHERE s.serialnum = '"+str+"'");
    }
    if(type=="Name"){
        qmodel->setQuery("SELECT * FROM patient AS p INNER JOIN serial AS s ON p.chartnum = s.chartnum INNER JOIN video AS v INNER JOIN image AS i WHERE p.name LIKE '%"+str+"%'");
    }
    if(type=="Bunk"){
        qmodel->setQuery("SELECT * FROM patient AS p "
                         "INNER JOIN serial AS s "
                         "ON p.chartnum = s.chartnum "
                         "INNER JOIN video AS v "
                         "ON v.chartnum = s.chartnum "
                         " WHERE s.bunknum = '"+str+"'");
    }
    if(type=="Remark"){
        qmodel->setQuery("SELECT * FROM patient AS p "
                         "INNER JOIN serial AS s "
                         "ON p.chartnum = s.chartnum "
                         "INNER JOIN video AS v "
                         "ON v.chartnum = s.chartnum "
                         " WHERE s.remark = '"+str+"'");
    }
    setTitle(videoOrImg);
}

void DbPage::selectImg(QString str,QString type,int videoOrImg)
{
    if(type=="Chart"){
       qmodel->setQuery("SELECT * FROM patient AS p "
                        "INNER JOIN serial AS s "
                        "ON p.chartnum = s.chartnum "
                        "INNER JOIN image AS i "
                        "ON i.chartnum = s.chartnum "
                        " WHERE p.chartnum = '"+str+"'");
    }
    if(type=="Serial"){
        qmodel->setQuery("SELECT * FROM patient AS p "
                         "INNER JOIN serial AS s "
                         "ON p.chartnum = s.chartnum "
                         "INNER JOIN image AS i "
                         "ON i.chartnum = s.chartnum "
                         " WHERE s.serialnum = '"+str+"'");
    }
    if(type=="Name"){
        qmodel->setQuery("SELECT * FROM patient AS p INNER JOIN serial AS s ON p.chartnum = s.chartnum INNER JOIN video AS v INNER JOIN image AS i WHERE p.name LIKE '%"+str+"%'");
    }
    if(type=="Bunk"){
        qmodel->setQuery("SELECT * FROM patient AS p "
                         "INNER JOIN serial AS s "
                         "ON p.chartnum = s.chartnum "
                         "INNER JOIN image AS i "
                         "ON i.chartnum = s.chartnum "
                         " WHERE s.bunknum = '"+str+"'");
    }
    if(type=="Remark"){
        qmodel->setQuery("SELECT * FROM patient AS p "
                         "INNER JOIN serial AS s "
                         "ON p.chartnum = s.chartnum "
                         "INNER JOIN image AS i "
                         "ON i.chartnum = s.chartnum "
                         " WHERE s.remark = '"+str+"'");
    }
    setTitle(videoOrImg);
}



void DbPage::setChartnum(const QString &chartNum)
{
    this->chartNum = chartNum;
}
void DbPage::setSerialnum(const QString &serialNum)
{
    this->serialNum = serialNum;
}

void DbPage::setBunknum(const QString &bunkNum)
{
    this->bunkNum = bunkNum;
}

void DbPage::setName(const QString &name)
{
    this->name = name;
}

void DbPage::setGender(const int &gender)
{
    this->gender = gender;
}

void DbPage::setAge(const int &age)
{
    this->age = age;
}

void DbPage::setAttendName(const QString &attendName)
{
    this->attendName = attendName;
}

void DbPage::setAssistantName(const QString &assistantName)
{
    this->assistantName = assistantName;
}

void DbPage::setRemark(const QString &remark)
{
    this->remark = remark;
}
void DbPage::setColumnNames(const QList<QString> &columnNames)
{
    this->columnNames = columnNames;
}

void DbPage::setColumnWidths(const QList<int> &columnWidths)
{
    this->columnWidths = columnWidths;
}

void DbPage::setTitle(int videoOrImg)
{
    if(videoOrImg==1){
        qmodel->removeColumn(0);
        qmodel->removeColumn(4);
        qmodel->removeColumn(9);
        qmodel->removeColumn(9);
        qmodel->removeColumn(9);
        qmodel->removeColumn(12);
        qmodel->removeColumn(12);
        qmodel->setHeaderData(0,Qt::Horizontal,"病历号");
        qmodel->setHeaderData(1,Qt::Horizontal,"姓名");
        qmodel->setHeaderData(2,Qt::Horizontal,"年龄");
        qmodel->setHeaderData(3,Qt::Horizontal,"性别");
        qmodel->setHeaderData(4,Qt::Horizontal,"流水号");
        qmodel->setHeaderData(5,Qt::Horizontal,"病床号");
        qmodel->setHeaderData(6,Qt::Horizontal,"主治医师");
        qmodel->setHeaderData(7,Qt::Horizontal,"助理医师");
        qmodel->setHeaderData(8,Qt::Horizontal,"备注");
        qmodel->setHeaderData(9,Qt::Horizontal,"视频开始时间");
        qmodel->setHeaderData(10,Qt::Horizontal,"视频结束时间");
        qmodel->setHeaderData(11,Qt::Horizontal,"视频保存路径");
        qmodel->setHeaderData(12,Qt::Horizontal,"图片保存路径");
    }else{
        qmodel->removeColumn(0);
        qmodel->removeColumn(4);
        qmodel->removeColumn(9);
        qmodel->removeColumn(9);
        qmodel->removeColumn(11);
        qmodel->removeColumn(11);
        qmodel->setHeaderData(0,Qt::Horizontal,"病历号");
        qmodel->setHeaderData(1,Qt::Horizontal,"姓名");
        qmodel->setHeaderData(2,Qt::Horizontal,"年龄");
        qmodel->setHeaderData(3,Qt::Horizontal,"性别");
        qmodel->setHeaderData(4,Qt::Horizontal,"流水号");
        qmodel->setHeaderData(5,Qt::Horizontal,"病床号");
        qmodel->setHeaderData(6,Qt::Horizontal,"主治医师");
        qmodel->setHeaderData(7,Qt::Horizontal,"助理医师");
        qmodel->setHeaderData(8,Qt::Horizontal,"备注");
        qmodel->setHeaderData(9,Qt::Horizontal,"图片名称");
        qmodel->setHeaderData(10,Qt::Horizontal,"图片保存路径");
    }
}

void DbPage::selectAttend(QString name)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO attend (attendname) VALUES(:attendname)");
    sqlQuery.bindValue(":attendname",name);

    if(!sqlQuery.exec())
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","attend表添加失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
}

void DbPage::selectAssistant(QString name)
{
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO assistant (assistantname) VALUES(:assistantname)");
    sqlQuery.bindValue(":assistantname",name);

    if(!sqlQuery.exec())
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","assistant表添加失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
}

QList<QString> DbPage::showAttend()     //遍历attend(主治医师)表 存到List中
{
    QList<QString> attendList;
    QSqlQuery sqlQuery;
    sqlQuery.prepare("SELECT *FROM attend");
    sqlQuery.exec();
    while (sqlQuery.next()) {
        attendList.append(sqlQuery.value("attendname").toString());
    }
    return attendList;
}

QList<QString> DbPage::showAssistant() //遍历assistant(助理医师)表 存到List中
{
    QList<QString> assistantList;
    QSqlQuery sqlQuery;
    sqlQuery.prepare("SELECT *FROM assistant");
    sqlQuery.exec();
    while (sqlQuery.next()) {
        assistantList.append(sqlQuery.value("assistantname").toString());
    }
    return assistantList;
}

#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    init();
}

Widget::~Widget()
{

    vid.release();
    delete ui;
    delete myThread;

}

QRect Widget::getScreenRect(bool available)
{
    QRect rect;
    int screenIndex = getScreenIndex();
    if (available) {
        rect = qApp->screens().at(screenIndex)->availableGeometry();
    } else {
        rect = qApp->screens().at(screenIndex)->geometry();
    }
    return rect;
}

int Widget::getScreenIndex()
{
    int screenIndex = 0;
    int screenCount = qApp->screens().count();
    if (screenCount > 1) {
        //找到当前鼠标所在屏幕
        QPoint pos = QCursor::pos();
        for (int i = 0; i < screenCount; ++i) {
            if (qApp->screens().at(i)->geometry().contains(pos)) {
                screenIndex = i;
                break;
            }
        }
    }
    return screenIndex;
}

void Widget::init()
{
    setWindowFlags(Qt::FramelessWindowHint); //设置无边框
    
    QPixmap img = QPixmap(":/title.jpg");
    QSize PixSize = ui->labelTitle->size();
    img.scaled(PixSize,Qt::KeepAspectRatioByExpanding);
    ui->labelTitle->setScaledContents(true);
    ui->labelTitle->setPixmap(img);

    mainThread = new QThread(this);
    myThread = new VedioThread;
    myThread->moveToThread(mainThread);

    childPortThread = new QThread(this);
    port1 = new Port();
    port1->moveToThread(childPortThread);
    childPortThread->start();
    if(!port1->openCom())   //打开串口
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","串口打开失败",QMessageBox::Yes);
        warnBox.exec();
    }
    dbPage = new DbPage(this);
    dbPage->showSystem();
    ui->edtImagePath->setText(dbPage->imagePath);
    ui->edtVedioPath->setText(dbPage->vedioPath);
    ui->label->setStyleSheet("QLabel{background-color:rgb(22,39,53);}");
    QSize icoSize(32, 32);
    int icoWidth = 85;
    loadStyle();           //设置全局样式
    ui->edtChart->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->cbGender->addItem("男");
    ui->cbGender->addItem("女");
    ui->comAssistant->addItem("");
    ui->comAttend->addItem("");
    ui->comAssistant_2->addItem("");
    ui->comAttend_2->addItem("");
    timer = new QTimer(this);    //初始化定时器用于摄像头的实时显示
    getAttend();
    getAssistant();
    QList<QAbstractButton *> tbtns = ui->widgetTop->findChildren<QAbstractButton *>();
    foreach (QAbstractButton *btn, tbtns) {
    btn->setIconSize(icoSize);
    btn->setMinimumWidth(icoWidth);
    btn->setCheckable(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(videoOpen()));
    connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }
    ui->btnMain->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->btnMain->setIcon(QIcon(":/main_main.png"));
    ui->btnData->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->btnData->setIcon(QIcon(":/main_data.png"));
    ui->btnConfig->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->btnConfig->setIcon(QIcon(":/main_config.png"));
}

void Widget::loadStyle()
{
    QElapsedTimer time;
    time.start();

    //加载样式表
    QString qss;
    QFile file(":/sd.css");
    if (file.open(QFile::ReadOnly)) {
        //用QTextStream读取样式文件不用区分文件编码 带bom也行
        QStringList list;
        QTextStream in(&file);
        //in.setCodec("utf-8");
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }

        file.close();
        qss = list.join("\n");
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(paletteColor));
        //用时主要在下面这句
        qApp->setStyleSheet(qss);
    }

    qDebug() << "用时:" << time.elapsed();
}

void Widget::getAttend() //初始化主治医师
{
    QList<QString> attendList;
    attendList = dbPage->showAttend();
    QList<QString>::const_iterator iter;
    ui->comAttend->clear();
    ui->comAttend_2->clear();
    for(iter = attendList.begin(); iter != attendList.end();++iter)
    {
        ui->comAttend->addItem(*iter);
        ui->comAttend_2->addItem(*iter);
    }
}

void Widget::getAssistant() //初始化助理医师
{
    QList<QString> assistantList;
    assistantList = dbPage->showAssistant();
    QList<QString>::const_iterator iter;
    ui->comAssistant->clear();
    ui->comAssistant_2->clear();
    for(iter = assistantList.begin(); iter != assistantList.end();++iter)
    {
        ui->comAssistant->addItem(*iter);
        ui->comAssistant_2->addItem(*iter);
    }
}

void Widget::videoOpen()
{
   vid>>frame;
   cv::Mat frame2;
   cv::Mat frame3;
   cv::Mat frame4;
   //cv::cvtColor(frame,frame1,CV_BGR2RGB);   //不处理的话颜色会偏蓝

   //frame2 = img.ImageBright(frame,beta,alpha); //调整亮度和对比度函数
   //frame3 = img.ImageSaturation(frame2,saturation);    //调整饱和度函数
   frame4 = img.ImageCV(frame);
   //frame3 = imgPro(frame);
   //cv::cvtColor(frame4,frame4,CV_BGR2RGB);
   //cv::imshow("video1", frame);
   //cv::namedWindow("video", cv::WINDOW_AUTOSIZE);

   cv::cvtColor(frame4,frame4,cv::COLOR_BGR2RGB);
   //cv::imshow("video", frame4);
   frame4.convertTo(frame4,CV_8UC3,255.0);
   QImage dst =QImage(frame4.data,frame4.cols,frame4.rows,frame4.step,QImage::Format_RGB888); //将Mat转换成Image放到Label里显示
   ui->label->setPixmap(QPixmap::fromImage(dst));

   //cv::imshow("ww",frame3);

   //cv::waitKey(0);
}

void Widget::on_btnOn_clicked()
{
    vid.open(0);        //打开摄像头
    if(vid.isOpened())
    {
    timer->start(33);   //摄像头每秒30帧



    ui->btnOn->setEnabled(false);
    ui->btnOff->setEnabled(true);
    ui->btnScreenshots->setEnabled(true);
    ui->btnStarRec->setEnabled(true);
    ui->btnStopRec->setEnabled(true);
    ui->sliderBright->setEnabled(true);
    ui->sliderContrast->setEnabled(true);
    ui->sliderSaturation->setEnabled(true);
    //摄像头未开启时各项参数不可调整

    }
    else
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","摄像头打开失败",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
}

void Widget::timer_out()
{
    videoOpen();
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    port1->sendComm(value);  //Slider的值每次改变都通过串口下发相应亮度值
}

void Widget::on_btnOff_clicked()
{
    timer->stop(); //关闭串口释放资源
    delete  port1;
    vid.release();
    ui->btnOff->setEnabled(false);
    ui->btnOn->setEnabled(true);
}

void Widget::on_btnScreenshots_clicked()
{
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    QString str = ui->edtImagePath->text();
    QString systimeDate = QString(str).append(systime.toString("yyyyMMddhhmmss")).append(".png"); //拼接保存路径
    string path = systimeDate.toStdString();
    dbPage->imageName = systime.toString("yyyyMMddhhmmss");
    dbPage->imagePath = ui->edtImagePath->text();
    cv::imwrite(path,frame); //保存图片
    dbPage->saveImage();
}

void Widget::on_btnStarRec_clicked()
{
    mainThread->start();
    myThread->openCamera();
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    QString str = ui->edtVedioPath->text();
    QString systimeDate = QString(str).append(systime.toString("yyyyMMddhhmmss")).append(".mp4"); //拼接保存路径
    //string path = systimeDate.toStdString();
    dbPage->vedioName = systime.toString("yyyyMMddhhmmss");
    dbPage->vedioPath = ui->edtImagePath->text();
    ui->btnStarRec->setEnabled(false);
    ui->btnStopRec->setEnabled(true);
    dbPage->startTime = QString(systime.toString("yyyy-MM-dd-hh:mm:ss"));
    dbPage->saveVedioStart();
    myThread->ThreadStart(systimeDate);

}

void Widget::on_btnStopRec_clicked()
{
    ui->btnStarRec->setEnabled(true);
    ui->btnStopRec->setEnabled(false);
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    dbPage->endTime = QString(systime.toString("yyyy-MM-dd-hh:mm:ss"));
    dbPage->saveVedioEnd();
    myThread->ThreadStop();
    mainThread->quit();
    mainThread->wait();

}

void Widget::on_sliderBright_valueChanged(int value)
{
    beta = float(value);
}

void Widget::on_sliderContrast_valueChanged(int value)
{
    alpha = float(value);
}



void Widget::on_sliderSaturation_valueChanged(int value)
{
    saturation = value;
}

void Widget::buttonClick()
{
    QAbstractButton *b = (QAbstractButton *)sender();
    QString name = b->text();

    QList<QAbstractButton *> tbtns = ui->widgetTop->findChildren<QAbstractButton *>();
    foreach (QAbstractButton *btn, tbtns) {
        btn->setChecked(btn == b);
    }

    if (name == "视频监控") {
        ui->stackedWidget->setCurrentIndex(0);
    } else if (name == "数据查询") {
        ui->stackedWidget->setCurrentIndex(1);
    } else if (name == "系统设置") {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void Widget::on_btnMenu_Close_clicked()
{
    close();
}

void Widget::on_btnSave_clicked()
{
    if (ui->edtSerial->text() ==""){
        QMessageBox warnBox(QMessageBox::Warning,"警告","流水号不得为空",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
    if (ui->edtChart->text() ==""){
        QMessageBox warnBox(QMessageBox::Warning,"警告","病例号不得为空",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
    if (ui->edtBunk->text() ==""){
        QMessageBox warnBox(QMessageBox::Warning,"警告","床位号不得为空",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
    if (ui->edtName->text() ==""){
        QMessageBox warnBox(QMessageBox::Warning,"警告","姓名不得为空",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
    if (ui->edtAge->text().toInt()<=0){
        QMessageBox warnBox(QMessageBox::Warning,"警告","请输入正确年龄",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
    dbPage->setChartnum(ui->edtChart->text().toInt());
    dbPage->setSerialnum(ui->edtSerial->text());
    dbPage->setBunknum(ui->edtBunk->text());
    dbPage->setName(ui->edtName->text());
    if(ui->cbGender->currentText() == '男')
    dbPage->setGender(1);
    else
    dbPage->setGender(0);
    dbPage->setAge(ui->edtAge->text().toInt());
    dbPage->setAttendName(ui->comAttend->currentText());
    dbPage->setAssistantName(ui->comAssistant->currentText());
    dbPage->setRemark(ui->textEdit->toPlainText());
    dbPage->saveData();
    ui->btnOn->setEnabled(true);

    //ui->btnOn->isEnabled();
    //ui->btnOn->isEnabled();
}

void Widget::on_btnConfig_clicked()
{


}

void Widget::on_btnData_clicked()
{
    ui->tableView->setModel(dbPage->qmodel);
    ui->tableView->setColumnWidth(9,150);
    ui->tableView->setColumnWidth(10,150);
    ui->tableView->setColumnWidth(11,150);
    ui->tableView->setColumnWidth(12,150);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->show();
}

void Widget::on_btnSelect_clicked()
{
    if(ui->selectSerial->text()!="")
    {
        dbPage->selectData(ui->selectSerial->text().toInt());
    }
    if(ui->selectChart->text()!="")
    {
        dbPage->selectData(ui->selectChart->text().toInt());
    }
    if(ui->selectSerial->text()=="" && ui->selectChart->text()=="" && ui->selectName->text()!="")
    {
        dbPage->selectData(ui->selectName->text());
    }

    if(ui->selectName->text()!="")
    {
        //dbPage->selectData();
    }
    if(ui->selectName->text()!="" && ui->selectAgeMin->text()!="" && ui->selectAgeMax->text()!="")
    {
        dbPage->selectData(ui->selectName->text(),ui->selectAgeMin->text().toInt(),ui->selectAgeMax->text().toInt());
    }
}

void Widget::on_choiceImagePath_clicked()
{
    QString curPath = QCoreApplication::applicationDirPath();
    QString dlgTitle= "选择一个目录";
    QString choiceDir = QFileDialog::getExistingDirectory(this,dlgTitle,curPath,QFileDialog::ShowDirsOnly);
    ui->edtImagePath->setText(choiceDir);
}

void Widget::on_choiceVedioPath_clicked()
{
    QString curPath = QCoreApplication::applicationDirPath();
    QString dlgTitle= "选择一个目录";
    QString choiceDir = QFileDialog::getExistingDirectory(this,dlgTitle,curPath,QFileDialog::ShowDirsOnly);
    ui->edtVedioPath->setText(choiceDir);
}

void Widget::on_btnSystemSave_clicked()
{
    dbPage->saveSystem(ui->edtImagePath->text(),ui->edtVedioPath->text());
    dbPage->showSystem();
    ui->edtImagePath->setText(dbPage->imagePath);
    ui->edtVedioPath->setText(dbPage->vedioPath);
}

void Widget::on_btnAttend_clicked()
{
    doctorForm *doctor = new doctorForm;

    doctor->setWindowModality(Qt::ApplicationModal);
    doctor->changeAttend();
    doctor->show();
}


void Widget::on_btnAssistant_clicked()
{
    doctorForm *doctor = new doctorForm;

    doctor->setWindowModality(Qt::ApplicationModal);
    doctor->changeAssistant();
    doctor->show();
}

void Widget::on_btnMenu_Max_clicked()
{
    static bool max = false;
    static QRect location = this->geometry();

    if (max) {
        this->setGeometry(location);
    } else {
        location = this->geometry();
        this->setGeometry(this->getScreenRect(true));
    }


    max = !max;
}

void Widget::on_btnMenu_Min_clicked()
{
    setWindowState(Qt::WindowMinimized);
}



#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    statusBar = new QStatusBar(this);
    layout()->addWidget(statusBar);
    //statusBar->setStyleSheet("QStatusBar::item{border:0px}");
    labelSerial = new QLabel(this);
    labelSerial->setText("手柄序列号：");

    labelUsedtime = new QLabel(this);
    statusBar->addWidget(labelSerial);
    statusBar->addWidget(labelUsedtime);
    statusBar->setStyleSheet("background:#57AAF7");
    labelSerial->setStyleSheet("color:#ffffff");
    labelUsedtime->setStyleSheet("color:#ffffff");
    init();
}

Widget::~Widget()
{

    vid.release();
    childPortThread->quit();
    childPortThread->wait();
    //delete videoplay;
    delete  port1;
    delete myThread;
    delete ui;


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

Widget::controlEnable(bool canBeTrue)
{
    ui->btnMain->setEnabled(canBeTrue);
    ui->btnData->setEnabled(canBeTrue);
    ui->edtSerial->setEnabled(canBeTrue);
    ui->edtChart->setEnabled(canBeTrue);
    ui->edtBunk->setEnabled(canBeTrue);
    ui->edtName->setEnabled(canBeTrue);
    ui->cbGender->setEnabled(canBeTrue);
    ui->edtAge->setEnabled(canBeTrue);
    ui->comAttend->setEnabled(canBeTrue);
    ui->comAssistant->setEnabled(canBeTrue);
    ui->textEdit->setEnabled(canBeTrue);
    ui->btnSave->setEnabled(canBeTrue);
}

void Widget::init()
{
    setWindowFlags(Qt::FramelessWindowHint); //设置无边框
    
    QPixmap img = QPixmap(":/logo.png");    //设置标题图片
    QSize PixSize = ui->labelTitle->size();
    img.scaled(PixSize,Qt::KeepAspectRatioByExpanding);
    ui->labelTitle->setScaledContents(true);
    ui->labelTitle->setPixmap(img);
    timerPort = new QTimer(this);
    realTime = new QTimer(this);
    timerDetect = new QTimer(this);
    timerSysTime = new QTimer(this);
    connect(timerSysTime,SIGNAL(timeout()),this,SLOT(on_btnReadSerial_clicked()));
    connect(realTime,SIGNAL(timeout()),this,SLOT(showTime()));
    realTime->start(1);
    mainThread = new QThread(this);          //视频录制线程
    myThread = new VideoThread;
    myThread->moveToThread(mainThread);

    childPortThread = new QThread(this);     //串口线程
    port1 = new Port(this);
    port1->moveToThread(childPortThread);
    childPortThread->start();

    connect(port1,SIGNAL(sendData(QString)),this,SLOT(handleToCamera(QString)));


    dbPage = new DbPage(this);
    dbPage->showSystem();
    ui->edtImagePath->setText(dbPage->imagePath);
    ui->edtVideoPath->setText(dbPage->videoPath);
    ui->slider_Bright->setValue(dbPage->bright);
    ui->slider_Contrast->setValue(dbPage->contrast);
    ui->slider_Chroma->setValue(dbPage->chroma);
    ui->slider_Saturation->setValue(dbPage->saturation);
    ui->sp_Bright->setValue(dbPage->bright);
    ui->spContrast->setValue(dbPage->contrast);
    ui->sp_Chroma->setValue(dbPage->chroma);
    ui->sp_Saturation->setValue(dbPage->saturation);
    ui->cbIfFrame->setChecked(dbPage->ifFrame);
    ui->CbWhite_Balance->setChecked(dbPage->whitebalance);
    ui->LEDSlider->setEnabled(false);
    videoplay = new VideoPlay;
    if (dbPage->ifFrame == true){
        //videoplay->ui->labelVideo->setFrameStyle(QFrame::Box);
        //videoplay->ui->label->setFrameStyle(QFrame::Box);
        ui->label->setFrameStyle(QFrame::Box);
    }else {
        //videoplay->ui->labelVideo->setFrameStyle(QFrame::NoFrame);
        //videoplay->ui->label->setFrameStyle(QFrame::NoFrame);
        ui->label->setFrameStyle(QFrame::NoFrame);
    }

    //ui->label->setStyleSheet("QLabel{background-color:rgb(22,39,53);}");
    QSize icoSize(32, 32);
    int icoWidth = 85;
    ui->btnImgMag->setEnabled(false);
    ui->btnImgRed->setEnabled(false);
    ui->edtChart->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->edtSerial->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->edtBunk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));

    ui->selectSerial->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->selectChart->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->selectBunk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->selectAgeMin->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->selectAgeMax->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->labCamera->setText("摄像头未开启");
    ui->cbGender->addItem("男");
    ui->cbGender->addItem("女");
    ui->cbGender_2->addItem("男");
    ui->cbGender_2->addItem("女");
    ui->comAssistant->setEditText("");
    ui->comAttend->setEditText("");
    ui->comAssistant_2->setEditText("");
    ui->comAttend_2->setEditText("");
    timer = new QTimer(this);    //初始化定时器用于摄像头的实时显示
    getAttend();
    getAssistant();
    QList<QAbstractButton *> tbtns = ui->widgetTop->findChildren<QAbstractButton *>();
    foreach (QAbstractButton *btn, tbtns) {
    btn->setIconSize(icoSize);
    btn->setMinimumWidth(icoWidth);
    btn->setCheckable(true);
    connect(timer,SIGNAL(timeout()),this,SLOT(videoOpen()));
    connect(timerDetect,SIGNAL(timeout()),this,SLOT(autoDetect()));
    connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }
    ui->btnMain->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->btnMain->setIcon(QIcon(":/main_main.png"));
    ui->btnData->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->btnData->setIcon(QIcon(":/main_data.png"));
    ui->btnConfig->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->btnConfig->setIcon(QIcon(":/main_config.png"));
    ui->labCamImg->setStyleSheet("background-image: url(:/cameraOff.png)");
    ui->edtSerial->setText(autoSerial());

    ui->btnNext->setEnabled(false);
    ui->btnPrevious->setEnabled(false);
    ui->btnFirst->setEnabled(false);
    ui->btnLast->setEnabled(false);
    ui->labNowPage->setVisible(false);
    ui->labTotal->setVisible(false);
    ui->labTotalPage->setVisible(false);


    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(showVideoReplay(QModelIndex)));
    loadStyle();           //设置全局样式
    if(!port1->openCom("COM5"))   //打开串口
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","串口打开失败",QMessageBox::Yes);
        warnBox.exec();
    }else{
            portOpen = "COM5";
            timerDetect->start(50);
//            QMessageBox warnBox(QMessageBox::Information,"提示","串口打开成功",QMessageBox::Yes);
//            warnBox.exec();
            readTime();
            timerSysTime->start(50);
    }

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

void Widget::readSerial()
{
    QString data = "4E 3A 3F 0D";
    QByteArray byteData = QByteArray::fromHex(data.toLatin1());
    port1->m_serialPort->write(byteData);
    port1->m_serialPort->waitForBytesWritten(30);
    timerSysTime->stop();
}

void Widget::readTime()
{
    QString data = "54 3A 3F 0D";
    QByteArray byteData = QByteArray::fromHex(data.toLatin1());
    port1->m_serialPort->write(byteData);
    port1->m_serialPort->waitForBytesWritten(30);
}

void Widget::autoDetect() //自动检测
{
    QStringList serialportinfo;
    foreach(QSerialPortInfo info,QSerialPortInfo::availablePorts())
        {
            serialportinfo<<info.portName();
        }
    int index = serialportinfo.indexOf(portOpen);
    if(index == -1 ){
        QMessageBox warnBox(QMessageBox::Information,"提示","串口被拔出，请检查并重新打开串口",QMessageBox::Yes);
        warnBox.exec();
    }
}

QString Widget::autoSerial() //自动生成流水号
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyyMMdd%");
    int num = dbPage->getSerialAmount(str);
    str = time.toString("yyyyMMdd");
    return str+QString("%1").arg(num+1,4,10,QChar('0'));
}

void Widget::showTime()
{
    QDateTime time = QDateTime::currentDateTime();
    QString txtTime = time.toString("yyyy-MM-dd hh:mm:ss"); // 这儿有各种格式，比如 ("yyyy-MM-dd hh:mm:ss dddd") 等等;
    ui->labelTime->setText(txtTime);

}

void Widget::handleToCamera(QString recData)
{
    if(recData == "4B3A530D"){
        on_btnScreenshots_clicked();
    }
    if(recData == "4B3A4C0D"){
        if(videoIsStart){
            on_btnStarRec_clicked();
        }else{
            on_btnStopRec_clicked();
        }
    }
    if(recData.length()==26){
        QString dateRecord="";
        dateRecord = recData.mid(15,8);
        labelSerial->setText("手柄序列号："+dateRecord+"");
    }
    if(recData.length()==36){
        if(recData.mid(18,2)=="00"){
            QMessageBox warnBox(QMessageBox::Information,"提示","首次使用",QMessageBox::Yes);
            warnBox.exec();
        }else{
            QString dateRecord="";
            dateRecord = recData.mid(20,4)+"-";
            dateRecord = dateRecord+recData.mid(24,2)+"-";
            dateRecord = dateRecord+recData.mid(26,2)+" ";
            dateRecord = dateRecord+recData.mid(28,2)+":";
            dateRecord = dateRecord+recData.mid(30,2);
            QDateTime dateTime = QDateTime::fromString(dateRecord,"yyyy-MM-dd hh:mm");
            labelUsedtime->setText("上次使用时间："+dateTime.toString("yyyy-MM-dd hh:mm")+"");
//            int i =dateTime.secsTo(QDateTime::currentDateTime());
//            qDebug()<<i;
            if(dateTime.secsTo(QDateTime::currentDateTime())>14400)
            {
                QMessageBox warnBox(QMessageBox::Warning,"警告","该设备已使用过，上次使用时间："+dateTime.toString("yyyy-MM-dd hh:mm")+"",QMessageBox::Yes);
                warnBox.exec();
            }

        }
    }
}

void Widget::videoOpen()
{
   vid>>frame;
   cv::Mat frame1;
   cv::Mat frame2;
   cv::Mat frame3;
   cv::Mat frame4;
   cv::Mat frame5;
   //cv::cvtColor(frame,frame1,CV_BGR2RGB);   //不处理的话颜色会偏蓝
   if (imgSize == 1){
       cv::resize(frame,frame,cv::Size(440,450));
   }else if(imgSize == 2){
       cv::resize(frame,frame,cv::Size(480,500));
   }else if(imgSize == 3){
       cv::resize(frame,frame,cv::Size(520,540));
   }else if(imgSize == 4){
       cv::resize(frame,frame,cv::Size(560,575));
   }else if(imgSize == 5){
       cv::resize(frame,frame,cv::Size(600,600));
   }

   frame1 = img.ImageHue(frame,hue);
   if(ui->cbWhiteBalance->isChecked()){
       frame2 = img.ImageWhitebalance(frame1);
   }else{
       frame2 =frame1;
   }

   frame3 = img.ImageBright(frame2,beta/10,alpha); //调整亮度和对比度函数
   frame4 = img.ImageSaturation(frame3,saturation);    //调整饱和度函数
   frame5 = img.ImageCV(frame4);
   //frame3 = imgPro(frame);
   //cv::cvtColor(frame4,frame4,CV_BGR2RGB);
   //cv::imshow("video1", frame);
   //cv::namedWindow("video", cv::WINDOW_AUTOSIZE);

   cv::cvtColor(frame5,frame5,cv::COLOR_BGR2RGB);
   //cv::imshow("video", frame4);
   frame5.convertTo(frame5,CV_8UC3,255.0);
   //cv::resize(frame5, frame5, cv::Size(frame5.cols*2, frame5.rows*2), cv::INTER_LINEAR);
   QImage dst =QImage(frame5.data,frame5.cols,frame5.rows,frame5.step,QImage::Format_RGB888); //将Mat转换成Image放到Label里显示
   QMatrix matrix;
   matrix.rotate(revolve);
   ui->label->setPixmap(QPixmap::fromImage(dst).transformed(matrix,Qt::SmoothTransformation));

   //cv::imshow("ww",frame3);

   //cv::waitKey(0);
}

void Widget::showVideoReplay(const QModelIndex &index) //传入表格中，被双击行的index
{ 
    QSqlRecord record = dbPage->qmodel->record(index.row()); //获取该行的所有字段信息
    if(videoOrImg ==1){
        QString strName = record.value(9).toString(); //获取视频开始时间字段信息
        strName = strName.remove(4,1).remove(6,1).remove(8,1).remove(10,1).remove(12,1);
        QString strPath = record.value(11).toString().append("/");
        videoplay = new VideoPlay;
        videoThread = new QThread(this);
        videoplay->imgPath = record.value(11).toString();
        videoplay->setWindowModality(Qt::ApplicationModal);
        videoplay->show();
        videoplay->getVideoName(strName,strPath);
        videoplay->getPatienInfo(record.value(4).toString(),record.value(0).toString(),record.value(5).toString(),record.value(1).toString(),record.value(3).toString(),record.value(2).toInt(),record.value(6).toString(),record.value(7).toString(),record.value(8).toString());
    }else{
        QString strName = record.value(9).toString(); //获取视频开始时间字段信息
        QString strPath = record.value(10).toString();
        screenshot = new Screenshot;
        screenshot->setWindowModality(Qt::ApplicationModal);
        screenshot->show();
        screenshot->getImgPatn(strName,strPath);
    }
}

void Widget::on_btnOn_clicked()
{
    revolve = 0;
    vid.open(0);        //打开摄像头
    if(vid.isOpened())
    {
    ui->labCamera->setText("摄像头已开启");
    timer->start(33);   //摄像头每秒30帧

    ui->btnOn->setEnabled(false);
    ui->btnOff->setEnabled(true);
    ui->btnScreenshots->setEnabled(true);
    ui->btnStarRec->setEnabled(true);
    ui->sliderBright->setEnabled(true);
    ui->sliderContrast->setEnabled(true);
    ui->sliderSaturation->setEnabled(true);
    ui->LEDSlider->setEnabled(true);
    ui->btnLeft->setEnabled(true);
    ui->btnRight->setEnabled(true);
    ui->spBright->setEnabled(true);
    ui->spContrast->setEnabled(true);
    ui->spSaturation->setEnabled(true);
    ui->spChroma->setEnabled(true);
    ui->cbWhiteBalance->setEnabled(true);
    ui->btnImgMag->setEnabled(true);
    ui->btnImgRed->setEnabled(true);
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

void Widget::on_LEDSlider_valueChanged(int value)
{
    port1->sendComm(value);  //Slider的值每次改变都通过串口下发相应亮度值
}

void Widget::on_btnOff_clicked()
{
    ui->labCamera->setText("摄像头未开启");
    timer->stop(); //关闭串口释放资源
    vid.release();
    ui->btnOff->setEnabled(false);
    ui->btnOn->setEnabled(true);
    ui->btnStarRec->setEnabled(false);
    ui->btnStopRec->setEnabled(false);
    ui->btnScreenshots->setEnabled(false);
    ui->LEDSlider->setEnabled(false);
    ui->spBright->setEnabled(false);
    ui->spContrast->setEnabled(false);
    ui->spSaturation->setEnabled(false);
    ui->spChroma->setEnabled(false);
    ui->cbWhiteBalance->setEnabled(false);
    ui->btnImgMag->setEnabled(false);
    ui->btnImgRed->setEnabled(false);
}

void Widget::on_btnScreenshots_clicked()
{
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    QString str = ui->edtImagePath->text();
    QString systimeDate = QString(str).append("/").append(systime.toString("yyyyMMddhhmmss")).append(".png"); //拼接保存路径
    string path = systimeDate.toStdString();
    dbPage->imageName = systime.toString("yyyyMMddhhmmss");
    dbPage->imagePath = ui->edtImagePath->text();
    frame = img.ImageCV(frame);
    frame.convertTo(frame,CV_8UC3,255.0);
    cv::imwrite(path,frame); //保存图片
    dbPage->saveImage();
    QMessageBox *box = new QMessageBox(QMessageBox::Information,tr("提示"),tr("已截图"));
    QTimer::singleShot(1500,box,SLOT(accept()));
    box->exec();
}

void Widget::on_btnStarRec_clicked()
{
    ui->labCamera->setText("正在录制");
    videoIsStart = false;
    mainThread->start();
    myThread->openCamera();
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    QString str = ui->edtVideoPath->text();
    QString systimeDate = QString(str).append("/").append(systime.toString("yyyyMMddhhmmss")).append(".mp4"); //拼接保存路径
    //string path = systimeDate.toStdString();
    dbPage->videoName = systime.toString("yyyyMMddhhmmss");
    dbPage->videoPath = ui->edtImagePath->text();
    ui->btnStarRec->setEnabled(false);
    ui->btnStopRec->setEnabled(true);
    ui->btnOff->setEnabled(false);
    dbPage->startTime = QString(systime.toString("yyyy-MM-dd-hh:mm:ss"));
    ui->labCamImg->setStyleSheet("background-image: url(:/camreaOn.png)");
    QMessageBox *box = new QMessageBox(QMessageBox::Information,tr("提示"),tr("开始录制"));
    QTimer::singleShot(1500,box,SLOT(accept()));
    box->exec();
    dbPage->saveVideoStart();

    myThread->ThreadStart(systimeDate);

}

void Widget::on_btnStopRec_clicked()
{
    ui->btnStarRec->setEnabled(true);
    ui->btnStopRec->setEnabled(false);
    ui->btnOff->setEnabled(true);
    ui->labCamera->setText("摄像头已开启");
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    dbPage->endTime = QString(systime.toString("yyyy-MM-dd-hh:mm:ss"));
    dbPage->saveVideoEnd();
    myThread->ThreadStop();
    mainThread->quit();
    mainThread->wait();
    videoIsStart = true;
    cv::destroyAllWindows();
    QMessageBox *box = new QMessageBox(QMessageBox::Information,tr("提示"),tr("结束录制"));
    ui->labCamImg->setStyleSheet("background-image: url(:/cameraOff.png)");
    QTimer::singleShot(1500,box,SLOT(accept()));
    box->exec();
}

void Widget::on_sliderBright_valueChanged(int value)
{
    beta = float(value);
    ui->spBright->setValue(value);
}

void Widget::on_sliderContrast_valueChanged(int value)
{
    alpha = float(value);
    ui->spContrast->setValue(value);
}

void Widget::on_sliderSaturation_valueChanged(int value)
{
    saturation = value;
    ui->spSaturation->setValue(value);
}

void Widget::on_sliderChroma_valueChanged(int value)
{
    hue = value;
    ui->spChroma->setValue(value);
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
        getAttend();
        getAssistant();
        ui->comAssistant->setEditText("");
        ui->comAttend->setEditText("");
        ui->comAssistant_2->setEditText("");
        ui->comAttend_2->setEditText("");
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
    if (ui->edtAge->text().toInt()<=0 or ui->edtAge->text().toInt()>150){
        QMessageBox warnBox(QMessageBox::Warning,"警告","请输入正确年龄",QMessageBox::Yes);
        warnBox.exec();
        return;
    }
    dbPage->setChartnum(ui->edtChart->text());
    dbPage->setSerialnum(ui->edtSerial->text());
    dbPage->setBunknum(ui->edtBunk->text());
    dbPage->setName(ui->edtName->text());
    dbPage->setGender(ui->cbGender->currentText());
    dbPage->setAge(ui->edtAge->text().toInt());
    dbPage->setAttendName(ui->comAttend->currentText());
    dbPage->setAssistantName(ui->comAssistant->currentText());
    dbPage->setRemark(ui->textEdit->toPlainText());
    dbPage->saveData();
    ui->btnOn->setEnabled(true);
    port1->sendTime(QDateTime::currentDateTime().toString("yyyyMMddhhmm"));
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
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->show();
}

void Widget::on_btnSelectVideo_clicked()
{
    int total = 0;
    int totalPage = 0;
    dbPage->nowPage = 1;
    videoOrImg = 1;

    if(ui->selectChart->text()!="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //病历号
        total = dbPage->selectVideo(ui->selectChart->text(),"Chart",videoOrImg);
    }
    if(ui->selectSerial->text()!="" && ui->selectChart->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //流水号
        total = dbPage->selectVideo(ui->selectSerial->text(),"Serial",videoOrImg);
    }
    if(ui->selectBunk->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //床位号
        total = dbPage->selectVideo(ui->selectBunk->text(),"Bunk",videoOrImg);
    }
    if(ui->selectName->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //姓名
        total = dbPage->selectVideo(ui->selectName->text(),"Name",videoOrImg);
    }
    if(ui->selectRemark->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()==""){
        total = dbPage->selectVideo(ui->selectRemark->text(),"Remark",videoOrImg);
    }
    if(ui->comAttend_2->currentText()!="" && ui->comAssistant_2->currentText()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){
        total = dbPage->selectVideo(ui->comAttend_2->currentText(),ui->comAssistant_2->currentText(),"doctor",videoOrImg);
    }
    if(ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()!="" && ui->selectAgeMin->text()!="" && ui->selectAgeMax->text()!=""){  //病历号

        total = dbPage->selectVideo(ui->selectName->text(),ui->selectAgeMin->text().toInt(),ui->selectAgeMax->text().toInt(),"patien",videoOrImg);
    }
    if(total%20>0){
        totalPage = total/20+1;
    }else{
        totalPage = total/20;
    }
    dbPage->totalPage = totalPage;
    QString str = QString("共%1页").arg(QString::number(totalPage));
    ui->labTotalPage->setText(str);
     str = QString("共%1条").arg(QString::number(total));
    if(totalPage==1)
    {
        ui->btnNext->setEnabled(false);
        ui->btnPrevious->setEnabled(false);
    }else{
        ui->btnNext->setEnabled(true);

    }
    ui->labTotal->setText(str);
    ui->tableView->setColumnWidth(0,100);
    ui->tableView->setColumnWidth(1,100);
    ui->tableView->setColumnWidth(2,50);
    ui->tableView->setColumnWidth(3,50);
    ui->tableView->setColumnWidth(4,100);
    ui->tableView->setColumnWidth(5,100);
    ui->tableView->setColumnWidth(6,100);
    ui->tableView->setColumnWidth(7,100);
    ui->tableView->setColumnWidth(8,100);
    ui->tableView->setColumnWidth(9,150);
    ui->tableView->setColumnWidth(10,150);
    ui->tableView->setColumnWidth(11,150);
    ui->tableView->setColumnWidth(12,150);

    ui->btnFirst->setEnabled(true);
    ui->btnLast->setEnabled(true);
    ui->labNowPage->setVisible(true);
    ui->labTotal->setVisible(true);
    ui->labTotalPage->setVisible(true);
}

void Widget::on_choiceImagePath_clicked()
{
    QString curPath = QCoreApplication::applicationDirPath();
    QString dlgTitle= "选择一个目录";
    QString choiceDir = QFileDialog::getExistingDirectory(this,dlgTitle,curPath,QFileDialog::ShowDirsOnly);
    ui->edtImagePath->setText(choiceDir);
}

void Widget::on_choiceVideoPath_clicked()
{
    QString curPath = QCoreApplication::applicationDirPath();
    QString dlgTitle= "选择一个目录";
    QString choiceDir = QFileDialog::getExistingDirectory(this,dlgTitle,curPath,QFileDialog::ShowDirsOnly);
    ui->edtVideoPath->setText(choiceDir);
}

void Widget::on_btnSystemSave_clicked()
{
    dbPage->saveSystem(ui->edtImagePath->text(),ui->edtVideoPath->text(),ui->slider_Bright->value(),ui->slider_Contrast->value(),ui->slider_Chroma->value(),ui->slider_Saturation->value(),ui->CbWhite_Balance->isChecked(),ui->cbIfFrame->isChecked());
    dbPage->showSystem();
    ui->edtImagePath->setText(dbPage->imagePath);
    ui->edtVideoPath->setText(dbPage->videoPath);
    ui->slider_Bright->setValue(dbPage->bright);
    ui->slider_Contrast->setValue(dbPage->contrast);
    ui->slider_Chroma->setValue(dbPage->chroma);
    ui->slider_Saturation->setValue(dbPage->saturation);
    ui->CbWhite_Balance->setChecked(dbPage->whitebalance);
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
        ui->btnMenu_Max->setStyleSheet("border-image: url(:/Maximize.png)");
    } else {
        location = this->geometry();
        this->setGeometry(this->getScreenRect(true));
        ui->btnMenu_Max->setStyleSheet("border-image: url(:/Middlemize.png)");
    }
    max = !max;
}

void Widget::on_btnMenu_Min_clicked()
{
    setWindowState(Qt::WindowMinimized);
}

void Widget::on_btnPortSearch_clicked()
{
    QStringList serialportinfo;
    foreach(QSerialPortInfo info,QSerialPortInfo::availablePorts())
        {
            serialportinfo<<info.portName();
        }
        ui->comPort->clear();
        ui->comPort->addItems(serialportinfo);
}

void Widget::on_btnOpenPort_clicked()
{

    if(!port1->openCom(ui->comPort->currentText()))   //打开串口
    {
        QMessageBox warnBox(QMessageBox::Warning,"警告","串口打开失败",QMessageBox::Yes);
        warnBox.exec();
    }else{
            portOpen = ui->comPort->currentText();
            timerDetect->start(50);
            QMessageBox warnBox(QMessageBox::Information,"提示","串口打开成功",QMessageBox::Yes);
            warnBox.exec();
            readTime();
    }
}

void Widget::on_btnReadSerial_clicked()
{
    readSerial();
}

//拖拽操作
void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = true;
        //获得鼠标的初始位置
        mouseStartPoint = event->globalPos();
        //mouseStartPoint = event->pos();
        //获得窗口的初始位置
        windowTopLeftPoint = this->frameGeometry().topLeft();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bDrag)
    {
        //获得鼠标移动的距离
        QPoint distance = event->globalPos() - mouseStartPoint;
        //QPoint distance = event->pos() - mouseStartPoint;
        //改变窗口的位置
        this->move(windowTopLeftPoint + distance);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}


void Widget::on_selectAgeMin_textChanged(const QString &arg1)
{
    if(ui->selectAgeMax->text()!=""){
        if((ui->selectAgeMin->text().toInt()) > (ui->selectAgeMax->text().toInt())){
            QMessageBox warnBox(QMessageBox::Information,"提示","年龄填写错误",QMessageBox::Yes);
            warnBox.exec();
        }
    }
}



void Widget::on_selectAgeMax_textChanged(const QString &arg1)
{
    if(ui->selectAgeMin->text()!=""){
        if((ui->selectAgeMin->text().toInt()) > (ui->selectAgeMax->text().toInt())){
            QMessageBox warnBox(QMessageBox::Information,"提示","年龄填写错误",QMessageBox::Yes);
            warnBox.exec();
        }
    }
}

void Widget::on_btnSelectImg_clicked()
{
    videoOrImg = 0;
    if(ui->selectChart->text()!=""){  //搜病历号
        dbPage->selectImg(ui->selectChart->text(),"Chart",videoOrImg);
    }
    if(ui->selectSerial->text()!=""){ //流水号
        dbPage->selectImg(ui->selectSerial->text(),"Serial",videoOrImg);
    }
    if(ui->selectBunk->text()!=""){  //床位号
        dbPage->selectImg(ui->selectBunk->text(),"Bunk",videoOrImg);
    }
    if(ui->selectName->text()!=""){ //姓名
        dbPage->selectImg(ui->selectName->text(),"Name",videoOrImg);
    }
    if(ui->selectRemark->text()!=""){
        dbPage->selectImg(ui->selectRemark->text(),"Remark",videoOrImg);
    }

    ui->tableView->setColumnWidth(0,100);
    ui->tableView->setColumnWidth(1,100);
    ui->tableView->setColumnWidth(2,50);
    ui->tableView->setColumnWidth(3,50);
    ui->tableView->setColumnWidth(4,100);
    ui->tableView->setColumnWidth(5,100);
    ui->tableView->setColumnWidth(6,100);
    ui->tableView->setColumnWidth(7,100);
    ui->tableView->setColumnWidth(8,100);
    ui->tableView->setColumnWidth(9,150);
    ui->tableView->setColumnWidth(10,150);
    ui->tableView->setColumnWidth(11,150);
    ui->tableView->setColumnWidth(12,150);
}



void Widget::on_btnClean_clicked()
{
    ui->edtChart->setText("");
    ui->edtBunk->setText("");
    ui->edtName->setText("");
    ui->edtAge->setValue(1);
    ui->textEdit->setText("");
}

void Widget::on_btnAdd_clicked()
{
    ui->edtSerial->setText(autoSerial());
    ui->edtChart->setText("");
    ui->edtBunk->setText("");
    ui->edtName->setText("");
    ui->edtAge->setValue(1);
    ui->textEdit->setText("");
}

void Widget::on_btnNext_clicked()
{
    int total;
    dbPage->nowPage +=1;
    QString str = QString("第%1页").arg(QString::number(dbPage->nowPage));
    ui->labNowPage->setText(str);
    if(dbPage->nowPage > 1 ){
        ui->btnPrevious->setEnabled(true);
    }
    if(ui->selectChart->text()!="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //病历号
        total = dbPage->selectVideo(ui->selectChart->text(),"Chart",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectSerial->text()!="" && ui->selectChart->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //流水号
        total = dbPage->selectVideo(ui->selectSerial->text(),"Serial",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectBunk->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //床位号
        total = dbPage->selectVideo(ui->selectBunk->text(),"Bunk",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectName->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //姓名
        total = dbPage->selectVideo(ui->selectName->text(),"Name",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectRemark->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()==""){
        total = dbPage->selectVideo(ui->selectRemark->text(),"Remark",videoOrImg,dbPage->nowPage);
    }
    if(ui->comAttend_2->currentText()!="" && ui->comAssistant_2->currentText()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){
        total = dbPage->selectVideo(ui->comAttend_2->currentText(),ui->comAssistant_2->currentText(),"doctor",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()!="" && ui->selectAgeMin->text()!="" && ui->selectAgeMax->text()!=""){  //病历号

        total = dbPage->selectVideo(ui->selectName->text(),ui->selectAgeMin->text().toInt(),ui->selectAgeMax->text().toInt(),"patien",videoOrImg,dbPage->nowPage);
    }
    if(dbPage->nowPage == dbPage->totalPage){
        ui->btnNext->setEnabled(false);
    }
}

void Widget::on_btnPrevious_clicked()
{
    int total;
    dbPage->nowPage -=1;
    QString str = QString("第%1页").arg(QString::number(dbPage->nowPage));
    ui->labNowPage->setText(str);

    if(ui->selectChart->text()!="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //病历号
        total = dbPage->selectVideo(ui->selectChart->text(),"Chart",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectSerial->text()!="" && ui->selectChart->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //流水号
        total = dbPage->selectVideo(ui->selectSerial->text(),"Serial",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectBunk->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //床位号
        total = dbPage->selectVideo(ui->selectBunk->text(),"Bunk",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectName->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //姓名
        total = dbPage->selectVideo(ui->selectName->text(),"Name",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectRemark->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()==""){
        total = dbPage->selectVideo(ui->selectRemark->text(),"Remark",videoOrImg,dbPage->nowPage);
    }
    if(ui->comAttend_2->currentText()!="" && ui->comAssistant_2->currentText()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){
        total = dbPage->selectVideo(ui->comAttend_2->currentText(),ui->comAssistant_2->currentText(),"doctor",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()!="" && ui->selectAgeMin->text()!="" && ui->selectAgeMax->text()!=""){  //病历号

        total = dbPage->selectVideo(ui->selectName->text(),ui->selectAgeMin->text().toInt(),ui->selectAgeMax->text().toInt(),"patien",videoOrImg,dbPage->nowPage);
    }

    if(dbPage->nowPage == 1 ){
        ui->btnPrevious->setEnabled(false);
        ui->btnNext->setEnabled(true);
    }
}

void Widget::on_btnFirst_clicked()
{
    int total;
    dbPage->nowPage = 1;
    QString str = QString("第%1页").arg(QString::number(dbPage->nowPage));
    ui->labNowPage->setText(str);

    if(ui->selectChart->text()!="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //病历号
        total = dbPage->selectVideo(ui->selectChart->text(),"Chart",videoOrImg);
    }
    if(ui->selectSerial->text()!="" && ui->selectChart->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //流水号
        total = dbPage->selectVideo(ui->selectSerial->text(),"Serial",videoOrImg);
    }
    if(ui->selectBunk->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //床位号
        total = dbPage->selectVideo(ui->selectBunk->text(),"Bunk",videoOrImg);
    }
    if(ui->selectName->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //姓名
        total = dbPage->selectVideo(ui->selectName->text(),"Name",videoOrImg);
    }
    if(ui->selectRemark->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()==""){
        total = dbPage->selectVideo(ui->selectRemark->text(),"Remark",videoOrImg);
    }
    if(ui->comAttend_2->currentText()!="" && ui->comAssistant_2->currentText()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){
        total = dbPage->selectVideo(ui->comAttend_2->currentText(),ui->comAssistant_2->currentText(),"doctor",videoOrImg);
    }
    if(ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()!="" && ui->selectAgeMin->text()!="" && ui->selectAgeMax->text()!=""){  //病历号

        total = dbPage->selectVideo(ui->selectName->text(),ui->selectAgeMin->text().toInt(),ui->selectAgeMax->text().toInt(),"patien",videoOrImg);
    }

    if(dbPage->totalPage ==1 ){
        ui->btnPrevious->setEnabled(false);
        ui->btnNext->setEnabled(false);
    }else{
        ui->btnPrevious->setEnabled(false);
        ui->btnNext->setEnabled(true);
    }
}

void Widget::on_btnLast_clicked()
{
    int total;
    dbPage->nowPage = dbPage->totalPage;
    QString str = QString("第%1页").arg(QString::number(dbPage->nowPage));
    ui->labNowPage->setText(str);

    if(ui->selectChart->text()!="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //病历号
        total = dbPage->selectVideo(ui->selectChart->text(),"Chart",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectSerial->text()!="" && ui->selectChart->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //流水号
        total = dbPage->selectVideo(ui->selectSerial->text(),"Serial",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectBunk->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){  //床位号
        total = dbPage->selectVideo(ui->selectBunk->text(),"Bunk",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectName->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){ //姓名
        total = dbPage->selectVideo(ui->selectName->text(),"Name",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectRemark->text()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()==""){
        total = dbPage->selectVideo(ui->selectRemark->text(),"Remark",videoOrImg,dbPage->nowPage);
    }
    if(ui->comAttend_2->currentText()!="" && ui->comAssistant_2->currentText()!="" && ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()=="" && ui->selectAgeMin->text()=="" && ui->selectAgeMax->text()=="" && ui->selectRemark->text()==""){
        total = dbPage->selectVideo(ui->comAttend_2->currentText(),ui->comAssistant_2->currentText(),"doctor",videoOrImg,dbPage->nowPage);
    }
    if(ui->selectChart->text()=="" && ui->selectSerial->text()=="" && ui->selectBunk->text()=="" && ui->selectName->text()!="" && ui->selectAgeMin->text()!="" && ui->selectAgeMax->text()!=""){  //病历号

        total = dbPage->selectVideo(ui->selectName->text(),ui->selectAgeMin->text().toInt(),ui->selectAgeMax->text().toInt(),"patien",videoOrImg,dbPage->nowPage);
    }
    if(dbPage->totalPage ==1 ){
        ui->btnPrevious->setEnabled(false);
        ui->btnNext->setEnabled(false);
    }else{
        ui->btnPrevious->setEnabled(true);
        ui->btnNext->setEnabled(false);
    }
}

void Widget::on_btnLeft_clicked()
{
    revolve-=90;
    if(revolve==-360){
        revolve = 0;
    }
    switch (revolve) {
    case 90:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDownGery);
        ui->labLeft->setPixmap(imgLeftGery);
        ui->labRight->setPixmap(imgRight);
        break;
    case 180:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDown);
        ui->labLeft->setPixmap(imgLeftGery);
        ui->labRight->setPixmap(imgRightGery);
        break;
     case 270:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDownGery);
        ui->labLeft->setPixmap(imgLeft);
        ui->labRight->setPixmap(imgRightGery);
        break;
    case -90:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDownGery);
        ui->labLeft->setPixmap(imgLeft);
        ui->labRight->setPixmap(imgRightGery);
        break;
    case -180:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDown);
        ui->labLeft->setPixmap(imgLeftGery);
        ui->labRight->setPixmap(imgRightGery);
        break;
     case -270:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDownGery);
        ui->labLeft->setPixmap(imgLeftGery);
        ui->labRight->setPixmap(imgRight);
        break;
    case 0:
       ui->labUp->setPixmap(imgUp);
       ui->labDown->setPixmap(imgDownGery);
       ui->labLeft->setPixmap(imgLeftGery);
       ui->labRight->setPixmap(imgRightGery);
       break;
   }
}

void Widget::on_btnRight_clicked()
{
    revolve+=90;
    if(revolve==360){
        revolve = 0;
    }
    switch (revolve) {
    case 90:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDownGery);
        ui->labLeft->setPixmap(imgLeftGery);
        ui->labRight->setPixmap(imgRight);
        break;
    case 180:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDown);
        ui->labLeft->setPixmap(imgLeftGery);
        ui->labRight->setPixmap(imgRightGery);
        break;
     case 270:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDownGery);
        ui->labLeft->setPixmap(imgLeft);
        ui->labRight->setPixmap(imgRightGery);
        break;
    case -90:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDownGery);
        ui->labLeft->setPixmap(imgLeft);
        ui->labRight->setPixmap(imgRightGery);
        break;
    case -180:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDown);
        ui->labLeft->setPixmap(imgLeftGery);
        ui->labRight->setPixmap(imgRightGery);
        break;
     case -270:
        ui->labUp->setPixmap(imgUpGery);
        ui->labDown->setPixmap(imgDownGery);
        ui->labLeft->setPixmap(imgLeftGery);
        ui->labRight->setPixmap(imgRight);
        break;
    case 0:
       ui->labUp->setPixmap(imgUp);
       ui->labDown->setPixmap(imgDownGery);
       ui->labLeft->setPixmap(imgLeftGery);
       ui->labRight->setPixmap(imgRightGery);
       break;
   }
}



void Widget::on_spBright_valueChanged(int arg1)
{
    ui->sliderBright->setValue(arg1);
}

void Widget::on_spContrast_valueChanged(int arg1)
{
    ui->sliderContrast->setValue(arg1);
}

void Widget::on_spSaturation_valueChanged(int arg1)
{
    ui->sliderSaturation->setValue(arg1);
}

void Widget::on_spChroma_valueChanged(int arg1)
{
    ui->sliderChroma->setValue(arg1);
}


void Widget::on_slider_Bright_valueChanged(int value)
{
    ui->sp_Bright->setValue(value);
}

void Widget::on_slider_Contrast_valueChanged(int value)
{
    ui->sp_Contrast->setValue(value);
}

void Widget::on_slider_Saturation_valueChanged(int value)
{
    ui->sp_Saturation->setValue(value);
}

void Widget::on_slider_Chroma_valueChanged(int value)
{
    ui->sp_Chroma->setValue(value);
}

void Widget::on_sp_Bright_valueChanged(int arg1)
{
    ui->slider_Bright->setValue(arg1);
}

void Widget::on_sp_Contrast_valueChanged(int arg1)
{
    ui->slider_Contrast->setValue(arg1);
}

void Widget::on_sp_Saturation_valueChanged(int arg1)
{
    ui->slider_Saturation->setValue(arg1);
}

void Widget::on_sp_Chroma_valueChanged(int arg1)
{
    ui->sliderChroma->setValue(arg1);
}

void Widget::on_cbIfFrame_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        ui->label->setFrameStyle(QFrame::Box);
    }else{
        ui->label->setFrameStyle(QFrame::NoFrame);
    }
}

void Widget::on_btnImgMag_clicked()
{
    if(imgSize>5){
        ui->btnImgMag->setEnabled(false);
        return;
    }else{
        ui->btnImgRed->setEnabled(true);
    }
    imgSize = imgSize+1;
    switch(imgSize){
        case 1:
            ui->label->setMinimumSize(440,450);
            ui->label->setMaximumSize(440,450);
            break;
        case 2:
            ui->label->setMinimumSize(480,500);
            ui->label->setMaximumSize(480,500);
            break;
        case 3:
            ui->label->setMinimumSize(520,540);
            ui->label->setMaximumSize(520,540);
            break;
        case 4:
            ui->label->setMinimumSize(560,575);
            ui->label->setMaximumSize(560,575);
            break;
        case 5:
            ui->label->setMinimumSize(600,600);
            ui->label->setMaximumSize(600,600);
            ui->btnImgMag->setEnabled(false);
            break;
    }



}

void Widget::on_btnImgRed_clicked()
{
    imgSize = imgSize-1;
    if(imgSize<0){
        ui->btnImgRed->setEnabled(false);
        return;
    }else{
        ui->btnImgMag->setEnabled(true);
    }

    switch(imgSize){
        case 0:
            ui->label->setMinimumSize(400,400);
            ui->label->setMaximumSize(400,400);
            ui->btnImgRed->setEnabled(false);
            break;
        case 1:
            ui->label->setMinimumSize(440,450);
            ui->label->setMaximumSize(440,450);
            break;
        case 2:
            ui->label->setMinimumSize(480,500);
            ui->label->setMaximumSize(480,500);
            break;
        case 3:
            ui->label->setMinimumSize(520,540);
            ui->label->setMaximumSize(520,540);
            break;
        case 4:
            ui->label->setMinimumSize(560,575);
            ui->label->setMaximumSize(560,575);
            break;
        case 5:
            ui->label->setMinimumSize(600,600);
            ui->label->setMaximumSize(600,600);
            break;
    }

}

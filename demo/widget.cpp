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
    
    QPixmap img = QPixmap(":/title.jpg");    //设置标题图片
    QSize PixSize = ui->labelTitle->size();
    img.scaled(PixSize,Qt::KeepAspectRatioByExpanding);
    ui->labelTitle->setScaledContents(true);
    ui->labelTitle->setPixmap(img);

    timerPort = new QTimer(this);
    realTime = new QTimer(this);

    connect(realTime,SIGNAL(timeout()),this,SLOT(showTime()));
    realTime->start(1);
    mainThread = new QThread(this);          //视频录制线程
    myThread = new VideoThread;
    myThread->moveToThread(mainThread);

    childPortThread = new QThread(this);     //串口线程
    port1 = new Port();
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
    ui->label->setStyleSheet("QLabel{background-color:rgb(22,39,53);}");
    QSize icoSize(32, 32);
    int icoWidth = 85;

    ui->edtChart->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->edtSerial->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->edtBunk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));

    ui->selectSerial->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->selectChart->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->selectBunk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{25}")));
    ui->selectAgeMin->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->selectAgeMax->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->cbGender->addItem("男");
    ui->cbGender->addItem("女");
    ui->cbGender_2->addItem("男");
    ui->cbGender_2->addItem("女");
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


    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(showVideoReplay(QModelIndex)));
    loadStyle();           //设置全局样式
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

}

void Widget::readTime()
{
    QString data = "54 3A 3F 0D";
    QByteArray byteData = QByteArray::fromHex(data.toLatin1());
    port1->m_serialPort->write(byteData);

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
   //cv::cvtColor(frame,frame1,CV_BGR2RGB);   //不处理的话颜色会偏蓝

   if(ui->CbWhiteBalance->isChecked()){
       frame1 = img.ImageWhitebalance(frame);
   }else{
       frame1 =frame;
   }

   frame2 = img.ImageBright(frame1,beta/10,alpha*10); //调整亮度和对比度函数
   frame3 = img.ImageSaturation(frame2,saturation);    //调整饱和度函数

   frame4 = img.ImageCV(frame3);
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

void Widget::showVideoReplay(const QModelIndex &index) //传入表格中，被双击行的index
{ 
    QSqlRecord record = dbPage->qmodel->record(index.row()); //获取该行的所有字段信息
    if(videoOrImg ==1){
        QString strName = record.value(9).toString(); //获取视频开始时间字段信息
        strName = strName.remove(4,1).remove(6,1).remove(8,1).remove(10,1).remove(12,1);
        QString strPath = record.value(11).toString();
        videoplay = new VideoPlay;
        videoThread = new QThread(this);
        videoplay->setWindowModality(Qt::ApplicationModal);
        videoplay->show();
        videoplay->getVideoName(strName,strPath);
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
    vid.open(0);        //打开摄像头
    if(vid.isOpened())
    {
    timer->start(33);   //摄像头每秒30帧

    ui->btnOn->setEnabled(false);
    ui->btnOff->setEnabled(true);
    ui->btnScreenshots->setEnabled(true);
    ui->btnStarRec->setEnabled(true);
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
    vid.release();
    ui->btnOff->setEnabled(false);
    ui->btnOn->setEnabled(true);
    ui->btnStarRec->setEnabled(false);
    ui->btnStopRec->setEnabled(false);
    ui->btnScreenshots->setEnabled(false);
}

void Widget::on_btnScreenshots_clicked()
{
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    QString str = ui->edtImagePath->text();
    QString systimeDate = QString(str).append(systime.toString("yyyyMMddhhmmss")).append(".png"); //拼接保存路径
    string path = systimeDate.toStdString();
    dbPage->imageName = systime.toString("yyyyMMddhhmmss");
    dbPage->imagePath = ui->edtImagePath->text();
    frame = img.ImageCV(frame);
    frame.convertTo(frame,CV_8UC3,255.0);
    cv::imwrite(path,frame); //保存图片
    dbPage->saveImage();
}

void Widget::on_btnStarRec_clicked()
{
    mainThread->start();
    myThread->openCamera();
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    QString str = ui->edtVideoPath->text();
    QString systimeDate = QString(str).append(systime.toString("yyyyMMddhhmmss")).append(".mp4"); //拼接保存路径
    //string path = systimeDate.toStdString();
    dbPage->videoName = systime.toString("yyyyMMddhhmmss");
    dbPage->videoPath = ui->edtImagePath->text();
    ui->btnStarRec->setEnabled(false);
    ui->btnStopRec->setEnabled(true);
    dbPage->startTime = QString(systime.toString("yyyy-MM-dd-hh:mm:ss"));
    dbPage->saveVideoStart();
    myThread->ThreadStart(systimeDate);
    videoIsStart = false;
}

void Widget::on_btnStopRec_clicked()
{
    ui->btnStarRec->setEnabled(true);
    ui->btnStopRec->setEnabled(false);
    QDateTime systime = QDateTime::currentDateTime(); //获得当前系统时间
    dbPage->endTime = QString(systime.toString("yyyy-MM-dd-hh:mm:ss"));
    dbPage->saveVideoEnd();
    myThread->ThreadStop();
    mainThread->quit();
    mainThread->wait();
    videoIsStart = true;
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
        getAttend();
        getAssistant();
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
    videoOrImg = 1;
    if(ui->selectChart->text()!=""){  //搜病历号
        dbPage->selectVideo(ui->selectChart->text(),"Chart",videoOrImg);
    }
    if(ui->selectSerial->text()!=""){ //流水号
        dbPage->selectVideo(ui->selectSerial->text(),"Serial",videoOrImg);
    }
    if(ui->selectBunk->text()!=""){  //床位号
        dbPage->selectVideo(ui->selectBunk->text(),"Bunk",videoOrImg);
    }
    if(ui->selectName->text()!=""){ //姓名
        dbPage->selectVideo(ui->selectName->text(),"Name",videoOrImg);
    }
    if(ui->selectRemark->text()!=""){
        dbPage->selectVideo(ui->selectRemark->text(),"Remark",videoOrImg);
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
    dbPage->saveSystem(ui->edtImagePath->text(),ui->edtVideoPath->text(),ui->slider_Bright->value(),ui->slider_Contrast->value(),ui->slider_Chroma->value(),ui->slider_Saturation->value());
    dbPage->showSystem();
    ui->edtImagePath->setText(dbPage->imagePath);
    ui->edtVideoPath->setText(dbPage->videoPath);
    ui->slider_Bright->setValue(dbPage->bright);
    ui->slider_Contrast->setValue(dbPage->contrast);
    ui->slider_Chroma->setValue(dbPage->chroma);
    ui->slider_Saturation->setValue(dbPage->saturation);
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



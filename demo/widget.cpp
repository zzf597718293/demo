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

    //QUIHelper::setFramelessForm(this);

    //IconHelper::setIcon(ui->labelTitle, 0xf073, 30);
    //IconHelper::setIcon(ui->btnMenu_Min, 0xf068);
    //IconHelper::setIcon(ui->btnMenu_Max, 0xf067);
    //IconHelper::setIcon(ui->btnMenu_Close, 0xf00d);

    setWindowFlags(Qt::FramelessWindowHint); //设置无边框
    QPixmap img = QPixmap(":/title.jpg");
    QSize PixSize = ui->labelTitle->size();
    img.scaled(PixSize,Qt::KeepAspectRatioByExpanding);
    ui->labelTitle->setScaledContents(true);
    ui->labelTitle->setPixmap(img);

    mainThread = new QThread;
    myThread = new VedioThread;
    myThread->moveToThread(mainThread);
    dbPage = new DbPage(this);
    dbPage->showSystem();
    ui->edtImagePath->setText(dbPage->imagePath);
    ui->edtVedioPath->setText(dbPage->vedioPath);
    ui->label->setStyleSheet("QLabel{background-color:rgb(22,39,53);}");
    QSize icoSize(32, 32);
    int icoWidth = 85;
    //loadStyle();           //设置全局样式
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

cv::Mat Widget::imgPro(cv::Mat image)
{
    cv::Mat image_gray, image_output, image_transform;   //定义输入图像，灰度图像，输出图像
    cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY); //转换为灰度图
    //cv::imshow("image_gray", image_gray);

        //1、傅里叶变换，image_output为可显示的频谱图，image_transform为傅里叶变换的复数结果
        DFT(image_gray, image_output, image_transform);
        //cv::imshow("image_output", image_output);
        //2、巴特沃斯低通滤波
        cv::Mat planes[] = { cv::Mat_<float>(image_output), cv::Mat::zeros(image_output.size(),CV_32F) };
        cv::split(image_transform, planes);//分离通道，获取实部虚部
        cv::Mat image_transform_real = planes[0];
        cv::Mat image_transform_imag = planes[1];

        int core_x = image_transform_real.rows / 2;//频谱图中心坐标
        int core_y = image_transform_real.cols / 2;
        int r = 60;  //滤波半径
        float h;
        float n = 1.5; //巴特沃斯系数
        float D;  //距离中心距离
        for (int i = 0; i < image_transform_real.rows; i++)
        {
            for (int j = 0; j < image_transform_real.cols; j++)
            {
                D = (i - core_x) * (i - core_x) + (j - core_y) * (j - core_y);
                h = 1/(1+pow((D/(r*r)) , n));
                image_transform_real.at<float>(i, j) = image_transform_real.at<float>(i, j) * h;
                image_transform_imag.at<float>(i, j) = image_transform_imag.at<float>(i, j) * h;

            }
        }
        planes[0] = image_transform_real;
        planes[1] = image_transform_imag;
        cv::Mat image_transform_ilpf;//定义巴特沃斯低通滤波结果
        cv::merge(planes, 2, image_transform_ilpf);

        //3、傅里叶逆变换
        cv::Mat iDft[] = { cv::Mat_<float>(image_output), cv::Mat::zeros(image_output.size(),CV_32F) };
        cv::idft(image_transform_ilpf, image_transform_ilpf);//傅立叶逆变换
        cv::split(image_transform_ilpf, iDft);//分离通道，主要获取0通道
        cv::magnitude(iDft[0], iDft[1], iDft[0]); //计算复数的幅值，保存在iDft[0]
        cv::normalize(iDft[0], iDft[0], 0, 1, cv::NORM_MINMAX);//归一化处理
        //cv::imshow("idft", iDft[0]);
        //cv::waitKey(0);

    return iDft[0];

}

void Widget::DFT(cv::Mat input_image, cv::Mat& output_image, cv::Mat& transform_image)
{
    //1.扩展图像矩阵，为2，3，5的倍数时运算速度快
        int m = cv::getOptimalDFTSize(input_image.rows);
        int n = cv::getOptimalDFTSize(input_image.cols);
        cv::copyMakeBorder(input_image, input_image, 0, m - input_image.rows, 0, n - input_image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

        //2.创建一个双通道矩阵planes，用来储存复数的实部与虚部
        cv::Mat planes[] = { cv::Mat_<float>(input_image), cv::Mat::zeros(input_image.size(), CV_32F) };

        //3.从多个单通道数组中创建一个多通道数组:transform_image。函数Merge将几个数组合并为一个多通道阵列，即输出数组的每个元素将是输入数组元素的级联
        cv::merge(planes, 2, transform_image);

        //4.进行傅立叶变换
        cv::dft(transform_image, transform_image);

        //5.计算复数的幅值，保存在output_image（频谱图）
        cv::split(transform_image, planes); // 将双通道分为两个单通道，一个表示实部，一个表示虚部
        cv::Mat transform_image_real = planes[0];
        cv::Mat transform_image_imag = planes[1];

        cv::magnitude(planes[0], planes[1], output_image); //计算复数的幅值，保存在output_image（频谱图）

        //6.前面得到的频谱图数级过大，不好显示，因此转换
        output_image += cv::Scalar(1);   // 取对数前将所有的像素都加1，防止log0
        cv::log(output_image, output_image);   // 取对数
        cv::normalize(output_image, output_image, 0, 1, cv::NORM_MINMAX); //归一化

        //7.剪切和重分布幅度图像限
        output_image = output_image(cv::Rect(0, 0, output_image.cols & -2, output_image.rows & -2));

        // 重新排列傅里叶图像中的象限，使原点位于图像中心
        int cx = output_image.cols / 2;
        int cy = output_image.rows / 2;
        cv::Mat q0(output_image, cv::Rect(0, 0, cx, cy));   // 左上区域
        cv::Mat q1(output_image, cv::Rect(cx, 0, cx, cy));  // 右上区域
        cv::Mat q2(output_image, cv::Rect(0, cy, cx, cy));  // 左下区域
        cv::Mat q3(output_image, cv::Rect(cx, cy, cx, cy)); // 右下区域

          //交换象限中心化
        cv::Mat tmp;
        q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);//左上与右下进行交换
        q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);//右上与左下进行交换


        cv::Mat q00(transform_image_real, cv::Rect(0, 0, cx, cy));   // 左上区域
        cv::Mat q01(transform_image_real, cv::Rect(cx, 0, cx, cy));  // 右上区域
        cv::Mat q02(transform_image_real, cv::Rect(0, cy, cx, cy));  // 左下区域
        cv::Mat q03(transform_image_real, cv::Rect(cx, cy, cx, cy)); // 右下区域
        q00.copyTo(tmp); q03.copyTo(q00); tmp.copyTo(q03);//左上与右下进行交换
        q01.copyTo(tmp); q02.copyTo(q01); tmp.copyTo(q02);//右上与左下进行交换

        cv::Mat q10(transform_image_imag, cv::Rect(0, 0, cx, cy));   // 左上区域
        cv::Mat q11(transform_image_imag, cv::Rect(cx, 0, cx, cy));  // 右上区域
        cv::Mat q12(transform_image_imag, cv::Rect(0, cy, cx, cy));  // 左下区域
        cv::Mat q13(transform_image_imag, cv::Rect(cx, cy, cx, cy)); // 右下区域
        q10.copyTo(tmp); q13.copyTo(q10); tmp.copyTo(q13);//左上与右下进行交换
        q11.copyTo(tmp); q12.copyTo(q11); tmp.copyTo(q12);//右上与左下进行交换

        planes[0] = transform_image_real;
        planes[1] = transform_image_imag;
        cv::merge(planes, 2, transform_image);//将傅里叶变换结果中心化
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

    port1 = new Port();
    port1->openCom();   //打开串口
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



#include "videoplay.h"
#include "ui_videoplay.h"

VideoPlay::VideoPlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoPlay)
{
    ui->setupUi(this);
    setWindowTitle("视频回放");
    img = new ImageProcess;
    myplayer = new QMediaPlayer(this);
    mywidget = new QVideoWidget(ui->labelVideo);
    mywidget->resize(ui->labelVideo->size());
    myplayer->setVideoOutput(mywidget);
    ui->videoSlider->setRange(0, 0);
    ui->cbGender->addItem("男");
    ui->cbGender->addItem("女");

    QList<QString> attendList;
    attendList = dbPage->showAttend();
    QList<QString>::const_iterator iter;
    ui->comAttend->clear();
    for(iter = attendList.begin(); iter != attendList.end();++iter)
    {
        ui->comAttend->addItem(*iter);
    }
    QList<QString> assistantList;
    assistantList = dbPage->showAssistant();

    ui->comAssistant->clear();

    for(iter = assistantList.begin(); iter != assistantList.end();++iter)
    {
        ui->comAssistant->addItem(*iter);
    }

    dbPage = new DbPage(this);
    connect(myplayer, &QMediaPlayer::positionChanged, this, &VideoPlay::positionChanged);
    connect(myplayer, &QMediaPlayer::durationChanged, this, &VideoPlay::durationChanged);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(this,SIGNAL(sendFrames(int)),ui->videoSlider,SLOT(setValue(int)));
    connect(ui->imageList,&QListWidget::itemClicked,this,&VideoPlay::showImage);
}

VideoPlay::~VideoPlay()
{
    myplayer->stop();
    delete mywidget;
    video.release();
    delete img;
    delete ui;

}

void VideoPlay::getVideoName(QString name, QString path)
{
    //QString str ="D:/trailer.mp4";
   // myplayer->setMedia(QUrl::fromLocalFile(str));
string str=path.toStdString()+name.toStdString()+".mp4";
myplayer->setMedia(QUrl::fromLocalFile(QString::fromStdString(str)));
}

void VideoPlay::getPatienInfo(QString serial, QString chart, QString bunk, QString name, QString gender, int age, QString attend, QString assistant, QString remark)
{
    chartnum = chart;
    ui->edtSerial->setText(serial);
    ui->edtChart->setText(chart);
    ui->edtBunk->setText(bunk);
    ui->edtName->setText(name);
    ui->cbGender->setCurrentText(gender);
    ui->edtAge->setValue(age);
    ui->comAttend->addItem(attend);
    ui->comAssistant->addItem(assistant);
    ui->textEdit->setText(remark);
    dbPage->setChartnum(ui->edtChart->text());
    dbPage->setSerialnum(ui->edtSerial->text());
    initList();
}

void VideoPlay::initList()
{
    imageList.clear();
    ui->imageList->clear();
    imageList = dbPage->getImageList();
    QList<QString>::iterator iter;
    for(iter = imageList.begin(); iter != imageList.end();++iter)
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(*iter);
        item->setCheckState(Qt::Unchecked);
        ui->imageList->addItem(item);
    }
}

void VideoPlay::showImage(QListWidgetItem *item)
{
    QString path = QString(dbPage->getImagePath(item->text()))+(item->text())+(".png");
    QImage img;
    img.load(path);
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void VideoPlay::on_videoSlider_sliderMoved(int position)
{
    setPosition(position);
}

void VideoPlay::on_btnVideoPalay_clicked()
{
    if(isStop){
        ui->btnVideoPalay->setStyleSheet("QPushButton#btnVideoPalay{border-image:url(:/video_stop.png)}");
        isStop = false;
        myplayer->play();
    }else{
        ui->btnVideoPalay->setStyleSheet("QPushButton#btnVideoPalay{border-image:url(:/video_play.png)}");
        isStop = true;
        myplayer->pause();
    }
}
void VideoPlay::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->btnVideoPalay->setStyleSheet("QPushButton#btnVideoPalay{border-image:url(:/video_stop.png)}");
        break;
    default:
        ui->btnVideoPalay->setStyleSheet("QPushButton#btnVideoPalay{border-image:url(:/video_play.png)}");
        break;
    }
}

void VideoPlay::positionChanged(qint64 position)
{
    ui->videoSlider->setValue(position);
}

void VideoPlay::durationChanged(qint64 duration)
{
    ui->videoSlider->setMaximum(duration);
}

void VideoPlay::setPosition(int position)
{
    myplayer->setPosition(position);
}

void VideoPlay::on_btnSave_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::question(this,"提示","是否保存数据？");
    if(result==QMessageBox::Yes){
        dbPage->upDatePatien(chartnum,ui->edtSerial->text(),ui->edtChart->text(),ui->edtBunk->text(),ui->edtName->text(),ui->cbGender->currentText(),ui->edtAge->value(),ui->comAttend->currentText(),ui->comAssistant->currentText(),ui->textEdit->toPlainText());
    }
}

void VideoPlay::on_btnDelete_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::question(this,"提示","是否删除该流水号数据？");
    if(result==QMessageBox::Yes){
    dbPage->deletePatien(ui->edtSerial->text());
    this->close();
    }
}

void VideoPlay::on_btnScreenshots_clicked()
{
    QPoint p = ui->labelVideo->mapToGlobal(QPoint(0,0));
    //qDebug()<<p.x()<<p.y();
    QDesktopWidget *desk = QApplication::desktop();
    QScreen * screen = QGuiApplication::primaryScreen();
    QPixmap p1 = screen->grabWindow(desk->winId(),p.x(),p.y(),400,400);
    ui->label->setPixmap(p1);
    QDateTime systime = QDateTime::currentDateTime();
    QString str = imgPath;
    QString systimeDate = QString(str).append(systime.toString("yyyyMMddhhmmss")).append(".png");
    p1.save(systimeDate);
    dbPage->imageName = systime.toString("yyyyMMddhhmmss");
    dbPage->imagePath = imgPath;
    dbPage->saveImage();
    initList();
}


void VideoPlay::on_btnSelectAll_clicked()
{
    for(int i=0;i<ui->imageList->count();i++){
        ui->imageList->item(i)->setCheckState(Qt::Checked);
    }
}

void VideoPlay::on_btnSelectNone_clicked()
{
    for(int i=0;i<ui->imageList->count();i++){
        ui->imageList->item(i)->setCheckState(Qt::Unchecked);
    }
}

void VideoPlay::on_btnDeleteList_clicked()
{
    QMessageBox::StandardButton result = QMessageBox::question(this,"提示","是否删除已选中的图片？");
    if(result==QMessageBox::Yes){
        for(int i=0;i<ui->imageList->count();i++){
            if(ui->imageList->item(i)->checkState()){
                dbPage->deleteImage(ui->imageList->item(i)->text());
            }
        }
        initList();
    }

}

void VideoPlay::on_btnSaveAs_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(NULL,"Save as",".");
    for(int i=0;i<ui->imageList->count();i++){
        if(ui->imageList->item(i)->checkState()){
            QString path = dbPage->getImagePath(ui->imageList->item(i)->text());
            path = path+ui->imageList->item(i)->text()+".png";
            QString newPath = fileName+"/"+ui->imageList->item(i)->text()+".png";
            QFile::copy(path,newPath);
        }
    }

}

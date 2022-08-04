#include "videoplay.h"
#include "ui_videoplay.h"

VideoPlay::VideoPlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoPlay)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(importFrame()));

}

VideoPlay::~VideoPlay()
{
    delete ui;
}

void VideoPlay::getVideoName(QString name, QString path)
{

 string str ="D:/20220802101128.mp4";
 //string str=path.toStdString()+name.toStdString()+".mp4";
 video.open(str);
 int frames = int(video.get(cv::CAP_PROP_FRAME_COUNT));
 qDebug()<<frames;
 timer->start(33);
}

VideoPlay::importFrame()
{
    video >> frame;
    if(frame.empty()){
     timer->stop();
     video.release();
     return 0;
    }
    frame=img.ImageCV(frame);
    cv::cvtColor(frame,frame,cv::COLOR_BGR2RGB);
    frame.convertTo(frame,CV_8UC3,255.0);
    //cv::cvtColor(frame, frame, CV_BGR2RGB);//only RGB of Qt
    QImage srcQImage =QImage(frame.data,frame.cols,frame.rows,frame.step,QImage::Format_RGB888);
    ui->labelVideo->setPixmap(QPixmap::fromImage(srcQImage));
    ui->labelVideo->resize(srcQImage.size());
    ui->labelVideo->show();

}

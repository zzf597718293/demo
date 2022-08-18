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
    connect(myplayer, &QMediaPlayer::positionChanged, this, &VideoPlay::positionChanged);
    connect(myplayer, &QMediaPlayer::durationChanged, this, &VideoPlay::durationChanged);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(this,SIGNAL(sendFrames(int)),ui->videoSlider,SLOT(setValue(int)));
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

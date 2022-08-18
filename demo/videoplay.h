#ifndef VIDEOPLAY_H
#define VIDEOPLAY_H
#include"imageprocess.h"
#include <QWidget>
#include<iostream>
#include<QMediaPlayer>
#include<QMediaPlaylist>
#include<QVideoWidget>
#include<QTimer>
#include<QDebug>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/video.hpp>
#include<opencv2/imgproc/types_c.h>
#include<opencv2/video/tracking.hpp>

#include<Windows.h>
#include <opencv2/highgui/highgui_c.h>
using namespace std;
namespace Ui {
class VideoPlay;
}

class VideoPlay : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlay(QWidget *parent = nullptr);
    ~VideoPlay();
    void getVideoName(QString,QString);

private:
    Ui::VideoPlay *ui;
    QTimer *timer;
    cv::Mat frame;
    cv::VideoCapture video;
    ImageProcess *img;
    int videoFrames = 0;
    bool isStop = true;
    QMediaPlayer *myplayer;
    QMediaPlaylist *myplayerlist;
    QVideoWidget *mywidget;
private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
    void on_videoSlider_sliderMoved(int position);
    void on_btnVideoPalay_clicked();
signals:
    sendFrames(int);
};

#endif // VIDEOPLAY_H

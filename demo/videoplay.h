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
#include"dbpage.h"
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
    void getPatienInfo(QString,QString,QString,QString,QString,int,QString,QString,QString);
    QString imgPath;
private:
    Ui::VideoPlay *ui;
    DbPage *dbPage;
    QTimer *timer;
    cv::Mat frame;
    cv::VideoCapture video;
    ImageProcess *img;
    int videoFrames = 0;
    bool isStop = true;
    QMediaPlayer *myplayer;
    QMediaPlaylist *myplayerlist;
    QVideoWidget *mywidget;
    QString chartnum;
    QList<QString> imageList;
private slots:
    void initList();
    void showImage(QListWidgetItem*);
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
    void on_videoSlider_sliderMoved(int position);
    void on_btnVideoPalay_clicked();
    void on_btnSave_clicked();

    void on_btnDelete_clicked();

    void on_btnScreenshots_clicked();

    void on_btnSelectAll_clicked();

    void on_btnSelectNone_clicked();

    void on_btnDeleteList_clicked();

    void on_btnSaveAs_clicked();

signals:
    sendFrames(int);
};

#endif // VIDEOPLAY_H

#ifndef VIDEOPLAY_H
#define VIDEOPLAY_H
#include"imageprocess.h"
#include <QWidget>
#include<iostream>
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
    ImageProcess img;
private slots:
    importFrame();
};

#endif // VIDEOPLAY_H

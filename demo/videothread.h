#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H
#include"imageprocess.h"
#include<QThread>
#include<QTimer>
#include<QPushButton>
#include<QLabel>
#include<QPixmap>
#include<QSlider>
#include<QDateTime>
#include<QString>
#include<vector>
#include<Windows.h>
#include<opencv2/core/core.hpp>
#include <opencv2/highgui/highgui_c.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/video.hpp>
#include<opencv2/imgproc/types_c.h>
#include<opencv2/video/tracking.hpp>
using namespace std;
class VideoThread : public QObject
{
    Q_OBJECT
public:
    explicit VideoThread(QObject *parent = 0);

    void openCamera();
    void ThreadStart(QString);
    void ThreadStop();

protected:

private:
    ImageProcess img;
    bool m_stop;
    QString path;
    cv::VideoCapture capture;
    cv::VideoWriter writer;
    cv::Mat frame;
    bool stopFlag=false;
};

#endif // VIDEOTHREAD_H

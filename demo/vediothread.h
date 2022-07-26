#ifndef VEDIOTHREAD_H
#define VEDIOTHREAD_H
#include<QThread>
#include<QTimer>
#include<QPushButton>
#include<QLabel>
#include<QPixmap>
#include<QSlider>
#include<QDateTime>
#include<QString>
#include<vector>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/video.hpp>
#include<opencv2/imgproc/types_c.h>
#include<opencv2/video/tracking.hpp>
using namespace std;
class VedioThread : public QObject
{
    Q_OBJECT
public:
    explicit VedioThread(QObject *parent = 0);

    void openCamera();
    void ThreadStart(QString);
    void ThreadStop();

protected:

private:
    bool m_stop;
    QString path;
    cv::VideoCapture capture;
    cv::VideoWriter writer;
    cv::Mat frame;
    bool stopFlag=false;
};

#endif // VEDIOTHREAD_H

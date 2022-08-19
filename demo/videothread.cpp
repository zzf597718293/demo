#include "videothread.h"

VideoThread::VideoThread(QObject *parent) : QObject(parent)
{
    stopFlag = false;
}
void VideoThread::openCamera()
{
    capture.open(0);
    if(!capture.isOpened())
    {
        return;
    }
}

void VideoThread::ThreadStart(QString path)
{
    m_stop = false;
    cv::Size S = cv::Size((int)capture.get(cv::CAP_PROP_FRAME_WIDTH),(int)capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    writer.open(path.toStdString(),cv::VideoWriter::fourcc('M','P','4','V'),30/4,S);
    while(!m_stop)
    {
        capture >> frame;
        frame = img.ImageCV(frame);
        frame.convertTo(frame,CV_8UC3,255.0);
        writer.write(frame);

        //cv::namedWindow("video", cv::WINDOW_NORMAL);
        cv::imshow("video ", frame);
        cv::waitKey(33);

    }
}

void VideoThread::ThreadStop()
{
    //capture.release();
    writer.release();

    m_stop = true;
}

#include "vediothread.h"

VedioThread::VedioThread(QObject *parent) : QObject(parent)
{
    stopFlag = false;
}
void VedioThread::openCamera()
{
    capture.open(0);
    if(!capture.isOpened())
    {
        return;
    }
}

void VedioThread::ThreadStart(QString path)
{
    int i=0;
    m_stop = false;
    cv::Size S = cv::Size((int)capture.get(cv::CAP_PROP_FRAME_WIDTH),(int)capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    writer.open(path.toStdString(),cv::VideoWriter::fourcc('M','P','4','V'),30,S);
    while(!m_stop)
    {
        capture >> frame;
        writer.write(frame);
        cv::namedWindow("video", cv::WINDOW_NORMAL);
        cv::imshow("video", frame);
        cv::waitKey(0);
    }
}

void VedioThread::ThreadStop()
{
    //capture.release();
    writer.release();

    m_stop = true;
}

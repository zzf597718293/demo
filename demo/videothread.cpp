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
    cv::Mat frame1;
    cv::Mat frame2;
    cv::Mat frame3;
    cv::Mat frame4;
    dbpage.showSystem();
    m_stop = false;
    cv::Size S = cv::Size((int)capture.get(cv::CAP_PROP_FRAME_WIDTH),(int)capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    writer.open(path.toStdString(),cv::VideoWriter::fourcc('M','P','4','V'),30/4,S);
    while(!m_stop)
    {
        capture >> frame;
        if(dbpage.whitebalance){
            frame1 = img.ImageWhitebalance(frame);
        }else{
            frame1 = frame;
        }
        frame2 = img.ImageBright(frame1,float(dbpage.bright)/10,float(dbpage.contrast)); //调整亮度和对比度函数
        frame3 = img.ImageSaturation(frame2,dbpage.saturation);    //调整饱和度函数
        frame4 = img.ImageCV(frame3);
        frame4.convertTo(frame4,CV_8UC3,255.0);
        //writer.write(frame);
        writer<<frame4;

        cvNamedWindow("video",cv::WINDOW_GUI_NORMAL);
        cv::resizeWindow("video",0,0);
        HWND win_handle = FindWindow(0, L"video");
        ShowWindow(win_handle, SW_HIDE);
        SetWindowLong(win_handle, GWL_STYLE, GetWindowLong(win_handle, GWL_EXSTYLE));

        //cv::imshow("video", frame3);

        cv::waitKey(33);
    }

}

void VideoThread::ThreadStop()
{
    //capture.release();
    writer.release();

    m_stop = true;
}

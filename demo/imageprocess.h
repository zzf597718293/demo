#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include<vector>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/video.hpp>
#include<opencv2/imgproc/types_c.h>
#include<opencv2/video/tracking.hpp>
class ImageProcess
{

public:
    ImageProcess();
    cv::Mat ImageBright(cv::Mat,float,float);      //亮度处理
    cv::Mat ImageContrast(cv::Mat&);    //对比度处理
    cv::Mat ImageHue(cv::Mat&);         //色调处理
    cv::Mat ImageSaturation(cv::Mat,int);  //饱和度处理
    cv::Mat ImageCV(cv::Mat);
    cv::Mat ImageWhitebalance(cv::Mat);//白平衡处理
private:
    void My_DFT(cv::Mat , cv::Mat& , cv::Mat&);

};

#endif // IMAGEPROCESS_H


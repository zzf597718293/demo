#include "imageprocess.h"

#define max2(a,b) (a>b?a:b)
#define max3(a,b,c) (a>b?max2(a,c):max2(b,c))
#define min2(a,b) (a<b?a:b)
#define min3(a,b,c) (a<b?min2(a,c):min2(b,c))


ImageProcess::ImageProcess()
{

}

cv::Mat ImageProcess::ImageBright(cv::Mat frame,float alpha,float beta)
{
    cv::Mat temp;
    int height = frame.rows;
    int width = frame.cols;
    temp = cv::Mat::zeros(frame.size(),frame.type());
    for (int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            if(frame.channels() == 3)
            {
                float b = frame.at<cv::Vec3b>(row,col)[0];
                float g = frame.at<cv::Vec3b>(row,col)[1];
                float r = frame.at<cv::Vec3b>(row,col)[2];

                temp.at<cv::Vec3b>(row,col)[0] = cv::saturate_cast<uchar>(b*alpha + beta);
                temp.at<cv::Vec3b>(row,col)[1] = cv::saturate_cast<uchar>(g*alpha + beta);
                temp.at<cv::Vec3b>(row,col)[2] = cv::saturate_cast<uchar>(r*alpha + beta);
            }
        }
    }
    return temp;
}

cv::Mat ImageProcess::ImageHue(cv::Mat &frame)
{

}

cv::Mat ImageProcess::ImageContrast(cv::Mat &frame)
{

}

cv::Mat ImageProcess::ImageSaturation(cv::Mat frame,int percent)
{
    float Increment = percent* 1.0f / 100;
        cv::Mat temp = frame.clone();
        int row = frame.rows;
        int col = frame.cols;
        for (int i = 0; i < row; ++i)
        {
            uchar *t = temp.ptr<uchar>(i);
            uchar *s = frame.ptr<uchar>(i);
            for (int j = 0; j < col; ++j)
            {
                uchar b = s[3 * j];
                uchar g = s[3 * j + 1];
                uchar r = s[3 * j + 2];
                float max = max3(r, g, b);
                float min = min3(r, g, b);
                float delta, value;
                float L, S, alpha;
                delta = (max - min) / 255;
                if (delta == 0)
                    continue;
                value = (max + min) / 255;
                L = value / 2;
                if (L < 0.5)
                    S = delta / value;
                else
                    S = delta / (2 - value);
                if (Increment >= 0)
                {
                    if ((Increment + S) >= 1)
                        alpha = S;
                    else
                        alpha = 1 - Increment;
                    alpha = 1 / alpha - 1;
                    t[3 * j + 2] =static_cast<uchar>( r + (r - L * 255) * alpha);
                    t[3 * j + 1] = static_cast<uchar>(g + (g - L * 255) * alpha);
                    t[3 * j] = static_cast<uchar>(b + (b - L * 255) * alpha);
                }
                else
                {
                    alpha = Increment;
                    t[3 * j + 2] = static_cast<uchar>(L * 255 + (r - L * 255) * (1 + alpha));
                    t[3 * j + 1] = static_cast<uchar>(L * 255 + (g - L * 255) * (1 + alpha));
                    t[3 * j] = static_cast<uchar>(L * 255 + (b - L * 255) * (1 + alpha));
                }
            }
        }
        return temp;


}



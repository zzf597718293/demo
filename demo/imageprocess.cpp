#include "imageprocess.h"

#define max2(a,b) (a>b?a:b)
#define max3(a,b,c) (a>b?max2(a,c):max2(b,c))
#define min2(a,b) (a<b?a:b)
#define min3(a,b,c) (a<b?min2(a,c):min2(b,c))

using namespace std;
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

cv::Mat ImageProcess::ImageCV(cv::Mat image)
{
    vector<cv::Mat> channels;
    cv::Mat image_output,image_transform;
    cv::split(image,channels);
    cv::Mat planes[3];

    for(int i=1;i<=3;i++)
    {
        My_DFT(channels.at(i-1), image_output, image_transform);
        //cv::imshow("image_output", image_output);

        //cv::line(image_transform, cv::Point(0, image_transform.size().height / 2), cv::Point(image_transform.size().width/2-20, image_transform.size().height / 2), cv::Scalar(0, 0, 255),4);
        //cv::line(image_transform, cv::Point(image_transform.size().width/2+20, image_transform.size().height / 2), cv::Point(image_transform.size().width, image_transform.size().height / 2), cv::Scalar(0, 0, 255),4);
        cv::circle(image_transform,cv::Point(100,200),15,cv::Scalar(0, 0, 255),-1,cv::LINE_AA);
        cv::circle(image_transform,cv::Point(300,200),15,cv::Scalar(0, 0, 255),-1,cv::LINE_AA);
        //cv::imshow("22", image_output);
        //3、傅里叶逆变换
        cv::Mat iDft[] = { cv::Mat_<float>(image_transform), cv::Mat::zeros(image_transform.size(),CV_32F) };
        cv::idft(image_transform, image_transform);//傅立叶逆变换
        cv::split(image_transform, iDft);//分离通道，主要获取0通道
        cv::magnitude(iDft[0], iDft[1], iDft[0]); //计算复数的幅值，保存在iDft[0]
        cv::normalize(iDft[0], iDft[0], 0, 1, cv::NORM_MINMAX);//归一化处理

        //cv::imshow("idft", iDft[0]);//显示逆变换图像
        planes[i-1]=iDft[0];
        //暂停，保持图像显示，等待按键结束
    }
        //1、傅里叶变换，image_output为可显示的频谱图，image_transform为傅里叶变换的复数结果

    cv::merge(planes, 3, image_transform);

    //cv::imshow("i12", image_transform);
    //cv::waitKey(0);
    return image_transform;
}

cv::Mat ImageProcess::ImageWhitebalance(cv::Mat frame)
{
    vector<cv::Mat> imageRGB;

        //RGB三通道分离
        cv::split(frame, imageRGB);

        //求原始图像的RGB分量的均值
        double R, G, B;
        B = cv::mean(imageRGB[0])[0];
        G = cv::mean(imageRGB[1])[0];
        R = cv::mean(imageRGB[2])[0];

        //需要调整的RGB分量的增益
        double KR, KG, KB;
        KB = (R + G + B) / (3 * B);
        KG = (R + G + B) / (3 * G);
        KR = (R + G + B) / (3 * R);

        //调整RGB三个通道各自的值
        imageRGB[0] = imageRGB[0] * KB;
        imageRGB[1] = imageRGB[1] * KG;
        imageRGB[2] = imageRGB[2] * KR;

        //RGB三通道图像合并
        cv::merge(imageRGB, frame);
        return frame;
}

void ImageProcess::My_DFT(cv::Mat input_image, cv::Mat& output_image, cv::Mat& transform_image)
{
    //1.扩展图像矩阵，为2，3，5的倍数时运算速度快
        int m = cv::getOptimalDFTSize(input_image.rows);
        int n = cv::getOptimalDFTSize(input_image.cols);
        cv::copyMakeBorder(input_image, input_image, 0, m - input_image.rows, 0, n - input_image.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

        //2.创建一个双通道矩阵planes，用来储存复数的实部与虚部
        cv::Mat planes[] = { cv::Mat_<float>(input_image), cv::Mat::zeros(input_image.size(), CV_32F) };

        //3.从多个单通道数组中创建一个多通道数组:transform_image。函数Merge将几个数组合并为一个多通道阵列，即输出数组的每个元素将是输入数组元素的级联
        cv::merge(planes, 2, transform_image);

        //4.进行傅立叶变换
        cv::dft(transform_image, transform_image);

        //5.计算复数的幅值，保存在output_image（频谱图）
        cv::split(transform_image, planes); // 将双通道分为两个单通道，一个表示实部，一个表示虚部
        cv::Mat transform_image_real = planes[0];
        cv::Mat transform_image_imag = planes[1];

        cv::magnitude(planes[0], planes[1], output_image); //计算复数的幅值，保存在output_image（频谱图）

        //6.前面得到的频谱图数级过大，不好显示，因此转换
        output_image += cv::Scalar(1);   // 取对数前将所有的像素都加1，防止log0
        cv::log(output_image, output_image);   // 取对数
        cv::normalize(output_image, output_image, 0, 1, cv::NORM_MINMAX); //归一化

        //7.剪切和重分布幅度图像限
        output_image = output_image(cv::Rect(0, 0, output_image.cols & -2, output_image.rows & -2));

        // 重新排列傅里叶图像中的象限，使原点位于图像中心
        int cx = output_image.cols / 2;
        int cy = output_image.rows / 2;
        cv::Mat q0(output_image, cv::Rect(0, 0, cx, cy));   // 左上区域
        cv::Mat q1(output_image, cv::Rect(cx, 0, cx, cy));  // 右上区域
        cv::Mat q2(output_image, cv::Rect(0, cy, cx, cy));  // 左下区域
        cv::Mat q3(output_image, cv::Rect(cx, cy, cx, cy)); // 右下区域

          //交换象限中心化
        cv::Mat tmp;
        q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);//左上与右下进行交换
        q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);//右上与左下进行交换


        cv::Mat q00(transform_image_real, cv::Rect(0, 0, cx, cy));   // 左上区域
        cv::Mat q01(transform_image_real, cv::Rect(cx, 0, cx, cy));  // 右上区域
        cv::Mat q02(transform_image_real, cv::Rect(0, cy, cx, cy));  // 左下区域
        cv::Mat q03(transform_image_real, cv::Rect(cx, cy, cx, cy)); // 右下区域
        q00.copyTo(tmp); q03.copyTo(q00); tmp.copyTo(q03);//左上与右下进行交换
        q01.copyTo(tmp); q02.copyTo(q01); tmp.copyTo(q02);//右上与左下进行交换

        cv::Mat q10(transform_image_imag, cv::Rect(0, 0, cx, cy));   // 左上区域
        cv::Mat q11(transform_image_imag, cv::Rect(cx, 0, cx, cy));  // 右上区域
        cv::Mat q12(transform_image_imag, cv::Rect(0, cy, cx, cy));  // 左下区域
        cv::Mat q13(transform_image_imag, cv::Rect(cx, cy, cx, cy)); // 右下区域
        q10.copyTo(tmp); q13.copyTo(q10); tmp.copyTo(q13);//左上与右下进行交换
        q11.copyTo(tmp); q12.copyTo(q11); tmp.copyTo(q12);//右上与左下进行交换

        planes[0] = transform_image_real;
        planes[1] = transform_image_imag;
        cv::merge(planes, 2, transform_image);//将傅里叶变换结果中心化
}



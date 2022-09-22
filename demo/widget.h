#ifndef WIDGET_H
#define WIDGET_H
#include"port.h"
#include"videoplay.h"
#include"ui_videoplay.h"
#include"screenshot.h"
#include"Dbpage.h"
#include"videothread.h"
#include"imageprocess.h"
#include"doctorform.h"
#include <QWidget>
#include<QSerialPort>
#include<QSerialPortInfo>
#include<QThread>
#include<QTimer>
#include<QPushButton>
#include<QLabel>
#include<QPixmap>
#include<QSlider>
#include<QDateTime>
#include<QString>
#include<vector>
#include<QMessageBox>
#include<QDebug>
#include<QStatusBar>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/video/video.hpp>
#include<opencv2/imgproc/types_c.h>
#include<opencv2/video/tracking.hpp>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE
using namespace std;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QRect getScreenRect(bool);
    int getScreenIndex();
    controlEnable(bool);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:

    Ui::Widget *ui;
    QStatusBar *statusBar;
    QLabel *labelSerial;
    QLabel *labelUsedtime;
    int bright = 0;
    float alpha = 0;
    int hue = 1;
    float beta = 1;
    int saturation = 0;
    int imgSize = 0;
    cv::VideoCapture vid;
    cv::Mat frame;
    cv::Mat frame1;
    ImageProcess img;
    QTimer *timer;
    QTimer *timerPort;
    QTimer *realTime;
    QTimer *timerDetect;
    QTimer *timerSysTime;
    Port *port1;
    QString portOpen = "";
    VideoPlay *videoplay;
    Screenshot *screenshot;
    VideoThread *myThread;
    QThread *mainThread;
    QThread *childPortThread;
    QThread *videoThread;
    QList<QString> columnNames; //字段名集合
    QList<int> columnWidths;    //字段宽度集合
    DbPage *dbPage;
    bool videoIsStart = true;
    bool m_bDrag;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
    int videoOrImg;  //1:v 0:i
    int revolve = 0;
    QPixmap imgUpGery =QPixmap(":/upGery.png");
    QPixmap imgDownGery =QPixmap(":/downGery.png");
    QPixmap imgLeftGery =QPixmap(":/leftGery.png");
    QPixmap imgRightGery =QPixmap(":/rightGery.png");
    QPixmap imgUp =QPixmap(":/up.png");
    QPixmap imgDown =QPixmap(":/down.png");
    QPixmap imgLeft =QPixmap(":/left.png");
    QPixmap imgRight =QPixmap(":/right.png");
    enum Direction
    {
        IMG_UP = 0,
        IMG_DOWN = 1,
        IMG_LEFT = 2,
        IMG_RIGET = 3
    };

    void init();
    void loadStyle();
    void getAttend();
    void getAssistant();
    void readSerial();
    void readTime();

    QString autoSerial(); //自动生成流水号
public slots:
    void videoOpen();
    void showTime();
private slots:
    void autoDetect();
    void showVideoReplay(const QModelIndex &);
    void handleToCamera(QString);
    void on_btnOn_clicked();
    void timer_out();
    void on_LEDSlider_valueChanged(int value);
    void on_btnOff_clicked();

    void on_btnScreenshots_clicked();
    void on_btnStarRec_clicked();
    void on_btnStopRec_clicked();

    void on_sliderBright_valueChanged(int value);
    void on_sliderContrast_valueChanged(int value);
    void on_sliderSaturation_valueChanged(int value);
    void on_sliderChroma_valueChanged(int value);
    void buttonClick();
    void on_btnMenu_Close_clicked();

    void on_btnSave_clicked();
    void on_btnConfig_clicked();
    void on_btnData_clicked();

    void on_btnSelectVideo_clicked();
    void on_choiceImagePath_clicked();
    void on_choiceVideoPath_clicked();
    void on_btnSystemSave_clicked();
    void on_btnAttend_clicked();
    void on_btnAssistant_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Min_clicked();

    void on_btnPortSearch_clicked();

    void on_btnOpenPort_clicked();
    void on_btnReadSerial_clicked();
    void on_selectAgeMin_textChanged(const QString &arg1);

    void on_selectAgeMax_textChanged(const QString &arg1);
    void on_btnSelectImg_clicked();

    void on_btnClean_clicked();
    void on_btnAdd_clicked();
    void on_btnNext_clicked();
    void on_btnPrevious_clicked();
    void on_btnFirst_clicked();
    void on_btnLast_clicked();
    void on_btnLeft_clicked();
    void on_btnRight_clicked();

    void on_spBright_valueChanged(int arg1);
    void on_spContrast_valueChanged(int arg1);
    void on_spSaturation_valueChanged(int arg1);
    void on_spChroma_valueChanged(int arg1);

    void on_slider_Bright_valueChanged(int value);
    void on_slider_Contrast_valueChanged(int value);
    void on_slider_Saturation_valueChanged(int value);
    void on_slider_Chroma_valueChanged(int value);
    void on_sp_Bright_valueChanged(int arg1);
    void on_sp_Contrast_valueChanged(int arg1);
    void on_sp_Saturation_valueChanged(int arg1);
    void on_sp_Chroma_valueChanged(int arg1);
    void on_cbIfFrame_stateChanged(int arg1);
    void on_btnImgMag_clicked();
    void on_btnImgRed_clicked();
};


#endif // WIDGET_H

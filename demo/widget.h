#ifndef WIDGET_H
#define WIDGET_H
#include"port.h"
#include"videoplay.h"
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
    float alpha;
    float beta =1;
    int saturation;
    cv::VideoCapture vid;
    cv::Mat frame;
    cv::Mat frame1;
    ImageProcess img;
    QTimer *timer;
    QTimer *timerPort;
    QTimer *realTime;
    Port *port1;
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

    void showVideoReplay(const QModelIndex &);
    void handleToCamera(QString);
    void on_btnOn_clicked();
    void timer_out();
    void on_horizontalSlider_valueChanged(int value);
    void on_btnOff_clicked();

    void on_btnScreenshots_clicked();
    void on_btnStarRec_clicked();
    void on_btnStopRec_clicked();

    void on_sliderBright_valueChanged(int value);
    void on_sliderContrast_valueChanged(int value);
    void on_sliderSaturation_valueChanged(int value);
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

    void on_btnClear_clicked();
    void on_btnAdd_clicked();
};


#endif // WIDGET_H

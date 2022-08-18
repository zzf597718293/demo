#include "screenshot.h"
#include "ui_screenshot.h"

Screenshot::Screenshot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Screenshot)
{
    ui->setupUi(this);
    setWindowTitle("查看截图");
}

Screenshot::~Screenshot()
{
    delete ui;
}

void Screenshot::getImgPatn(QString name, QString path)
{
    string str=path.toStdString()+name.toStdString()+".png";
    QImage *img = new QImage;
    if(!(img->load(QString::fromStdString(str)))){
        QMessageBox::information(this,
                                 tr("打开图像失败"),
                                 tr("打开图像失败!"));
        delete img;
        return;
    }
    ui->label->setPixmap(QPixmap::fromImage(*img));
}

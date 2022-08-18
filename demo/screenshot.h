#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QWidget>
#include<iostream>
#include<QMessageBox>
using namespace std;
namespace Ui {
class Screenshot;
}

class Screenshot : public QWidget
{
    Q_OBJECT

public:
    explicit Screenshot(QWidget *parent = nullptr);
    ~Screenshot();
void getImgPatn(QString, QString);
private:
    Ui::Screenshot *ui;
};

#endif // SCREENSHOT_H

#include "widget.h"
#include "unistd.h"
#include"port.h"
#include <QApplication>
#include <QMovie>
#include <QSplashScreen>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QPixmap pixmap(":/logo.gif");
        QSplashScreen splash(pixmap);
        QLabel label(&splash);
        QMovie mv(":/logo.gif");
        label.setMovie(&mv);
        mv.start();
        splash.show();
        //splash.setCursor(Qt::BlankCursor);
        for(int i=0; i<9000; i+=mv.speed())
        {
            QCoreApplication::processEvents();
            usleep(500*static_cast<useconds_t>(mv.speed()));
        }
    Widget w;
    w.show();
    //w.showMaximized();
    splash.finish(&w);
    return a.exec();
}

QT       += core gui serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customcontrols.cpp \
    dbpage.cpp \
    doctorform.cpp \
    imageprocess.cpp \
    main.cpp \
    port.cpp \
    videoplay.cpp \
    videothread.cpp \
    widget.cpp

HEADERS += \
    customcontrols.h \
    dbpage.h \
    doctorform.h \
    head.h \
    imageprocess.h \
    port.h \
    videoplay.h \
    videothread.h \
    widget.h
    head.h

FORMS += \
    doctorform.ui \
    videoplay.ui \
    widget.ui

#RESOURCES   += $$PWD/../core_qss/qss.qrc

#INCLUDEPATH += $$PWD
#INCLUDEPATH += $$PWD/form
#include ($$PWD/form/form.pri)

#INCLUDEPATH += $$PWD/../core_common
#include ($$PWD/../core_common/core_common.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += C:\opencv\opencv-build\install\include
LIBS += C:\opencv\opencv-build\lib\libopencv_*.a

RESOURCES += \
    image.qrc \
    style.qrc

DISTFILES +=

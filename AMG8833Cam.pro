# Copyright(C) 2019-2022 Andrey Yaromenok, ayaromenok@gmail.com
# MIT License
# https://github.com/ayaromenok/AMG8833Cam/blob/master/LICENSE

QT       += core gui widgets
#QT       += multimedia multimediawidgets
QT       += serialport

TARGET = AGM8833Cam

TEMPLATE = app

CONFIG += c++11

SOURCES += \
        src/main.cpp \        
        src/widget.cpp

HEADERS += \        
        src/widget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#RESOURCES += \
#    res.qrc



CVLINUX = /usr
#Darwin, Windows, Android, iOS
equals(QMAKE_HOST.os, Linux) {
    message("        Settings for Unix/Linux.")
    INCLUDEPATH += $${CVLINUX}/include/opencv4
    LIBS += -L$${CVLINUX}/lib
    LIBS += -lopencv_core -lopencv_imgcodecs -lopencv_imgproc
    LIBS += -lopencv_highgui -lopencv_videoio    

    equals(QMAKE_HOST.arch, x86_64){
        DEFINES += DEBUG_PC
        message("        arch: amd64")
    }
    contains(QMAKE_HOST.arch, armv7l){
        message("        arch: RPi/armv7l")
        DEFINES += DEBUG_RPI_V7L
    }
    contains(QMAKE_HOST.arch, armv6l){
        message("        arch: RPi/armv6l")
        DEFINES += DEBUG_RPI_V6L
    }
    contains(QMAKE_HOST.arch, aarch64){
        message("        arch: RPi3/aarch64")
        DEFINES += DEBUG_RPI_ARM64
    }
}

RESOURCES += \
    res/resources.qrc

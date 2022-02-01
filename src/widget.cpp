#include "widget.h"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <QtWidgets>
//#include <QtMultimedia>
//#include <QtMultimediaWidgets>
#include <QSerialPort>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget{parent}
{
#ifdef DEBUG_PC
    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    setGeometry(100,100,640,320);
    setCamCv();
    setCamIr();
    setUI();

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    _timer->start(1000); //msec. min is 100 - 10fps - maximum for AGM8833, but 1000ms for debug for now

}
Widget::~Widget(){
#ifdef DEBUG_PC
    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    _timer->stop();
}

void
Widget::setUI(){
#ifdef DEBUG_PC
    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    _loutMain = new QHBoxLayout;
    _imgCamIR = new QImage(":/8x8.png");
    qDebug()<< "image" << _imgCamIR->width() <<"x" << _imgCamIR->height();
    _lbCamIR = new QLabel("IR image");
    _lbCamIR->setPixmap(QPixmap::fromImage(*_imgCamIR).scaled(240, 240));
    _loutMain->addWidget(_lbCamIR);

    _imgCamCV = new QImage(":/640x480.png");
    _lbCamCV = new QLabel("CV image");
    _lbCamCV->setPixmap(QPixmap::fromImage(*_imgCamCV).scaled(320, 240));
    _loutMain->addWidget(_lbCamCV);
    setLayout(_loutMain);
}

void
Widget::setCamCv(){
#ifdef DEBUG_PC
    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    _cvCap = new cv::VideoCapture();
    _cvCap->open(0); // cv::CAP_V4L);
    _cvCap->set(cv::CAP_PROP_FRAME_WIDTH, 320);
    _cvCap->set(cv::CAP_PROP_FRAME_HEIGHT, 240);
    //cvCap.set(cv::CAP_PROP_FPS, 30);

    if (!_cvCap->isOpened()){
        qErrnoWarning("CV camera capture error");
        return;
    } else {
        _frame = new cv::Mat(320,240, CV_8UC3);
    }
    *_cvCap >> *_frame;
}

void
Widget::setCamIr(){
#ifdef DEBUG_PC
    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    _tty = new QSerialPort(this);
    _tty->setPortName("/dev/ttyUSB0");
    _tty->setBaudRate(9600);
    if (!_tty->open(QIODevice::ReadOnly)) {
        qErrnoWarning("Serial port opening error");
        return;
    }
}

void
Widget::timerUpdate(){
#ifdef DEBUG_PC
//    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    *_cvCap >> *_frame;
    if (0 == _frame->cols){
        qErrnoWarning("CV Camera FRAME capture error");
    } else {
        //qInfo() << "frame: "<< _frame->cols <<"x"<< _frame->rows;
        camCvUpdate();
    }
    camIrUpdate();
}
void
Widget::camCvUpdate(){
#ifdef DEBUG_PC
//    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    cv::Mat imgTmp;
    cv::cvtColor(*_frame, imgTmp, cv::COLOR_RGB2GRAY);
    QImage imageOut(imgTmp.cols, imgTmp.rows,  QImage::Format_RGB888);
    cv::Mat imageCvOut(cv::Size(imgTmp.cols,imgTmp.rows),
                       CV_8UC3, imageOut.bits());

    cv::cvtColor(imgTmp, imageCvOut, cv::COLOR_BGR2RGB);

    _lbCamCV->setPixmap(QPixmap::fromImage(imageOut.scaledToWidth(320)));
}
void
Widget::camIrUpdate(){
#ifdef DEBUG_PC
//    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    QByteArray readData = _tty->readAll();
    qInfo() << readData.length();
    if(130 == readData.length()) {
        cv::Mat imageIr(cv::Size(8,8), CV_16UC1, readData.data());
        cv::Mat imgTmp, imgTmp8, imgTmpCm;
        //cv::imwrite("8x8.png",imageIr);//OK
        cv::resize(imageIr,imgTmp, cv::Size(240,240), cv::INTER_LINEAR);
        //cv::imwrite("240x240.png",imgTmp);//OK
        imgTmp.convertTo(imgTmp8, CV_8U,1/256.0);
        //cv::imwrite("240x240x8.png",imgTmp8);//OK
        cv::applyColorMap(imgTmp8,imgTmpCm,cv::COLORMAP_JET);
        //cv::imwrite("240x240cm.png",imgTmpCm);//OK, need sometuning

        QImage imageQOut(imgTmp8.cols, imgTmp8.rows,  QImage::Format_RGB888);
        cv::Mat imageIrOut(cv::Size(imgTmp8.cols,imgTmp8.rows),
                           CV_8UC3, imageQOut.bits());

        cv::cvtColor(imgTmpCm, imageIrOut, cv::COLOR_BGR2RGB);

        _lbCamIR->setPixmap(QPixmap::fromImage(imageQOut.scaledToWidth(240)));
    }
}

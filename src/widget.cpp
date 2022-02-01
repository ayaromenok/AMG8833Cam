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
    _fMin=18.0f;
    _fMax=-28.0f;
    _fScale=10.0f;
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

    _loutCtl = new QVBoxLayout();
    _edMin = new QLineEdit(QString::number(_fMin));
    _edMax = new QLineEdit(QString::number(_fMax));
    _edScale = new QLineEdit(QString::number(_fScale));
    _loutCtl->addWidget(_edScale);
    _loutCtl->addWidget(_edMax);
    _loutCtl->addWidget(_edMin);
    _loutMain->addItem(_loutCtl);
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
    _tty->setBaudRate(115200);
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
    cv::Mat imgTmp, imgTmpCm;
    QByteArray imgData;
    float fData[64];
    float fTmp=0.0;

    QByteArray readData = _tty->readAll();

    if(386 == readData.length()) {
        QList<QByteArray> arr = readData.split(' ');
        //qDebug() << arr.length();
        for (int i=0; i<64;i++){
            fTmp = arr.at(i).toFloat();
            fData[i] = fTmp;
            if (fTmp>_fMax){ _fMax=fTmp; }
            if (fTmp<_fMin){ _fMin=fTmp; }

        }//for
        _fScale=255.0/(_fMax-_fMin);
        qDebug() << "min: " << _fMin <<", max:"<< _fMax << " ,scale" << _fScale;
        _edScale->setText(QString::number(_fScale));
        _edMax->setText(QString::number(_fMax));
        _edMin->setText(QString::number(_fMin));


        for (int i=0; i<64;i++){
            fTmp = (fData[i]-_fMin)*_fScale;
            imgData.append((int) fTmp);
        }//for


        cv::Mat imageIr(cv::Size(8,8), CV_8U, imgData.data());
        cv::flip(imageIr, imageIr, 0);

        cv::resize(imageIr,imgTmp, cv::Size(240,240), cv::INTER_LINEAR);
        cv::applyColorMap(imgTmp,imgTmpCm,cv::COLORMAP_JET);


        QImage imageQOut(imgTmp.cols, imgTmp.rows,  QImage::Format_RGB888);
        cv::Mat imageIrOut(cv::Size(imgTmp.cols,imgTmp.rows),
                           CV_8UC3, imageQOut.bits());

        cv::cvtColor(imgTmpCm, imageIrOut, cv::COLOR_BGR2RGB); //copy to Qt image
        _lbCamIR->setPixmap(QPixmap::fromImage(imageQOut.scaledToWidth(240)));

        //_fMin=287.0f;
        //_fMax=-287.0f;
    } else {
        qInfo() << "wrong read: "<<readData.length() << " bytes";
    }
}




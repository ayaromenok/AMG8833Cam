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
    setGeometry(100,100,1024,320);
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

    _imgCamCam = new QImage(":/640x480.png");
    _lbCamCam = new QLabel("Cam image");
    _lbCamCam->setPixmap(QPixmap::fromImage(*_imgCamCam).scaled(320, 240));
    _loutMain->addWidget(_lbCamCam);

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
        camCamUpdate();
        camCvUpdate();
    }
}

void
Widget::camCamUpdate(){
#ifdef DEBUG_PC
//    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    cv::Mat imgTmpCam, imgTmpEdge, imgTmpEdgeRGB, imgTmpMix;
    cv::cvtColor(*_frame, imgTmpCam, cv::COLOR_RGB2GRAY);
    cv::Canny(imgTmpCam, imgTmpEdge, 80,160);
    cv::cvtColor(imgTmpEdge,imgTmpEdgeRGB, cv::COLOR_GRAY2RGB);
    cv::addWeighted(*_frame, 0.8, imgTmpEdgeRGB, 0.4,0.0,imgTmpMix);

    QImage imageOut(imgTmpCam.cols, imgTmpCam.rows,  QImage::Format_RGB888);
    cv::Mat imageCvOut(cv::Size(imgTmpCam.cols,imgTmpCam.rows),
                       CV_8UC3, imageOut.bits());

    cv::cvtColor(imgTmpMix, imageCvOut, cv::COLOR_BGR2RGB);

    _lbCamCam->setPixmap(QPixmap::fromImage(imageOut.scaledToWidth(320)));
}
void
Widget::camCvUpdate(){
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


        cv::Mat imgIr(cv::Size(8,8), CV_8U, imgData.data());
        cv::flip(imgIr, imgIr, 0);
        cv::rotate(imgIr, imgIr,  cv::ROTATE_180);

        cv::resize(imgIr,imgTmp, cv::Size(240,240), cv::INTER_LINEAR);
        cv::applyColorMap(imgTmp,imgTmpCm,cv::COLORMAP_JET);


        QImage imgQOut(imgTmp.cols, imgTmp.rows,  QImage::Format_RGB888);
        cv::Mat imgIrOut(cv::Size(imgTmp.cols,imgTmp.rows),
                           CV_8UC3, imgQOut.bits());

        cv::cvtColor(imgTmpCm, imgIrOut, cv::COLOR_BGR2RGB); //copy to Qt image
        _lbCamIR->setPixmap(QPixmap::fromImage(imgQOut.scaledToWidth(240)));
//cam
        cv::Mat imgTmpCam, imgTmpEdge, imgTmpMix, imgTmpCmResize, imgTmpMixClr;
        cv::cvtColor(*_frame, imgTmpCam, cv::COLOR_RGB2GRAY);
        cv::Canny(imgTmpCam, imgTmpEdge, 80,160);

        //try to fit two camera images
        cv::resize(imgTmpCm,imgTmpCm, cv::Size(360,360), cv::INTER_LINEAR);
        imgTmpCmResize = imgTmpCm(cv::Range(60,300),cv::Range(20,340));
        //cv::copyMakeBorder(imgTmpCm, imgTmpCmResize, 0,0,-40,-40,cv::BORDER_CONSTANT);

        cv::cvtColor(imgTmpEdge, imgTmpMixClr, cv::COLOR_GRAY2RGB);
        cv::addWeighted(imgTmpCmResize, 0.6, imgTmpMixClr, 0.4,0.0, imgTmpMixClr);

        QImage imageOut(imgTmpCam.cols, imgTmpCam.rows,  QImage::Format_RGB888);
        cv::Mat imageCvOut(cv::Size(imgTmpCam.cols,imgTmpCam.rows),
                           CV_8UC3, imageOut.bits());

        cv::cvtColor(imgTmpMixClr, imageCvOut, cv::COLOR_BGR2RGB);

        _lbCamCV->setPixmap(QPixmap::fromImage(imageOut.scaledToWidth(320)));

        //_fMin=287.0f;
        //_fMax=-287.0f;
    } else {
        qInfo() << "wrong read: "<<readData.length() << " bytes";
    }
}




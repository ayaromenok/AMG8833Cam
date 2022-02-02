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
    setGeometry(140,200,1180,320);
    setUI();
    setCamCv();
    setCamIr();


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

    _gbInput = new QGroupBox("Input");
    _loutInput = new QHBoxLayout();
    _loutInputIr = new QVBoxLayout();
    _loutInputCam = new QVBoxLayout();

    _lbCamIrDescr = new QLabel("IR image");
    _imgCamIR = new QImage(":/8x8.png");
    _lbCamIR = new QLabel("IR image");
    _lbCamIR->setPixmap(QPixmap::fromImage(*_imgCamIR).scaled(240, 240));
    _loutInputIr->addWidget(_lbCamIrDescr);
    _loutInputIr->addWidget(_lbCamIR);


    _lbCamCamDescr = new QLabel("Cam image");
    _imgCamCam = new QImage(":/640x480.png");
    _lbCamCam = new QLabel("Cam image");
    _lbCamCam->setPixmap(QPixmap::fromImage(*_imgCamCam).scaled(320, 240));
    _loutInputCam->addWidget(_lbCamCamDescr);
    _loutInputCam->addWidget(_lbCamCam);

    _loutInput->addItem(_loutInputIr);
    _loutInput->addItem(_loutInputCam);
    _gbInput->setLayout(_loutInput);

    _gbRes = new QGroupBox("Result");
    _loutRes = new QVBoxLayout();
    _imgCamCV = new QImage(":/640x480.png");
    _lbCamCV = new QLabel("CV image");
    _lbCamCvDescr= new QLabel("Result image");
    _lbCamCV->setPixmap(QPixmap::fromImage(*_imgCamCV).scaled(320, 240));
    _loutRes->addWidget(_lbCamCvDescr);
    _loutRes->addWidget(_lbCamCV);
    _gbRes->setLayout(_loutRes);

    _loutRight=new QVBoxLayout();
    _gbSave = new QGroupBox("Save");
    _pbSave = new QPushButton("Save to file");
    _loutSave = new QVBoxLayout();
    _loutSave->addWidget(_pbSave);
    _gbSave->setLayout(_loutSave);

    _gbTempRange = new QGroupBox("Temp Range");
    _loutTempRange = new QGridLayout();
    _imgGradient = new QImage(":/20x256gradient.png");
    _lbGradient = new QLabel("Gradient");
    _lbGradient->setPixmap(QPixmap::fromImage(*_imgGradient).scaled(20, 128));
    _lbTempMinDescr = new QLabel("Temp Min/C");
    _lbTempMaxDescr = new QLabel("Temp Max/C");
    _lbTempScaleDescr = new QLabel("Scale");
    _edTempMin = new QLineEdit(QString::number(_fMin));
    _edTempMax = new QLineEdit(QString::number(_fMax));
    _edTempScale = new QLineEdit(QString::number(_fScale));
    _edTempMin->setMaximumWidth(60);
    _edTempMax->setMaximumWidth(60);
    _edTempScale->setMaximumWidth(60);
    _loutTempRange->addWidget(_lbGradient, 0,0,3,1);
    _loutTempRange->addWidget(_edTempMax,   0,1);
    _loutTempRange->addWidget(_edTempScale, 1,1);
    _loutTempRange->addWidget(_edTempMin,   2,1);
    _loutTempRange->addWidget(_lbTempMaxDescr,   0,2);
    _loutTempRange->addWidget(_lbTempScaleDescr, 1,2);
    _loutTempRange->addWidget(_lbTempMinDescr,   2,2);
    _gbTempRange->setLayout(_loutTempRange);

    _gbSettings = new QGroupBox("Settings");
    _loutSettings = new QGridLayout();
    _lbCamPathDescr = new QLabel("Cam:");
    _lbIrPathDescr = new QLabel("IR:");
    _cbCamPath = new QComboBox();
    //!ToDo: need to grab devices from file system
    _cbCamPath->insertItem(0,"/dev/video0");
    _cbCamPath->insertItem(1,"/dev/video1");
    _cbCamPath->insertItem(2,"/dev/video2");
    _cbCamPath->setMinimumWidth(50);
    _cbIrPath = new QComboBox();
    _cbIrPath->insertItem(0,"/dev/ttyUSB0");
    _cbIrPath->insertItem(1,"/dev/ttyUSB1");
    _loutSettings->addWidget(_lbCamPathDescr, 0,0);
    _loutSettings->addWidget(_lbIrPathDescr, 1,0);
    _loutSettings->addWidget(_cbCamPath, 0,1);
    _loutSettings->addWidget(_cbIrPath, 1,1);
    _gbSettings->setLayout(_loutSettings);

    _loutRight->addWidget(_gbSave);
    _loutRight->addWidget(_gbTempRange);
    _loutRight->addWidget(_gbSettings);


    _loutMain->addWidget(_gbInput);
    _loutMain->addWidget(_gbRes);
    _loutMain->addItem(_loutRight);

    setLayout(_loutMain);
}

void
Widget::setCamCv(){
#ifdef DEBUG_PC
    qDebug() << __PRETTY_FUNCTION__;
#endif //DEBUG_PC
    _cvCap = new cv::VideoCapture();
    _cvCap->open(_cbCamPath->currentIndex()); // cv::CAP_V4L);
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
    _tty->setPortName(_cbIrPath->currentText());
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
        _edTempScale->setText(QString::number(_fScale));
        _edTempMax->setText(QString::number(_fMax));
        _edTempMin->setText(QString::number(_fMin));


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




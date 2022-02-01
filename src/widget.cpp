#include "widget.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <QtWidgets>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget{parent}
{
    setGeometry(100,100,640,320);
    setUI();
}
Widget::~Widget(){

}

void
Widget::setUI(){
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
Widget::setCamCV(){

}

void
Widget::setCamIR(){

}

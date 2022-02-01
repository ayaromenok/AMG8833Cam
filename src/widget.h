#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>

class QImage;
class QLabel;
class QSerialPort;

namespace cv{
    class VideoCapture;
    class Mat;
}

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
private slots:
    void    timerUpdate();
    void    camCvUpdate();
    void    camIrUpdate();

private:
    void    setUI();
    void    setCamCv();
    void    setCamIr();

    QLayout     *_loutMain;
    QLabel      *_lbCamCV;
    QLabel      *_lbCamIR;
    QImage      *_imgCamCV;
    QImage      *_imgCamIR;

    QTimer      *_timer;

    cv::VideoCapture        *_cvCap;
    cv::Mat                 *_frame;
    QSerialPort             *_tty;
    QVector<quint8>         data;


};

#endif // WIDGET_H

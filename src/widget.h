#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>

class QImage;
class QLabel;
class QSerialPort;
class QLineEdit;

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
    QLayout     *_loutCtl;
    QLineEdit   *_edMin;
    QLineEdit   *_edMax;
    QLineEdit   *_edScale;

    QTimer      *_timer;

    cv::VideoCapture        *_cvCap;
    cv::Mat                 *_frame;
    QSerialPort             *_tty;
    QVector<quint8>         data;

    float                   _fMin;
    float                   _fMax;
    float                   _fScale;
};

#endif // WIDGET_H

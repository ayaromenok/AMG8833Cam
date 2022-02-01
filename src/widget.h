#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>

class QImage;
class QLabel;

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

private:
    void    setUI();
    void    setCamCV();
    void    setCamIR();

    QLayout     *_loutMain;
    QLabel      *_lbCamCV;
    QLabel      *_lbCamIR;
    QImage      *_imgCamCV;
    QImage      *_imgCamIR;

    QTimer      *_timer;

    cv::VideoCapture        *cvCap;
    cv::Mat                 *frame;
    QVector<quint8>         data;
};

#endif // WIDGET_H

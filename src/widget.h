#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>

class QImage;
class QLabel;
class QSerialPort;
class QLineEdit;
class QGroupBox;

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
    void    camCamUpdate();
    void    camCvUpdate();

private:
    void    setUI();
    void    setCamCv();
    void    setCamIr();

    QLayout     *_loutMain;

    QGroupBox   *_gbInput;
    QGroupBox   *_gbRes;
    QGroupBox   *_gbSave;
    QGroupBox   *_gbTempRange;
    QGroupBox   *_gbSettings;
    QLabel      *_lbCamCvDescr;
    QLabel      *_lbCamIrDescr;
    QLabel      *_lbCamCamDescr;
    QLayout     *_loutInput;
    QLayout     *_loutInputIr;
    QLayout     *_loutInputCam;
    QLayout     *_loutRes;
    QLayout     *_loutRight;
    QLayout     *_loutSave;
    QLayout     *_loutTempRange;
    QLayout     *_loutSettings;

    QLabel      *_lbCamCV;
    QLabel      *_lbCamIR;
    QLabel      *_lbCamCam;
    QImage      *_imgCamCV;
    QImage      *_imgCamIR;
    QImage      *_imgCamCam;

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

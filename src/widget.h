// Copyright(C) 2019-2022 Andrey Yaromenok, ayaromenok@gmail.com
// MIT License
// https://github.com/ayaromenok/AMG8833Cam/blob/master/LICENSE

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVector>

class QImage;
class QLabel;
class QSerialPort;
class QLineEdit;
class QGroupBox;
class QPushButton;
class QGridLayout;
class QComboBox;
class QSettings;
class QCheckBox;

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

    QSettings   *_settings;
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
    QGridLayout *_loutTempRange;
    QGridLayout *_loutSettings;
    QPushButton *_pbSave;

    QLabel      *_lbCamCV;
    QLabel      *_lbCamIR;
    QLabel      *_lbCamCam;
    QImage      *_imgCamCV;
    QImage      *_imgCamIR;
    QImage      *_imgCamCam;

    QLabel      *_lbGradient;
    QImage      *_imgGradient;
    QLabel      *_lbTempMinDescr;
    QLabel      *_lbTempMaxDescr;
    QLabel      *_lbTempScaleDescr;
    QLineEdit   *_edTempMin;
    QLineEdit   *_edTempMax;
    QLineEdit   *_edTempScale;

    QLabel      *_lbCamPathDescr;
    QLabel      *_lbIrPathDescr;
    QComboBox   *_cbCamPath;
    QComboBox   *_cbIrPath;
    QComboBox   *_cbIrSpeed;
    QCheckBox   *_cbDebugUi;

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

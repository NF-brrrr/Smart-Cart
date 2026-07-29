#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>
#include "barcodecapture.h"
#include "barcodedecoder.h"
#include "pccameracapture.h"
#include "esp32camstream.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSourceChanged(int index);
    void onScanBarcodeClicked();
    void onEsp32CaptureReady(const QImage &image);
    void onEsp32CaptureError(const QString &message);
    void onEsp32StreamFrame(const QImage &frame);
    void onEsp32StreamError(const QString &message);
    void onPcImageReady(const QImage &image);
    void onPcError(const QString &message);
    void resumeLiveView();

private:
    enum class CaptureSource { Esp32, Pc };

    void processDecodedImage(const QImage &image);

    QComboBox *sourceCombo;
    QStackedWidget *displayStack;
    QLabel *resultStillLabel;   // index 0: captured still + decode result
    QLabel *esp32LiveLabel;     // index 1: ESP32 live stream (for aiming)
    // PC webcam viewfinder widget goes at index 2 (owned by PcCameraCapture)

    QLabel *resultLabel;
    QPushButton *scanButton;

    BarcodeCapture *m_esp32Capture;
    Esp32CamStream *m_esp32Stream;
    PcCameraCapture *m_pcCapture;
    CaptureSource m_currentSource = CaptureSource::Esp32;
};

#endif // MAINWINDOW_H
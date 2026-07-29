#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include "esp32camstream.h"
#include "barcodecapture.h"
#include "barcodedecoder.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectCameraClicked();
    void onCameraFrameReady(const QImage &frame);
    void onCameraError(const QString &message);
    void onScanBarcodeClicked();
    void onCaptureReady(const QImage &image);
    void onCaptureError(const QString &message);

private:
    QLabel *camLabel;
    QPushButton *connectCameraButton;
    Esp32CamStream *m_camStream;
    QLabel *resultLabel;
    QPushButton *scanButton;
    BarcodeCapture *m_barcodeCapture;
};

#endif // MAINWINDOW_H
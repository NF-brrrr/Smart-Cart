#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include "esp32camstream.h"

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

private:
    QLabel *camLabel;
    QPushButton *connectCameraButton;
    Esp32CamStream *m_camStream;
};

#endif // MAINWINDOW_H
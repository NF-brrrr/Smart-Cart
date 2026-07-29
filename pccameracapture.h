#ifndef PCCAMERACAPTURE_H
#define PCCAMERACAPTURE_H

#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QImageCapture>
#include <QVideoWidget>
#include <QImage>

class PcCameraCapture : public QObject
{
    Q_OBJECT
public:
    explicit PcCameraCapture(QObject *parent = nullptr);

    QVideoWidget *viewfinder() const { return m_viewfinder; }

    void start();
    void stop();
    void captureFrame();

signals:
    void imageReady(const QImage &image);
    void cameraError(const QString &message);

private slots:
    void onImageCaptured(int id, const QImage &image);
    void onCameraErrorOccurred(QCamera::Error error, const QString &errorString);

private:
    QCamera *m_camera;
    QMediaCaptureSession m_session;
    QImageCapture *m_imageCapture;
    QVideoWidget *m_viewfinder;
};

#endif // PCCAMERACAPTURE_H
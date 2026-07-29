#include "pccameracapture.h"
#include <QMediaDevices>
#include <QDebug>

PcCameraCapture::PcCameraCapture(QObject *parent)
    : QObject(parent)
{
    QCameraDevice device = QMediaDevices::defaultVideoInput();
    if (device.isNull()) {
        qWarning() << "No webcam detected on this PC";
    }

    m_camera = new QCamera(device, this);
    m_imageCapture = new QImageCapture(this);
    m_viewfinder = new QVideoWidget();

    m_session.setCamera(m_camera);
    m_session.setImageCapture(m_imageCapture);
    m_session.setVideoOutput(m_viewfinder);

    connect(m_imageCapture, &QImageCapture::imageCaptured,
            this, &PcCameraCapture::onImageCaptured);
    connect(m_camera, &QCamera::errorOccurred,
            this, &PcCameraCapture::onCameraErrorOccurred);
}

void PcCameraCapture::start()
{
    if (!m_camera->isActive()) m_camera->start();
}

void PcCameraCapture::stop()
{
    if (m_camera->isActive()) m_camera->stop();
}

void PcCameraCapture::captureFrame()
{
    if (!m_camera->isActive()) {
        emit cameraError("PC camera is not active");
        return;
    }
    m_imageCapture->capture();
}

void PcCameraCapture::onImageCaptured(int id, const QImage &image)
{
    Q_UNUSED(id);
    emit imageReady(image);
}

void PcCameraCapture::onCameraErrorOccurred(QCamera::Error error, const QString &errorString)
{
    Q_UNUSED(error);
    emit cameraError(errorString);
}
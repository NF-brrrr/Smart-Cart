// barcodecapture.h
#ifndef BARCODECAPTURE_H
#define BARCODECAPTURE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QImage>

class BarcodeCapture : public QObject
{
    Q_OBJECT
public:
    explicit BarcodeCapture(QObject *parent = nullptr);
    void capture(const QUrl &captureUrl);

signals:
    void imageReady(const QImage &image);
    void captureError(const QString &message);

private:
    QNetworkAccessManager m_manager;
};

#endif
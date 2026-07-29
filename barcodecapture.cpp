// barcodecapture.cpp
#include "barcodecapture.h"
#include <QNetworkReply>

BarcodeCapture::BarcodeCapture(QObject *parent) : QObject(parent) {}

void BarcodeCapture::capture(const QUrl &captureUrl)
{
    QNetworkRequest request(captureUrl);
    QNetworkReply *reply = m_manager.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit captureError(reply->errorString());
            return;
        }
        QImage img;
        if (img.loadFromData(reply->readAll(), "JPEG")) {
            emit imageReady(img);
        } else {
            emit captureError("Failed to decode JPEG from ESP32");
        }
    });
}
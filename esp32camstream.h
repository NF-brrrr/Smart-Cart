#ifndef ESP32CAMSTREAM_H
#define ESP32CAMSTREAM_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QImage>
#include <QByteArray>

class Esp32CamStream : public QObject
{
    Q_OBJECT
public:
    explicit Esp32CamStream(QObject *parent = nullptr);
    void start(const QUrl &streamUrl);
    void stop();

signals:
    void frameReady(const QImage &frame);
    void connectionError(const QString &message);

private slots:
    void onReadyRead();
    void onFinished();
    void onErrorOccurred(QNetworkReply::NetworkError code);

private:
    QNetworkAccessManager m_manager;
    QNetworkReply *m_reply = nullptr;
    QByteArray m_buffer;
};

#endif // ESP32CAMSTREAM_H
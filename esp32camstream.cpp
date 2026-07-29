#include "esp32camstream.h"

Esp32CamStream::Esp32CamStream(QObject *parent) : QObject(parent) {}

void Esp32CamStream::start(const QUrl &streamUrl)
{
    if (m_reply) stop();

    QNetworkRequest request(streamUrl);
    m_reply = m_manager.get(request);

    connect(m_reply, &QNetworkReply::readyRead, this, &Esp32CamStream::onReadyRead);
    connect(m_reply, &QNetworkReply::finished, this, &Esp32CamStream::onFinished);
    connect(m_reply, &QNetworkReply::errorOccurred, this, &Esp32CamStream::onErrorOccurred);
}

void Esp32CamStream::stop()
{
    if (m_reply) {
        m_reply->disconnect();
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    m_buffer.clear();
}

void Esp32CamStream::onReadyRead()
{
    m_buffer.append(m_reply->readAll());

    static const QByteArray SOI = QByteArray::fromHex("FFD8"); // JPEG start
    static const QByteArray EOI = QByteArray::fromHex("FFD9"); // JPEG end

    while (true) {
        int start = m_buffer.indexOf(SOI);
        if (start == -1) {
            if (m_buffer.size() > 65536) m_buffer.clear(); // avoid unbounded growth on junk
            break;
        }
        int end = m_buffer.indexOf(EOI, start + 2);
        if (end == -1) {
            if (start > 0) m_buffer.remove(0, start); // drop multipart boundary text before SOI
            break; // frame not fully received yet
        }
        end += 2; // include EOI bytes
        QByteArray jpegData = m_buffer.mid(start, end - start);
        m_buffer.remove(0, end);

        QImage img;
        if (img.loadFromData(jpegData, "JPEG")) {
            emit frameReady(img);
        }
    }
}

void Esp32CamStream::onFinished()
{
    if (m_reply) {
        m_reply->deleteLater();
        m_reply = nullptr;
    }
}

void Esp32CamStream::onErrorOccurred(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    if (m_reply) emit connectionError(m_reply->errorString());
}
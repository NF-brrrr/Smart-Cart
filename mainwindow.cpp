#include "mainwindow.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_esp32Capture(new BarcodeCapture(this))
    , m_esp32Stream(new Esp32CamStream(this))
    , m_pcCapture(new PcCameraCapture(this))
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    sourceCombo = new QComboBox(central);
    sourceCombo->addItem("ESP32 Camera (WiFi)");
    sourceCombo->addItem("PC Webcam");

    displayStack = new QStackedWidget(central);

    resultStillLabel = new QLabel("No photo yet", central);
    resultStillLabel->setAlignment(Qt::AlignCenter);
    resultStillLabel->setMinimumSize(320, 240);
    resultStillLabel->setStyleSheet("background-color: black; color: white;");

    esp32LiveLabel = new QLabel("Connecting to ESP32 stream...", central);
    esp32LiveLabel->setAlignment(Qt::AlignCenter);
    esp32LiveLabel->setMinimumSize(320, 240);
    esp32LiveLabel->setStyleSheet("background-color: black; color: white;");

    m_pcCapture->viewfinder()->setMinimumSize(320, 240);

    displayStack->addWidget(resultStillLabel);          // index 0
    displayStack->addWidget(esp32LiveLabel);             // index 1
    displayStack->addWidget(m_pcCapture->viewfinder());  // index 2

    resultLabel = new QLabel("Scanned code: —", central);
    scanButton = new QPushButton("Scan Barcode", central);

    layout->addWidget(sourceCombo);
    layout->addWidget(displayStack);
    layout->addWidget(resultLabel);
    layout->addWidget(scanButton);
    setCentralWidget(central);
    resize(480, 460);
    setWindowTitle("Smart Cart");

    connect(sourceCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onSourceChanged);
    connect(scanButton, &QPushButton::clicked, this, &MainWindow::onScanBarcodeClicked);

    connect(m_esp32Capture, &BarcodeCapture::imageReady, this, &MainWindow::onEsp32CaptureReady);
    connect(m_esp32Capture, &BarcodeCapture::captureError, this, &MainWindow::onEsp32CaptureError);

    connect(m_esp32Stream, &Esp32CamStream::frameReady, this, &MainWindow::onEsp32StreamFrame);
    connect(m_esp32Stream, &Esp32CamStream::connectionError, this, &MainWindow::onEsp32StreamError);

    connect(m_pcCapture, &PcCameraCapture::imageReady, this, &MainWindow::onPcImageReady);
    connect(m_pcCapture, &PcCameraCapture::cameraError, this, &MainWindow::onPcError);

    // Default: ESP32 source, start its live stream right away for aiming
    m_esp32Stream->start(QUrl("http://192.168.4.1:81/stream"));
    displayStack->setCurrentIndex(1);
}

MainWindow::~MainWindow()
{
    m_esp32Stream->stop();
    m_pcCapture->stop();
}

void MainWindow::onSourceChanged(int index)
{
    m_currentSource = (index == 1) ? CaptureSource::Pc : CaptureSource::Esp32;
    resultLabel->setText("Scanned code: —");

    if (m_currentSource == CaptureSource::Pc) {
        m_esp32Stream->stop();
        m_pcCapture->start();
        displayStack->setCurrentIndex(2);
    } else {
        m_pcCapture->stop();
        m_esp32Stream->start(QUrl("http://192.168.4.1:81/stream"));
        displayStack->setCurrentIndex(1);
    }
}

void MainWindow::onScanBarcodeClicked()
{
    scanButton->setEnabled(false);
    scanButton->setText("Scanning...");

    if (m_currentSource == CaptureSource::Esp32) {
        m_esp32Capture->capture(QUrl("http://192.168.4.1/capture"));
    } else {
        m_pcCapture->captureFrame();
    }
}

void MainWindow::onEsp32StreamFrame(const QImage &frame)
{
    if (m_currentSource == CaptureSource::Esp32 && displayStack->currentIndex() == 1) {
        esp32LiveLabel->setPixmap(QPixmap::fromImage(frame).scaled(
            esp32LiveLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::onEsp32StreamError(const QString &message)
{
    esp32LiveLabel->setText("Stream error: " + message);
}

void MainWindow::onEsp32CaptureReady(const QImage &image)
{
    processDecodedImage(image);
}

void MainWindow::onEsp32CaptureError(const QString &message)
{
    scanButton->setEnabled(true);
    scanButton->setText("Scan Barcode");
    resultLabel->setText("Error: " + message);
}

void MainWindow::onPcImageReady(const QImage &image)
{
    processDecodedImage(image);
    QTimer::singleShot(1500, this, &MainWindow::resumeLiveView);
}

void MainWindow::onPcError(const QString &message)
{
    scanButton->setEnabled(true);
    scanButton->setText("Scan Barcode");
    resultLabel->setText("Error: " + message);
}

void MainWindow::resumeLiveView()
{
    displayStack->setCurrentIndex(m_currentSource == CaptureSource::Esp32 ? 1 : 2);
}

void MainWindow::processDecodedImage(const QImage &image)
{
    scanButton->setEnabled(true);
    scanButton->setText("Scan Barcode");

    displayStack->setCurrentIndex(0);
    resultStillLabel->setPixmap(QPixmap::fromImage(image).scaled(
        resultStillLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QList<DecodedBarcode> codes = decodeBarcodes(image);
    if (codes.isEmpty()) {
        resultLabel->setText("Scanned code: none found — try again");
    } else {
        resultLabel->setText("Scanned code: " + codes.first().data +
                             " (" + codes.first().type + ")");
    }

    if (m_currentSource == CaptureSource::Esp32) {
        QTimer::singleShot(1500, this, &MainWindow::resumeLiveView);
    }
}
#include "mainwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_camStream(new Esp32CamStream(this))
    , m_barcodeCapture(new BarcodeCapture(this))
{
    // Central widget + layout
    QWidget *central = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    QHBoxLayout *labelLayout = new QHBoxLayout();

    camLabel = new QLabel("No camera feed");
    camLabel->setAlignment(Qt::AlignCenter);
    camLabel->setMinimumSize(320, 240);
    camLabel->setStyleSheet("background-color: black; color: white;");

    resultLabel = new QLabel("Scanned code: —");
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setMinimumWidth(200);

    // Put the labels side by side
    labelLayout->addWidget(camLabel, 3);      // Takes more space
    labelLayout->addWidget(resultLabel, 1);   // Takes less space

    connectCameraButton = new QPushButton("Connect to Camera");
    scanButton = new QPushButton("Scan Barcode");

    // Build the main layout
    mainLayout->addLayout(labelLayout);
    mainLayout->addWidget(connectCameraButton);
    mainLayout->addWidget(scanButton);

    setCentralWidget(central);
    setWindowTitle("Smart Cart");

    connect(scanButton, &QPushButton::clicked, this, &MainWindow::onScanBarcodeClicked);
    connect(m_barcodeCapture, &BarcodeCapture::imageReady, this, &MainWindow::onCaptureReady);
    connect(m_barcodeCapture, &BarcodeCapture::captureError, this, &MainWindow::onCaptureError);

    connect(connectCameraButton, &QPushButton::clicked,
            this, &MainWindow::onConnectCameraClicked);

    connect(m_camStream, &Esp32CamStream::frameReady,
            this, &MainWindow::onCameraFrameReady);

    connect(m_camStream, &Esp32CamStream::connectionError,
            this, &MainWindow::onCameraError);
}

MainWindow::~MainWindow()
{
    m_camStream->stop();
}

void MainWindow::onConnectCameraClicked()
{
    m_camStream->start(QUrl("http://192.168.4.1:81/stream"));
    connectCameraButton->setEnabled(false);
    connectCameraButton->setText("Connecting...");
}

void MainWindow::onCameraFrameReady(const QImage &frame)
{
    connectCameraButton->setText("Connected");
    camLabel->setPixmap(QPixmap::fromImage(frame).scaled(
        camLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::onCameraError(const QString &message)
{
    qWarning() << "Camera stream error:" << message;
    connectCameraButton->setEnabled(true);
    connectCameraButton->setText("Connect to Camera");
}

void MainWindow::onScanBarcodeClicked()
{
    scanButton->setEnabled(false);
    scanButton->setText("Scanning...");
    m_barcodeCapture->capture(QUrl("http://192.168.4.1/capture"));
}

void MainWindow::onCaptureReady(const QImage &image)
{
    scanButton->setEnabled(true);
    scanButton->setText("Scan Barcode");

    camLabel->setPixmap(QPixmap::fromImage(image).scaled(
        camLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QList<DecodedBarcode> codes = decodeBarcodes(image);
    if (codes.isEmpty()) {
        resultLabel->setText("Scanned code: none found — try again");
    } else {
        resultLabel->setText("Scanned code: " + codes.first().data +
                             " (" + codes.first().type + ")");
    }
}

void MainWindow::onCaptureError(const QString &message)
{
    scanButton->setEnabled(true);
    scanButton->setText("Scan Barcode");
    resultLabel->setText("Error: " + message);
}
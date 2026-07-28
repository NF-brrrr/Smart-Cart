#include "mainwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_camStream(new Esp32CamStream(this))
{
    // Central widget + layout
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    camLabel = new QLabel("No camera feed", central);
    camLabel->setAlignment(Qt::AlignCenter);
    camLabel->setMinimumSize(320, 240);
    camLabel->setStyleSheet("background-color: black; color: white;");

    connectCameraButton = new QPushButton("Connect to Camera", central);

    layout->addWidget(camLabel);
    layout->addWidget(connectCameraButton);
    central->setLayout(layout);

    setCentralWidget(central);
    resize(480, 400);
    setWindowTitle("Smart Cart");

    // Signals
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
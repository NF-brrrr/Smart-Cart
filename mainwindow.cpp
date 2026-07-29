#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QTimer>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_esp32Capture(new BarcodeCapture(this))
    , m_esp32Stream(new Esp32CamStream(this))
    , m_pcCapture(new PcCameraCapture(this))
{
    ui->setupUi(this);

    setWindowTitle("Smart Cart");
    resize(1280, 800);
    this->setStyleSheet("QMainWindow {background-color : #070D19; }");

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    sidebarWidget = new QWidget(centralWidget);
    sidebarWidget->setFixedWidth(260);

    sidebarWidget->setStyleSheet(
        "QWidget { background-color: #0B1224; }"
        "QPushButton { color: white; border: none; text-align: left; padding-left: 10px; background: transparent; height: 45px; font-size: 14px; }"
        "QPushButton:checked { background-color: blue; border-radius: 5px; font-weight: bold; }"
        );

    dashboardWidget = new QWidget(centralWidget);
    dashboardWidget->setStyleSheet("background-color: #070D19;");

    contentStack = new QStackedWidget(centralWidget);
    contentStack->addWidget(dashboardWidget); // page 0: Dashboard

    mainLayout->addWidget(sidebarWidget);
    mainLayout->addWidget(contentStack);

    barMenu = new QVBoxLayout(sidebarWidget);
    barMenu->setContentsMargins(20, 30, 20, 20);
    barMenu->setSpacing(15);

    titleLabel = new QLabel(sidebarWidget);
    titleLabel->setText("<span style='color: #4EA2E4; font-weight: bold; font-size: 16px;'>SMART</span><br>"
                        "<span style='color: #2D63C8; font-weight: bold; font-size: 25px;'>C A R T</span>");

    barMenu->addWidget(titleLabel);
    barMenu->addSpacing(20);

    QPushButton *btnAjoutproduit = new QPushButton("Ajouter du produit", sidebarWidget);
    btnAjoutproduit->setStyleSheet(
        "QPushButton { color: #4EA2E4; }"
        "QPushButton:checked { color: #FFFFFF; }"
        );
    btnAjoutproduit->setCheckable(true);
    barMenu->addWidget(btnAjoutproduit);

    QPushButton *btnLancercaddie = new QPushButton("Lancer Caddie",sidebarWidget);
    btnLancercaddie->setStyleSheet(
        "QPushButton { color: #4EA2E4; }"
        "QPushButton:checked { color: #FFFFFF; }"
        );
    btnLancercaddie->setCheckable(true);
    barMenu->addWidget(btnLancercaddie);

    QPushButton *btnScannerCodeBar = new QPushButton("Scanner CodeBar", sidebarWidget);
    btnScannerCodeBar->setStyleSheet(
        "QPushButton { color: #4EA2E4; }"
        "QPushButton:checked { color: #FFFFFF; }"
        );
    btnScannerCodeBar->setCheckable(true);
    barMenu->addWidget(btnScannerCodeBar);

    QButtonGroup *sidebarButtonGroup = new QButtonGroup(this);
    sidebarButtonGroup->setExclusive(true);
    sidebarButtonGroup->addButton(btnAjoutproduit);
    sidebarButtonGroup->addButton(btnLancercaddie);
    sidebarButtonGroup->addButton(btnScannerCodeBar);

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    barMenu->addItem(spacer);

    statusWidget = new QWidget(sidebarWidget);
    statusWidget->setFixedHeight(80);
    statusWidget->setStyleSheet("background-color: red; border-radius: 8px;");

    statusLayout = new QVBoxLayout(statusWidget);
    statusLayout->setContentsMargins(15, 10, 15, 10);

    btnConnection = new QPushButton("Non connecte", statusWidget);
    btnConnection->setCheckable(true);
    btnConnection->setFixedHeight(45);
    btnConnection->setStyleSheet("background-color: transparent; border: none; color: white; font-weight: bold;");
    statusLayout->addWidget(btnConnection);

    barMenu->addWidget(statusWidget);

    m_connectionCheckManager = new QNetworkAccessManager(this);
    m_connectionCheckTimer = new QTimer(this);
    connect(m_connectionCheckTimer, &QTimer::timeout, this, &MainWindow::verifyConnection);
    m_connectionCheckTimer->start(5000);

    buildScanPage();

    connect(btnConnection, &QPushButton::clicked, this, [=](){ verifyConnection(); });
    connect(btnLancercaddie,&QPushButton::clicked,this,[=] () { lancercaddie();});
    connect(btnScannerCodeBar, &QPushButton::clicked, this, [=](){ scannerCodeBar(); });
    verifyConnection();
}

void MainWindow::verifyConnection(){
    QNetworkRequest request(QUrl("http://192.168.4.1/status"));
    request.setTransferTimeout(2000); // ms — don't let the UI hang if the hotspot isn't joined

    QNetworkReply *reply = m_connectionCheckManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        updateConnectionUi(reply->error() == QNetworkReply::NoError);
    });
}

void MainWindow::updateConnectionUi(bool connected)
{
    if (connected) {
        statusWidget->setStyleSheet("background-color: #10B981; border-radius: 8px;");
        btnConnection->setText("Connected");
    } else {
        statusWidget->setStyleSheet("background-color: red; border-radius: 8px;");
        btnConnection->setText("Non connecte");
    }
    btnConnection->setStyleSheet("background-color: transparent; border: none; color: white; font-weight: bold;");
}

void MainWindow::lancercaddie()
{

}

void MainWindow::buildScanPage()
{
    scanPageWidget = new QWidget(contentStack);
    scanPageWidget->setStyleSheet("background-color: #070D19;");

    QVBoxLayout *scanLayout = new QVBoxLayout(scanPageWidget);
    scanLayout->setContentsMargins(30, 30, 30, 30);
    scanLayout->setSpacing(20);

    QLabel *scanTitle = new QLabel("<b style='color: white; font-size: 22px;'>Scanner CodeBar</b><br>"
                                   "<span style='color: #718096; font-size: 13px;'>Scanner un produit via la caméra ESP32 ou la webcam PC</span>", scanPageWidget);
    scanLayout->addWidget(scanTitle);

    scanSourceCombo = new QComboBox(scanPageWidget);
    scanSourceCombo->addItem("Caméra ESP32 (WiFi)");
    scanSourceCombo->addItem("Webcam PC");
    scanLayout->addWidget(scanSourceCombo);

    scanDisplayStack = new QStackedWidget(scanPageWidget);

    scanResultStillLabel = new QLabel("Aucune photo", scanPageWidget);
    scanResultStillLabel->setAlignment(Qt::AlignCenter);
    scanResultStillLabel->setMinimumSize(320, 240);
    scanResultStillLabel->setStyleSheet("background-color: black; color: white;");

    scanEsp32LiveLabel = new QLabel("Connexion au flux ESP32...", scanPageWidget);
    scanEsp32LiveLabel->setAlignment(Qt::AlignCenter);
    scanEsp32LiveLabel->setMinimumSize(320, 240);
    scanEsp32LiveLabel->setStyleSheet("background-color: black; color: white;");

    m_pcCapture->viewfinder()->setMinimumSize(320, 240);

    scanDisplayStack->addWidget(scanResultStillLabel);      // index 0: captured still + result
    scanDisplayStack->addWidget(scanEsp32LiveLabel);         // index 1: ESP32 live stream
    scanDisplayStack->addWidget(m_pcCapture->viewfinder());  // index 2: PC webcam live view

    scanLayout->addWidget(scanDisplayStack);

    scanResultLabel = new QLabel("Code scanné : —", scanPageWidget);
    scanResultLabel->setStyleSheet("color: white;");
    scanLayout->addWidget(scanResultLabel);

    scanButton = new QPushButton("Scanner", scanPageWidget);
    scanButton->setStyleSheet("QPushButton { color: white; background-color: #2D63C8; border-radius: 6px; padding: 10px; }");
    scanLayout->addWidget(scanButton);
    scanLayout->addStretch();

    connect(scanSourceCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onScanSourceChanged);
    connect(scanButton, &QPushButton::clicked, this, &MainWindow::onScanButtonClicked);

    connect(m_esp32Capture, &BarcodeCapture::imageReady, this, &MainWindow::onEsp32CaptureReady);
    connect(m_esp32Capture, &BarcodeCapture::captureError, this, &MainWindow::onEsp32CaptureError);

    connect(m_esp32Stream, &Esp32CamStream::frameReady, this, &MainWindow::onEsp32StreamFrame);
    connect(m_esp32Stream, &Esp32CamStream::connectionError, this, &MainWindow::onEsp32StreamError);

    connect(m_pcCapture, &PcCameraCapture::imageReady, this, &MainWindow::onPcImageReady);
    connect(m_pcCapture, &PcCameraCapture::cameraError, this, &MainWindow::onPcError);

    contentStack->addWidget(scanPageWidget); // page 1: Scanner CodeBar
}

void MainWindow::scannerCodeBar()
{
    contentStack->setCurrentWidget(scanPageWidget);
    m_currentScanSource = ScanSource::Esp32;
    scanSourceCombo->setCurrentIndex(0);
    m_pcCapture->stop();
    m_esp32Stream->start(QUrl("http://192.168.4.1:81/stream"));
    scanDisplayStack->setCurrentIndex(1);
}

void MainWindow::onScanSourceChanged(int index)
{
    m_currentScanSource = (index == 1) ? ScanSource::Pc : ScanSource::Esp32;
    scanResultLabel->setText("Code scanné : —");

    if (m_currentScanSource == ScanSource::Pc) {
        m_esp32Stream->stop();
        m_pcCapture->start();
        scanDisplayStack->setCurrentIndex(2);
    } else {
        m_pcCapture->stop();
        m_esp32Stream->start(QUrl("http://192.168.4.1:81/stream"));
        scanDisplayStack->setCurrentIndex(1);
    }
}

void MainWindow::onScanButtonClicked()
{
    scanButton->setEnabled(false);
    scanButton->setText("Scan en cours...");

    if (m_currentScanSource == ScanSource::Esp32) {
        m_esp32Capture->capture(QUrl("http://192.168.4.1/capture"));
    } else {
        m_pcCapture->captureFrame();
    }
}

void MainWindow::onEsp32StreamFrame(const QImage &frame)
{
    if (m_currentScanSource == ScanSource::Esp32 && scanDisplayStack->currentIndex() == 1) {
        scanEsp32LiveLabel->setPixmap(QPixmap::fromImage(frame).scaled(
            scanEsp32LiveLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::onEsp32StreamError(const QString &message)
{
    scanEsp32LiveLabel->setText("Erreur de flux : " + message);
}

void MainWindow::onEsp32CaptureReady(const QImage &image)
{
    processDecodedImage(image);
}

void MainWindow::onEsp32CaptureError(const QString &message)
{
    scanButton->setEnabled(true);
    scanButton->setText("Scanner");
    scanResultLabel->setText("Erreur : " + message);
}

void MainWindow::onPcImageReady(const QImage &image)
{
    processDecodedImage(image);
    QTimer::singleShot(1500, this, &MainWindow::resumeScanLiveView);
}

void MainWindow::onPcError(const QString &message)
{
    scanButton->setEnabled(true);
    scanButton->setText("Scanner");
    scanResultLabel->setText("Erreur : " + message);
}

void MainWindow::resumeScanLiveView()
{
    scanDisplayStack->setCurrentIndex(m_currentScanSource == ScanSource::Esp32 ? 1 : 2);
}

void MainWindow::processDecodedImage(const QImage &image)
{
    scanButton->setEnabled(true);
    scanButton->setText("Scanner");

    scanDisplayStack->setCurrentIndex(0);
    scanResultStillLabel->setPixmap(QPixmap::fromImage(image).scaled(
        scanResultStillLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QList<DecodedBarcode> codes = decodeBarcodes(image);
    if (codes.isEmpty()) {
        scanResultLabel->setText("Code scanné : aucun trouvé — réessayez");
    } else {
        scanResultLabel->setText("Code scanné : " + codes.first().data +
                                 " (" + codes.first().type + ")");
    }

    if (m_currentScanSource == ScanSource::Esp32) {
        QTimer::singleShot(1500, this, &MainWindow::resumeScanLiveView);
    }
}

MainWindow::~MainWindow()
{
    m_esp32Stream->stop();
    m_pcCapture->stop();
    delete ui;
}
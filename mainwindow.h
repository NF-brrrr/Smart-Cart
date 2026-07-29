#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QButtonGroup>
#include "barcodecapture.h"
#include "barcodedecoder.h"
#include "esp32camstream.h"
#include "pccameracapture.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void verifyConnection();
    void lancercaddie();
    void scannerCodeBar();

    void onScanSourceChanged(int index);
    void onScanButtonClicked();
    void onEsp32CaptureReady(const QImage &image);
    void onEsp32CaptureError(const QString &message);
    void onEsp32StreamFrame(const QImage &frame);
    void onEsp32StreamError(const QString &message);
    void onPcImageReady(const QImage &image);
    void onPcError(const QString &message);
    void resumeScanLiveView();

private:
    enum class ScanSource { Esp32, Pc };
    void processDecodedImage(const QImage &image);
    void buildScanPage();

    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *barMenu;
    QLabel *titleLabel;
    QVBoxLayout *statusLayout;
    QPushButton *btnConnection;
    QWidget *statusWidget;
    QWidget *sidebarWidget;
    QWidget *dashboardWidget;

    QStackedWidget *contentStack;

    // Scanner CodeBar page
    QWidget *scanPageWidget;
    QComboBox *scanSourceCombo;
    QStackedWidget *scanDisplayStack;
    QLabel *scanResultStillLabel;
    QLabel *scanEsp32LiveLabel;
    QLabel *scanResultLabel;
    QPushButton *scanButton;

    BarcodeCapture *m_esp32Capture;
    Esp32CamStream *m_esp32Stream;
    PcCameraCapture *m_pcCapture;
    ScanSource m_currentScanSource = ScanSource::Esp32;

    QNetworkAccessManager *m_connectionCheckManager;
    QTimer *m_connectionCheckTimer;
    void updateConnectionUi(bool connected);

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
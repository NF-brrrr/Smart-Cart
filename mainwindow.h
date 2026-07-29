#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "btconnection.h"

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
    void tableauDebord();
    void lancercaddie();
private:
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;

    QVBoxLayout *barMenu;
    QLabel *titleLabel;
    QVBoxLayout *statusLayout;

    QPushButton *btnDashboard;
    QPushButton *btnAuth;
    QPushButton *btnHistory;
    QPushButton *btnConnection;

    QWidget *statusWidget;
    QWidget *sidebarWidget;
    QWidget *dashboardWidget;
    BtConnection* btClient;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

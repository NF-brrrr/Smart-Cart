#ifndef BTCONNECTION_H
#define BTCONNECTION_H

#include <QObject>
#include <QtBluetooth>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>

class BtConnection : public QObject
{
    Q_OBJECT
public:
    explicit BtConnection(QObject *parent = nullptr);
    void startScan();
    void sendData(const QByteArray &data);
    QLowEnergyController* getController();
    QLowEnergyService* getService();
    QLowEnergyCharacteristic getRxCharacteristic();
    QLowEnergyCharacteristic getTxCharacteristic();

private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void scanFinished();

    void controllerConnected();
    void controllerDisconnected();
    void serviceDiscovered(const QBluetoothUuid &gattUuid);
    void discoveryFinished();

    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void characteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value);

private:
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QLowEnergyController *controller;
    QLowEnergyService *service;

    QLowEnergyCharacteristic rxCharacteristic;
    QLowEnergyCharacteristic txCharacteristic;

    const QBluetoothUuid NUS_SERVICE_UUID = QBluetoothUuid(QString("6e400001-b5a3-f393-e0a9-e50e24dcca9e"));
    const QBluetoothUuid NUS_RX_UUID      = QBluetoothUuid(QString("6e400002-b5a3-f393-e0a9-e50e24dcca9e"));
    const QBluetoothUuid NUS_TX_UUID      = QBluetoothUuid(QString("6e400003-b5a3-f393-e0a9-e50e24dcca9e"));

};

#endif // BTCONNECTION_H

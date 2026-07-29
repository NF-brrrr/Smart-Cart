#include "btconnection.h"
#include <QDebug>

BtConnection::BtConnection(QObject *parent) : QObject(parent), controller(nullptr), service(nullptr){
    discoveryAgent= new QBluetoothDeviceDiscoveryAgent(this);
    discoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::deviceDiscovered,this,&BtConnection::deviceDiscovered);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,this,&BtConnection::scanFinished);

}

void BtConnection::startScan(){
    qDebug() << "Starting scan...";
    discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BtConnection::deviceDiscovered(const QBluetoothDeviceInfo &device){
    if (device.name() == "ESP32S3_BLE_Demo") {
        qDebug() << "Found ESP32-S3! Stopping scan...";
        discoveryAgent->stop();

        if (controller) {
            controller->disconnectFromDevice();
            delete controller;
        }

        controller = QLowEnergyController::createCentral(device, this);

        connect(controller, &QLowEnergyController::connected, this, &BtConnection::controllerConnected);
        connect(controller, &QLowEnergyController::disconnected, this, &BtConnection::controllerDisconnected);
        connect(controller, &QLowEnergyController::serviceDiscovered, this, &BtConnection::serviceDiscovered);
        connect(controller, &QLowEnergyController::discoveryFinished, this, &BtConnection::discoveryFinished);

        qDebug() << "Connecting to peripheral...";
        controller->connectToDevice();
    }
}

void BtConnection::serviceDiscovered(const QBluetoothUuid &gattUuid) {
    if (gattUuid == NUS_SERVICE_UUID) {
        qDebug() << "Found Nordic UART Service (NUS)!";
    }
}

void BtConnection::scanFinished(){
    qDebug() << "Scan complete.";
}

void BtConnection::controllerConnected(){
    qDebug() << "Discovering Service...";
    controller->discoverServices();
}

void BtConnection::controllerDisconnected(){
    qDebug() << "Disconnected";
    service=nullptr;
}

void BtConnection::discoveryFinished(){
    qDebug() << "Service discovery finished. Handling services...";
    service = controller->createServiceObject(NUS_SERVICE_UUID,this);

    if (!service){
        qDebug() << "Could not map service object.";
        return;
    }

    connect(service,&QLowEnergyService::stateChanged,this,&BtConnection::serviceStateChanged);
    connect(service,&QLowEnergyService::characteristicChanged,this,&BtConnection::characteristicChanged);

    service->discoverDetails();
}

void BtConnection::serviceStateChanged(QLowEnergyService::ServiceState newState){
    if(newState == QLowEnergyService::ServiceState::RemoteServiceDiscovered){
        qDebug() << "Service details fully discovered";

        rxCharacteristic = service->characteristic(NUS_RX_UUID);
        txCharacteristic = service->characteristic(NUS_TX_UUID);

        if(txCharacteristic.isValid()){
            QLowEnergyDescriptor notificationDescriptor = txCharacteristic.descriptor(
                QBluetoothUuid(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration)
                );
            if(notificationDescriptor.isValid()){
                service->writeDescriptor(notificationDescriptor, QByteArray::fromHex("0100"));
                qDebug() << "Subscribed to data notifications from ESP32";

                sendData("1");
            }
        }
    }
}

void BtConnection::characteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value) {
    if (info.uuid() == NUS_TX_UUID) {
        qDebug() << "Received from ESP32-S3:" << value;
    }
}

QLowEnergyController* BtConnection::getController(){
    return controller;
}

QLowEnergyService* BtConnection::getService(){
    return service;
}

QLowEnergyCharacteristic BtConnection::getRxCharacteristic(){
    return rxCharacteristic;
}

QLowEnergyCharacteristic BtConnection::getTxCharacteristic(){
    return txCharacteristic;
}

void BtConnection::sendData(const QByteArray &data) {
    if (service && rxCharacteristic.isValid()) {
        service->writeCharacteristic(rxCharacteristic, data, QLowEnergyService::WriteWithoutResponse);
        qDebug() << "Sent data to ESP32-S3:" << data;
    } else {
        qDebug() << "Cannot send data, BLE characteristics are not ready.";
    }
}

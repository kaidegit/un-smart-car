#include "BLE-UART.h"

#include "Arduino.h"

using funCB = std::function<void()>;

class MyServerCallbacks : public BLEServerCallbacks {
   public:
    funCB deviceConnected = NULL;
    funCB deviceDisconnected = NULL;

    MyServerCallbacks(funCB ponConn, funCB ponDisc)
        : deviceConnected(ponConn), deviceDisconnected(ponDisc) {}

    void onConnect(BLEServer *pServer) { deviceConnected(); }

    void onDisconnect(BLEServer *pServer) { deviceDisconnected(); }
};

void BLE_UART::init() {
    // Create the BLE Device
    BLEDevice::init("ESP32S3 BTSerial");
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    auto fun1 = std::bind(&BLE_UART::onConnect, this);
    auto fun2 = std::bind(&BLE_UART::onDisconnect, this);
    pServer->setCallbacks(new MyServerCallbacks(fun1, fun2));
    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    // Create a BLE Characteristic
    pTxCharacteristic =
        pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());
    // BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    //     CHARACTERISTIC_UUID_RX,
    //     BLECharacteristic::PROPERTY_WRITE
    // );
    // pRxCharacteristic->setCallbacks(new MyCallbacks());
    pService->start();
    pServer->getAdvertising()->start();
}

void BLE_UART::onConnect() {
    deviceConnected = true;
    Serial.println("Device connected");
}

void BLE_UART::onDisconnect() {
    deviceConnected = false;
    Serial.println("Device disconnected");
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("start advertising");
}

void BLE_UART::write(uint8_t ch) {
    if (deviceConnected) {
        pTxCharacteristic->setValue(&ch, 1);
        pTxCharacteristic->notify();
    } else {
        Serial.println("Device not connected");
    }
}

void BLE_UART::write(uint8_t *str, uint8_t len) {
    if (deviceConnected) {
        pTxCharacteristic->setValue(str, len);
        pTxCharacteristic->notify();
    } else {
        Serial.println("Device not connected");
    }
}
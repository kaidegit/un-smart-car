#include "BLE-UART.h"

#include "Arduino.h"

using funCB = std::function<void()>;

volatile bool isStart = false;

class MyServerCallbacks : public BLEServerCallbacks {
   public:
    funCB deviceConnected = NULL;
    funCB deviceDisconnected = NULL;

    MyServerCallbacks(funCB ponConn, funCB ponDisc)
        : deviceConnected(ponConn), deviceDisconnected(ponDisc) {}

    void onConnect(BLEServer *pServer) { deviceConnected(); }

    void onDisconnect(BLEServer *pServer) { deviceDisconnected(); }
};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            isStart = true;
        }
    }
};

void BLE_UART::init() {
    buf_lock = xSemaphoreCreateMutex();
    xSemaphoreGive(buf_lock);
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
    pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX,
                                                       BLECharacteristic::PROPERTY_NOTIFY |
                                                           BLECharacteristic::PROPERTY_WRITE);
    pTxCharacteristic->addDescriptor(new BLE2902());
    pTxCharacteristic->setCallbacks(new MyCallbacks());
    BLECharacteristic *pRxCharacteristic =
        pService->createCharacteristic(CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new MyCallbacks());
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

size_t BLE_UART::printf(const char *format, ...) {
    char loc_buf[64];
    char *temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if (len < 0) {
        va_end(arg);
        return 0;
    };
    if (len >= sizeof(loc_buf)) {
        temp = (char *)malloc(len + 1);
        if (temp == NULL) {
            va_end(arg);
            return 0;
        }
        len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);
    write((uint8_t *)temp, len);
    if (temp != loc_buf) {
        free(temp);
    }
    return len;
}

void BLE_UART::write(uint8_t *str, uint8_t len) {
    while (bufLen != 0) {
        delay(10);
    }

    xSemaphoreTake(buf_lock, portMAX_DELAY);
    memcpy(buf, str, len);
    bufLen = len;
    xSemaphoreGive(buf_lock);
}

void BLE_UART::TrySend() {
    xSemaphoreTake(buf_lock, portMAX_DELAY);
    if (bufLen == 0) goto __exit;
    if (deviceConnected) {
        pTxCharacteristic->setValue(buf, bufLen);
        pTxCharacteristic->notify();
        bufLen = 0;
    } else {
        Serial.println("Device not connected");
        bufLen = 0;
    }
__exit:
    xSemaphoreGive(buf_lock);
}
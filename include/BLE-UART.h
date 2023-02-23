#pragma once

#include "stdint.h"

#include "freertos/freeRTOS.h"
#include "freertos/semphr.h"

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class BLE_UART {
   private:
    BLEServer *pServer = NULL;
    BLEService *pService = NULL;
    BLECharacteristic *pTxCharacteristic = NULL;
    BLECharacteristic *pRxCharacteristic = NULL;
    bool deviceConnected = false;

    uint8_t buf[1024];
    int bufLen = 0;
    SemaphoreHandle_t buf_lock = NULL;

   public:
    BLE_UART(){};
    ~BLE_UART(){};
    void init();
    // void loop();
    void onConnect();
    void onDisconnect();
    void TrySend();
    void write(uint8_t* str, uint8_t len);
};
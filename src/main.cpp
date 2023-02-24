#include <Arduino.h>
// #include <ArduinoOTA.h>
// #include <ESPmDNS.h>
// #include <WiFi.h>
// #include <WiFiUdp.h>

// #include "AnaGraySensor.h"
#include "BLE-UART.h"
#include "DigGraySensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motor.h"

const char* ssid = "cm_2.4";
const char* password = "yekai7880";

auto LMotor = Motor(17, 18);
auto RMotor = Motor(16, 15);
auto ble_uart = BLE_UART();
auto sensor = DigGraySensor(3, 8, 7, 5, 6);

int weight[] = {-5, -2, 0, 2, 5};

void ble_task(void* arg) {
    while (1) {
        ble_uart.TrySend();
        vTaskDelay(10);
    }
}

// void ota_task(void* arg) {
//     while (1) {
//         ArduinoOTA.handle();
//         vTaskDelay(10);
//     }
// }

void sensor_upload(void* arg) {
    while (1) {
        ble_uart.printf("state:%d %d %d %d %d\r\n", sensor.sensorState[0], sensor.sensorState[1],
                        sensor.sensorState[2], sensor.sensorState[3], sensor.sensorState[4]);
        ble_uart.printf("motor:%d %d\r\n", LMotor.speed, RMotor.speed);
        vTaskDelay(200);
    }
}

void setup() {
    Serial.begin(115200);
    // TODO OTA
    // WiFi.mode(WIFI_STA);
    // // WiFi.begin(ssid, password);
    // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //     Serial.println("Connection Failed! Rebooting...");
    //     delay(5000);
    //     ESP.restart();
    // }
    // Serial.println("Connected!");
    // Serial.printf("IP address: ");
    // Serial.println(WiFi.localIP());
    // ArduinoOTA
    //     .onStart([]() {
    //         String type;
    //         if (ArduinoOTA.getCommand() == U_FLASH)
    //             type = "sketch";
    //         else  // U_SPIFFS
    //             type = "filesystem";

    //         // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using
    //         SPIFFS.end() Serial.println("Start updating " + type);
    //     })
    //     .onEnd([]() { Serial.println("\nEnd"); })
    //     .onProgress([](unsigned int progress, unsigned int total) {
    //         Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    //     })
    //     .onError([](ota_error_t error) {
    //         Serial.printf("Error[%u]: ", error);
    //         if (error == OTA_AUTH_ERROR)
    //             Serial.println("Auth Failed");
    //         else if (error == OTA_BEGIN_ERROR)
    //             Serial.println("Begin Failed");
    //         else if (error == OTA_CONNECT_ERROR)
    //             Serial.println("Connect Failed");
    //         else if (error == OTA_RECEIVE_ERROR)
    //             Serial.println("Receive Failed");
    //         else if (error == OTA_END_ERROR)
    //             Serial.println("End Failed");
    //     });

    // ArduinoOTA.begin();

    LMotor.init();
    RMotor.init();
    sensor.init();
    // sensor.SetThreshold(720);
    ble_uart.init();
    xTaskCreate(ble_task, "ble_task", 4096, NULL, 5, NULL);
    xTaskCreate(sensor_upload, "sensor_upload", 4096, NULL, 5, NULL);
    // xTaskCreate(ota_task, "ota_task", 4096, NULL, 5, NULL);
    // delay(10000);

    // while (isStart == false) {
    //     delay(10);
    // }
}

int state = 0;

void loop() {
    sensor.FreshValue();
    // ble_uart.printf("sensor:%d %d %d %d %d\r\n", sensor.sensorValue[0], sensor.sensorValue[1],
    //                 sensor.sensorValue[2], sensor.sensorValue[3], sensor.sensorValue[4]);
    // ble_uart.printf("state:%d %d %d %d %d\r\n", sensor.sensorState[0], sensor.sensorState[1],
    //                 sensor.sensorState[2], sensor.sensorState[3], sensor.sensorState[4]);
    // delay(1000);
    switch (state) {
        case 0:// 起点到圆
        case 1:// 圆到三角
            auto sum = 0;
            for (auto i = 0; i < 5; i++) {
                sum += sensor.sensorState[i] * weight[i];
            }
            auto speed = abs(sum) * 10 + 50;

            if (sensor.sensorState[0] == 0 && sensor.sensorState[1] == 0 &&
                sensor.sensorState[2] == 0 && sensor.sensorState[3] == 0 &&
                sensor.sensorState[4] == 0) {
                // 检测到可能的圆
                RMotor.SetSpeed(60);
                LMotor.SetSpeed(60);
                delay(200);
                RMotor.SetSpeed(0);
                delay(500);
                state ++;
                goto _next;
            }
            if (sum > 0) {
                LMotor.SetSpeed(speed);
                RMotor.SetSpeed(-speed);
            } else if (sum < 0) {
                LMotor.SetSpeed(-speed);
                RMotor.SetSpeed(speed);
            } else {
                LMotor.SetSpeed(60);
                RMotor.SetSpeed(60);
            }

            break;
    }

_next:
    delay(20);
}

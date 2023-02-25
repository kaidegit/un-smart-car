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

const char* ssid = "yekai";
const char* password = "yekai7880";

int btnPin = 19;

auto LMotor = Motor(17, 18);
auto RMotor = Motor(16, 15);
auto ble_uart = BLE_UART();
auto sensor = DigGraySensor(9, 3, 8, 7, 5, 6, 4);

int weight[] = {-5, -3, -1, 0, 1, 3, 5};
// int weight[] = {-3, 0, 1, 2, 3, 5, 7};

int state = 0;

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

auto dist = 0, strength = 0;

void sensor_upload(void* arg) {
    while (1) {
        ble_uart.printf("state:%d %d %d %d %d %d %d\r\n", sensor.sensorState[0],
                        sensor.sensorState[1], sensor.sensorState[2], sensor.sensorState[3],
                        sensor.sensorState[4], sensor.sensorState[5], sensor.sensorState[6]);
        ble_uart.printf("state: %d\r\n", state);
        ble_uart.printf("dist: %dcm\r\n", dist);
        ble_uart.printf("motor:%d %d\r\n", LMotor.speed, RMotor.speed);
        vTaskDelay(200);
    }
}

int received_bytes = 0;

void onReceiveFunction(void) {
    // This is a callback function that will be activated on UART RX events
    size_t available = Serial1.available();
    received_bytes = available;
    // Serial.printf("onReceive Callback:: There are %d bytes available: ", available);
    char buf[received_bytes] = {0};
    Serial1.readBytes(buf, received_bytes);

    if ((buf[0] == 0x59) && (buf[1] == 0x59)) {
        dist = buf[2] + (buf[3] << 8);
        strength = buf[4] + (buf[5] << 8);
        if ((strength < 1000) || (strength == 0xffff) || (dist == 0)) {
            return;
        }
        // Serial.printf("dist: %dcm\r\n", dist);
    }
    // char hex[received_bytes * 3 + 1] = {0};
    // for (int i = 0; i < received_bytes; i++) {
    //     sprintf(hex + i * 3, "%02X ", buf[i]);
    // }
    // Serial.printf("Received: %s\r\n", hex);
}

void setup() {
    Serial.begin(115200);
    // TODO OTA
    // WiFi.mode(WIFI_STA);
    // WiFi.begin(ssid, password);
    // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //     Serial.println("Connection Failed! Rebooting...");
    //     delay(5000);
    //     ESP.restart();
    // }
    // Serial.println("Connected!");
    // Serial.printf("IP address: ");
    // Serial.println(WiFi.localIP());
    // ArduinoOTA.setHostname("esp32s3");
    // ArduinoOTA.setPort(12345);
    // ArduinoOTA
    //     .onStart([]() {
    //         String type;
    //         if (ArduinoOTA.getCommand() == U_FLASH)
    //             type = "sketch";
    //         else  // U_SPIFFS
    //             type = "filesystem";

    //         // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using
    //         Serial.println("Start updating " + type);
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

    pinMode(btnPin, INPUT_PULLUP);

    Serial1.begin(115200, SERIAL_8N1, 12, 11);
    Serial1.setTimeout(5);
    Serial1.onReceive(onReceiveFunction, true);
    delay(1000);
    // 50ms， 20Hz
    uint8_t set_freq[] = {0x5A, 0x06, 0x03, 0x14, 0x00, 0x77};
    Serial1.write(set_freq, 6);
    // Serial1.flush();
    Serial.println("set freq");
}

void SetSpeed(int speed) {
    if (speed != 50) {
        LMotor.SetSpeed(speed);
        RMotor.SetSpeed(-speed);
    } else {
        LMotor.SetSpeed(60);
        RMotor.SetSpeed(60);
    }
}

int CaclSpeed() {
    int sum = 0;
    for (auto i = 0; i < 7; i++) {
        sum += sensor.sensorState[i] * weight[i];
    }
    auto speed = abs(sum) * 10 + 50;
    return sum >= 0 ? speed : -speed;
}

void loop() {
    sensor.FreshValue();

    int sum, speed;
    static int cnt = 0;
    static int inactive_cnt = 0;
    static bool waitToChangeState = false;

    switch (state) {
        case 0:  // 等待按键发车
            if (digitalRead(btnPin) == LOW) {
                // state++;
                // TODO: only test
                state = 5;
            }
            cnt = 0;
            break;
        case 1:  // 直线到圆
            speed = CaclSpeed();

            // 几秒后再判断？
            cnt++;
            // 遇黑 直行后遇白
            if ((cnt > 100) && (sensor.GetActivePinCnt() >= 3)) {
                waitToChangeState = true;
                LMotor.SetSpeed(60);
                RMotor.SetSpeed(60);
                delay(400);
            }
            if (waitToChangeState && (cnt > 100) && (sensor.GetActivePinCnt() == 0)) {
                inactive_cnt++;
                ble_uart.printf("inactive_cnt: %d\r\n", inactive_cnt);
                if (inactive_cnt > 5) {
                    ble_uart.printf("change state\r\n");
                    // 检测到可能的圆
                    LMotor.SetSpeed(-60);
                    RMotor.SetSpeed(-60);
                    delay(650);
                    RMotor.SetSpeed(0);
                    LMotor.SetSpeed(60);
                    delay(500);
                    LMotor.SetSpeed(60);
                    RMotor.SetSpeed(60);
                    delay(200);
                    state++;
                    inactive_cnt = 0;
                    cnt = 0;
                    waitToChangeState = false;
                    goto _next;
                }
            } else {
                inactive_cnt = 0;
            }
            SetSpeed(speed);
            break;
        case 2:  // 圆转出直线
            speed = CaclSpeed();
            cnt++;
            // 右边遇黑
            if ((cnt > 100) &&
                (sensor.sensorState[4] + sensor.sensorState[5] + sensor.sensorState[6] >= 2)) {
                LMotor.SetSpeed(-60);
                RMotor.SetSpeed(-60);
                delay(200);
                RMotor.SetSpeed(0);
                LMotor.SetSpeed(60);
                delay(500);
                LMotor.SetSpeed(60);
                RMotor.SetSpeed(60);
                delay(200);
                state++;
                cnt = 0;
                goto _next;
            }
            SetSpeed(speed);
            break;
        case 3:  // 直线进三角
            speed = CaclSpeed();
            cnt++;
            // 中间白两边黑
            if ((cnt > 25) && ((sensor.sensorState[0] + sensor.sensorState[1] > 0) &&
                               (sensor.sensorState[5] + sensor.sensorState[6] > 0) &&
                               sensor.sensorState[3] == 0)) {
                LMotor.SetSpeed(-60);
                RMotor.SetSpeed(-60);
                delay(200);
                RMotor.SetSpeed(0);
                LMotor.SetSpeed(60);
                delay(350);
                LMotor.SetSpeed(60);
                RMotor.SetSpeed(60);
                delay(250);
                state++;
                cnt = 0;
                goto _next;
            }
            SetSpeed(speed);
            break;
        case 4:  // 三角中间转弯
            speed = CaclSpeed();
            cnt++;
            // 左边黑
            if ((cnt > 25) &&
                (sensor.sensorState[0] + sensor.sensorState[1] + sensor.sensorState[2] >= 2)) {
                LMotor.SetSpeed(-60);
                RMotor.SetSpeed(-60);
                delay(200);
                RMotor.SetSpeed(60);
                LMotor.SetSpeed(0);
                delay(1000);
                LMotor.SetSpeed(60);
                RMotor.SetSpeed(60);
                delay(250);
                state++;
                cnt = 0;
                goto _next;
            }
            SetSpeed(speed);
            break;
        case 5:  // 避障
            speed = CaclSpeed();
            cnt++;
            // 40cm障碍物
            if ((cnt > 20) && (dist <= 40)) {
                RMotor.SetSpeed(0);
                LMotor.SetSpeed(60);
                delay(500);
                LMotor.SetSpeed(60);
                RMotor.SetSpeed(60);
                delay(1500);
                RMotor.SetSpeed(60);
                LMotor.SetSpeed(0);
                delay(1000);
                LMotor.SetSpeed(60);
                RMotor.SetSpeed(60);
                state++;
                cnt = 0;
                goto _next;
            }
            SetSpeed(speed);
            break;
        case 6:  // 等待归线
            speed = CaclSpeed();
            cnt++;
            // 左边有黑
            if ((cnt > 25) &&
                (sensor.sensorState[0] + sensor.sensorState[1] + sensor.sensorState[2] >= 1)) {
                RMotor.SetSpeed(60);
                LMotor.SetSpeed(60);
                delay(200);
                RMotor.SetSpeed(0);
                LMotor.SetSpeed(60);
                delay(400);
                state++;
                cnt = 0;
                goto _next;
            }
            SetSpeed(speed);
            break;
        default:
            speed = CaclSpeed();
            SetSpeed(speed);
            break;
    }

_next:
    delay(20);
}

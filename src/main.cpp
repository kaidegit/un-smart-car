#include <Arduino.h>

#include "motor.h"
#include "esp_log.h"

auto LMotor = Motor(17, 18);
auto RMotor = Motor(16, 15);

void setup() {
    Serial.begin(115200);
    LMotor.init();
    RMotor.init();
    // pinMode(15, OUTPUT);
    // digitalWrite(15, LOW);
    // pinMode(16, OUTPUT);
    // digitalWrite(16, LOW);
    // pinMode(17, OUTPUT);
    // digitalWrite(17, LOW);
    // pinMode(18, OUTPUT);
    // digitalWrite(18, LOW);
    delay(10000);
}

void loop() {
    // 向前
    // Serial.println("向前");
    // digitalWrite(15, LOW);
    // digitalWrite(18, LOW);
    // digitalWrite(16, HIGH);
    // digitalWrite(17, HIGH);
    // delay(5000);
    // // 向后
    // Serial.println("向后");
    // digitalWrite(16, LOW);
    // digitalWrite(17, LOW);
    // digitalWrite(15, HIGH);
    // digitalWrite(18, HIGH);
    // delay(5000);

    // LMotor
    Serial.println("LMotor");
    LMotor.SetSpeed(100);
    delay(5000);
    LMotor.SetSpeed(-100);
    delay(5000);
    LMotor.SetSpeed(0);
    delay(5000);
    // RMotor
    Serial.println("RMotor");
    RMotor.SetSpeed(100);
    delay(5000);
    RMotor.SetSpeed(-100);
    delay(5000);
    RMotor.SetSpeed(0);
    delay(5000);
}
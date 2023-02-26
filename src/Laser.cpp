#include "Laser.h"

#include "Arduino.h"

Laser::Laser(int rxPin, int txPin) : rxPin(rxPin), txPin(txPin) {}

void Laser::init() {
    Serial1.begin(115200, SERIAL_8N1, rxPin, txPin);
    Serial1.setTimeout(5);
    // 50ms， 20Hz
    uint8_t set_freq[] = {0x5A, 0x06, 0x03, 0x14, 0x00, 0x77};
    Serial1.onReceive(
        [this]() {
            // Serial.println("onReceive");
            size_t available = Serial1.available();
            // Serial.printf("get %d bytes\r\n", available);
            char buf[available] = {0};
            Serial1.readBytes(buf, available);

            if ((available == 9) && (buf[0] == 0x59) && (buf[1] == 0x59)) {
                // if ((buf[0] == 0x59) && (buf[1] == 0x59)) {
                dist = buf[2] + (buf[3] << 8);
                strength = buf[4] + (buf[5] << 8);
                if ((strength < 1000) || (strength == 0xffff) || (dist == 0)) {
                    return;
                }
                Serial.printf("dist: %dcm\r\n", dist);
            }
        },
        true);
    Serial1.write(set_freq, 6);
}

int Laser::GetDist() { return dist; }
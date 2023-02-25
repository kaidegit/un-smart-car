#include "Laser.h"

#include "Arduino.h"

Laser::Laser(int rxPin, int txPin)
    : rxPin(rxPin), txPin(txPin) {}

void Laser::init() {
    Serial1.begin(115200, SERIAL_8N1, rxPin, txPin);
    Serial1.setTimeout(5);
    Serial1.onReceive(
        [this]() {
            // Serial.println("onReceive");
            size_t available = Serial1.available();
            char buf[available] = {0};
            Serial1.readBytes(buf, available);

            if ((buf[0] == 0x59) && (buf[1] == 0x59)) {
                dist = buf[2] + (buf[3] << 8);
                strength = buf[4] + (buf[5] << 8);
                if ((strength < 1000) || (strength == 0xffff) || (dist == 0)) {
                    return;
                }
                // Serial.printf("dist: %dcm\r\n", dist);
            }
        },
        true);
}

int Laser::GetDist() {
    return dist;
}
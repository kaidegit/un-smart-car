#include "motor.h"

volatile uint8_t usedChannelList[8] = {0, 0, 0, 0, 0, 0, 0, 0};

Motor::Motor(int posPin, int negPin) : posPin(posPin), negPin(negPin) {}

void Motor::init() {
    for (int i = 0; i < 8; i++) {
        if (usedChannelList[i] == 0) {
            usedChannelList[i] = 1;
            posChan = i;
            break;
        }
    }
    ledcSetup(posChan, 12000, 10);
    ledcAttachPin(posPin, posChan);
    for (int i = 0; i < 8; i++) {
        if (usedChannelList[i] == 0) {
            usedChannelList[i] = 1;
            negChan = i;
            break;
        }
    }
    ledcSetup(negChan, 12000, 10);
    ledcAttachPin(negPin, negChan);
    Serial.printf("regist a motor at posPin: %d, negPin: %d with posChan: %d, negChan: %d\r\n", posPin,
             negPin, posChan, negChan);
}

void Motor::SetSpeed(int speed) {
    speed = min(speed, 100);
    this->speed = speed;
    auto duty = speed * 1023 / 100;
    if (speed > 0) {
        // 向前
        ledcWrite(negChan, 0);
        ledcWrite(posChan, duty);
    } else {
        // 向后
        ledcWrite(posChan, 0);
        ledcWrite(negChan, -duty);
    }
}
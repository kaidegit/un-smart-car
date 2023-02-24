#include "AnaGraySensor.h"

AnaGraySensor::AnaGraySensor(int pin0, int pin1, int pin2, int pin3, int pin4) {
    pinArr[0] = pin0;
    pinArr[1] = pin1;
    pinArr[2] = pin2;
    pinArr[3] = pin3;
    pinArr[4] = pin4;
}

void AnaGraySensor::init() {
    analogSetAttenuation(ADC_6db);
    for (int i = 0; i < 5; i++) {
        pinMode(pinArr[i], INPUT);
    }
}

void AnaGraySensor::SetThreshold(int threshold) {
    this->threshold = threshold;
}

void AnaGraySensor::FreshValue() {
    for (int i = 0; i < 5; i++) {
        sensorValue[i] = analogRead(pinArr[i]);
        if (sensorValue[i] > threshold) {
            sensorState[i] = 1;
        } else {
            sensorState[i] = 0;
        }
    }
}




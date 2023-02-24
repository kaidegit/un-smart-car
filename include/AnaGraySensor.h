#pragma once

#include "Arduino.h"

class AnaGraySensor {
   private:
    int pinArr[5];
    int threshold;

   public:
    AnaGraySensor(int pin0, int pin1, int pin2, int pin3, int pin4);
    ~AnaGraySensor(){};
    void init();
    void FreshValue();
    void SetThreshold(int threshold);

    int sensorValue[5];
    int sensorState[5];
};
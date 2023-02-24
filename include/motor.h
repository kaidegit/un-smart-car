#pragma once

#include "Arduino.h"

class Motor {
   private:
    int speed;
    int posPin, negPin;
    int posChan = -1, negChan = -1;

   public:
    Motor(int posPin, int negPin);
    ~Motor(){};
    void init();
    void SetSpeed(int speed);
};
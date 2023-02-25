#pragma once

#include <Arduino.h>

class Laser
{
private:
    int dist, strength;
    int rxPin, txPin;
public:
    Laser(int rxPin, int txPin);
    ~Laser() = default;

    void init();
    int GetDist();
};
#pragma once

class DigGraySensor
{
private:
    /* data */
    int pinArr[7];

public:
    DigGraySensor(int pin1, int pin2, int pin3, int pin4, int pin5);
    ~DigGraySensor();
    void init();
    void FreshValue();

    int sensorState[7];
};
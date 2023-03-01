#pragma once

class DigGraySensor
{
private:
    /* data */
    int pinArr[7];
    int pinNum = 5;

public:
    DigGraySensor(int pin1, int pin2, int pin3, int pin4, int pin5);
    DigGraySensor(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7);
    ~DigGraySensor() = default;
    void init();
    void FreshValue();
    int GetActivePinCnt();

    int sensorState[7];
};
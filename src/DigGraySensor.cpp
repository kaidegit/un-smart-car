#include "Arduino.h"
#include "DigGraySensor.h"

DigGraySensor::DigGraySensor(int pin1, int pin2, int pin3, int pin4, int pin5)
{
    pinArr[0] = pin1;
    pinArr[1] = pin2;
    pinArr[2] = pin3;
    pinArr[3] = pin4;
    pinArr[4] = pin5;
    pinNum = 5;
}

DigGraySensor::DigGraySensor(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin7)
{
    pinArr[0] = pin1;
    pinArr[1] = pin2;
    pinArr[2] = pin3;
    pinArr[3] = pin4;
    pinArr[4] = pin5;
    pinArr[5] = pin6;
    pinArr[6] = pin7;
    pinNum = 7;
}

DigGraySensor::~DigGraySensor()
{
}
    
void DigGraySensor::init()
{
    for (int i = 0; i < pinNum; i++)
    {
        pinMode(pinArr[i], INPUT);
    }
}

void DigGraySensor::FreshValue()
{
    for (int i = 0; i < pinNum; i++)
    {
        sensorState[i] = digitalRead(pinArr[i]);
    }
}

int DigGraySensor::GetActivePinCnt()
{
    int sum = 0;
    for (int i = 0; i < pinNum; i++)
    {
        sum += sensorState[i];
    }
    return sum;
}
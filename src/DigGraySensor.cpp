#include "Arduino.h"
#include "DigGraySensor.h"

DigGraySensor::DigGraySensor(int pin1, int pin2, int pin3, int pin4, int pin5)
{
    pinArr[0] = pin1;
    pinArr[1] = pin2;
    pinArr[2] = pin3;
    pinArr[3] = pin4;
    pinArr[4] = pin5;
}

DigGraySensor::~DigGraySensor()
{
}
    
void DigGraySensor::init()
{
    for (int i = 0; i < 5; i++)
    {
        pinMode(pinArr[i], INPUT);
    }
}

void DigGraySensor::FreshValue()
{
    for (int i = 0; i < 5; i++)
    {
        sensorState[i] = digitalRead(pinArr[i]);
    }
}
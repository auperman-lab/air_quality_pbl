#pragma once
#include <Arduino.h>

class HumidifierDriver
{
public:
    explicit HumidifierDriver(uint8_t pin);
    void begin();
    void turnOn();
    void turnOff();
    bool isOn() const;

private:
    uint8_t _pin;
    bool _state;
};
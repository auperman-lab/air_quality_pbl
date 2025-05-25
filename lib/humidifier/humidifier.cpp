#include "humidifier.h"

HumidifierDriver::HumidifierDriver(uint8_t pin)
    : _pin(pin), _state(false) {}

void HumidifierDriver::begin()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void HumidifierDriver::turnOn()
{
    digitalWrite(_pin, HIGH);
    _state = true;
}

void HumidifierDriver::turnOff()
{
    digitalWrite(_pin, LOW);
    _state = false;
}

bool HumidifierDriver::isOn() const
{
    return _state;
}
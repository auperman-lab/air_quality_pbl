#include "env_sensor.h"

EnvSensor::EnvSensor(uint8_t pin)
    : _dht(pin, DHTTYPE) {}

void EnvSensor::begin()
{
    _dht.begin();
}

float EnvSensor::readHumidity()
{
    return _dht.readHumidity();
}

float EnvSensor::readTemperature()
{
    return _dht.readTemperature();
}
#pragma once
#include <Arduino.h>
#include <DHT.h>

#ifndef DHTTYPE
#define DHTTYPE DHT22
#endif

class EnvSensor
{
public:
    explicit EnvSensor(uint8_t pin);
    void begin();
    float readHumidity();
    float readTemperature();

private:
    DHT _dht;
};
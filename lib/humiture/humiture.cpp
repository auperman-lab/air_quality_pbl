#include <humiture.h>
#include <DHT.h>

Humiture::Humiture(int pin) : pin(pin), dhtSensor()
{
    dhtSensor.setup(pin, DHT::DHT22);
}

float Humiture::getTemperature()
{
    float temp = dhtSensor.getTemperature();
    if (isnan(temp))
    {
        Serial.println("Failed to read temperature");
        return -1;
    }
    return temp;
}

float Humiture::getHumidity()
{
    float humidity = dhtSensor.getHumidity();
    if (isnan(humidity))
    {
        Serial.println("Failed to read humidity");
        return -1;
    }
    return humidity;
}

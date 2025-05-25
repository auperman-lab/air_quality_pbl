#include "air_control.h"
#include <BlynkSimpleEsp8266.h>

AirControlService::AirControlService(WindowDriver &wd,
                                     HumidifierDriver &hd,
                                     EnvSensor &es,
                                     float lowHum,
                                     float highHum)
    : _window(wd), _humidifier(hd), _sensor(es), _lowThreshold(lowHum), _highThreshold(highHum)
{
}

void AirControlService::begin()
{
    // no initialization needed
}

void AirControlService::update()
{
    float h = _sensor.readHumidity();
    Blynk.virtualWrite(V2, h);

    if (h < _lowThreshold)
    {
        _humidifier.turnOn();
        _window.openToPercent(20.0f);
    }
    else if (h > _highThreshold)
    {
        _humidifier.turnOff();
        _window.closeToPercent(0.0f);
    }
}
#pragma once
#include <window.h>
#include <humidifier.h>
#include <env_sensor.h>

class AirControlService
{
public:
    AirControlService(WindowDriver &wd,
                      HumidifierDriver &hd,
                      EnvSensor &es,
                      float lowHum = 40.0f,
                      float highHum = 60.0f);

    void begin();
    void update();

private:
    WindowDriver &_window;
    HumidifierDriver &_humidifier;
    EnvSensor &_sensor;
    float _lowThreshold;
    float _highThreshold;
};
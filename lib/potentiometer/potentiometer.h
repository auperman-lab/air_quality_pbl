#ifndef SRV_POTENTIOMETER_H
#define SRV_POTENTIOMETER_H

#include <stdint.h>
#include <Arduino.h>  


class Potentiometer {
public:
    Potentiometer(int analogPin);

    void begin();

    int readRaw() const;

    float readMappedFloat(float minVal = 0., float maxVal = 100.) const;

    int readMapped(int minVal = 0, int maxVal = 100) const;


private:
    int analogPin;
};

#endif 

#ifndef DS18B20_H
#define DS18B20_H

#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>




class DS18B20_Reader {
    private:
        OneWire oneWire;
        DallasTemperature ds;

    public:
    DS18B20_Reader(int pin); 

    void ds_setup();

    float getTemperature();

};

#endif  


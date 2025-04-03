#ifndef HUMITURE_H
#define HUMITURE_H

#include <Arduino.h>
#include <DHT.h>



class Humiture {
    private:
        int pin;
        DHT dhtSensor;

    public:
    Humiture(int pin); 

    void humiture_setup();

    float getTemperature();

    float getHumidity();

};

#endif  


#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

class Relay {
private:
    int relayPin;
    bool state;
public:
    Relay(int pin);
    void on();
    void off();
    bool isOn();
};

#endif
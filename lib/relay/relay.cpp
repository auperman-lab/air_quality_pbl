
#include "relay.h"
#include <Arduino.h>

Relay::Relay(int pin) {
    relayPin = pin;
    pinMode(relayPin, OUTPUT);
    state = false;
    off();
}


void Relay::on() {
    digitalWrite(relayPin, HIGH);
    state = true;
}

void Relay::off() {
    digitalWrite(relayPin, LOW);
    state = false;
}

bool Relay::isOn() {
    return state;
}
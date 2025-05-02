#include "potentiometer.h"

// Constructor
Potentiometer::Potentiometer(int analogPin) : analogPin(analogPin) {}

void Potentiometer::begin() {
    pinMode(analogPin, INPUT);
}

int Potentiometer::readRaw() const {
    return analogRead(analogPin);
}

int Potentiometer::readMapped(int minVal, int maxVal) const {
    int raw = readRaw();
    return map(raw, 0, 1023, minVal, maxVal);
}


float Potentiometer::readMappedFloat(float minVal, float maxVal) const {
    int raw = readRaw();
    float ratio = raw / 1023.0f;
    return minVal + (maxVal - minVal) * ratio;
}

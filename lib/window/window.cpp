#include "window.h"
#include <Arduino.h>

WindowDriver::WindowDriver(uint8_t mO, uint8_t mC,
                           uint8_t hO, uint8_t hC,
                           uint8_t eMO, uint8_t eMC,
                           uint8_t eHO, uint8_t eHC,
                           float fullMain, float fullHand)
    : _mO(mO), _mC(mC), _hO(hO), _hC(hC), _eMO(eMO), _eMC(eMC), _eHO(eHO), _eHC(eHC), _tMain(fullMain), _tHand(fullHand)
{
}

void WindowDriver::begin()
{
    pinMode(_mO, OUTPUT);
    digitalWrite(_mO, LOW);
    pinMode(_mC, OUTPUT);
    digitalWrite(_mC, LOW);
    pinMode(_hO, OUTPUT);
    digitalWrite(_hO, LOW);
    pinMode(_hC, OUTPUT);
    digitalWrite(_hC, LOW);

    pinMode(_eMO, INPUT_PULLUP);
    pinMode(_eMC, INPUT_PULLUP);
    pinMode(_eHO, INPUT_PULLUP);
    pinMode(_eHC, INPUT_PULLUP);
}

bool WindowDriver::isFullyOpen() const
{
    return digitalRead(_eMO) == LOW;
}

bool WindowDriver::isFullyClosed() const
{
    return digitalRead(_eMC) == LOW;
}

bool WindowDriver::openToPercent(float pct)
{
    pct = constrain(pct, 0.0f, 100.0f);
    if (isFullyOpen())
        return true;
    float dur = percentToTime(pct, _tMain);
    drive(_mO, _mC, dur);
    return isFullyOpen();
}

bool WindowDriver::closeToPercent(float pct)
{
    pct = constrain(pct, 0.0f, 100.0f);
    if (isFullyClosed())
        return true;
    float dur = percentToTime(100.0f - pct, _tMain);
    drive(_mC, _mO, dur);
    return isFullyClosed();
}

void WindowDriver::drive(uint8_t pinOn, uint8_t pinOff, float duration)
{
    digitalWrite(pinOn, HIGH);
    delay((unsigned long)(duration * 1000));
    digitalWrite(pinOn, LOW);
}

float WindowDriver::percentToTime(float pct, float fullTime) const
{
    return (pct / 100.0f) * fullTime;
}
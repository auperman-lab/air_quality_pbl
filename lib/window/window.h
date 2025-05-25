#pragma once
#include <Arduino.h>

class WindowDriver
{
public:
    WindowDriver(uint8_t mO, uint8_t mC,
                 uint8_t hO, uint8_t hC,
                 uint8_t eMO, uint8_t eMC,
                 uint8_t eHO, uint8_t eHC,
                 float fullMain, float fullHand);

    void begin();
    bool openToPercent(float pct);
    bool closeToPercent(float pct);
    bool isFullyOpen() const;
    bool isFullyClosed() const;

private:
    uint8_t _mO, _mC, _hO, _hC, _eMO, _eMC, _eHO, _eHC;
    float _tMain, _tHand;

    void drive(uint8_t pinOn, uint8_t pinOff, float duration);
    float percentToTime(float pct, float fullTime) const;
};
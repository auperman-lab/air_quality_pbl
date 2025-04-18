#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include <Arduino.h>  


class Motor {
public:
    Motor(int pwmPin, int dirPin);

    void begin();

    void stop();

    void brakeMotor();

    void inc(int16_t incSpeed);

    void dec(int16_t decSpeed);

    void setDirection(bool direction);

    void setSpeed(int8_t percentent);

    int8_t getSpeed() const;

    bool getDirection() const;


private:
    int pwmPin;
    int dirPin;
    bool direction;
    float speed;
    bool brake;

    void updatePWM();
};

#endif 

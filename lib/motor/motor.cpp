#include "motor.h"

Motor::Motor(int pwmPin, int dirPin)
    : pwmPin(pwmPin), dirPin(dirPin), direction(true), brake(false), speed(0) {}

void Motor::begin() {
    pinMode(pwmPin, OUTPUT);
    analogWrite(pwmPin, 0); 
    analogWrite(dirPin, 0);
}

void Motor::stop() {
    speed = 0;
    brake = false;
    updatePWM();

}
void Motor::brakeMotor(){
    speed = 0;
    brake = true;
    updatePWM();
}

void Motor::inc(int16_t incSpeed) {
    if (direction && speed < 100) {
        speed += incSpeed;
        if (speed > 100) speed = 100;
    } else if (!direction && speed > -100) {
        speed -= incSpeed;
        if (speed < -100) speed = -100;
    }
    updatePWM();
}

void Motor::dec(int16_t decSpeed) {
    if (direction && speed > 0) {
        speed -= decSpeed;
        if (speed < 0) speed = 0;
    } else if (!direction && speed < 0) {
        speed += decSpeed;
        if (speed > 0) speed = 0;
    }
    updatePWM();
}

void Motor::setDirection(bool dir) {
    direction = dir;
    if (direction && speed < 0) {
        speed = -speed;
    } else if (!direction && speed > 0) {
        speed = -speed;
    }
    updatePWM();
}

void Motor::setSpeed(int8_t percent) {
    if (percent > 100) percent = 100;
    if (percent < -100) percent = -100;

    direction = percent >= 0;
    speed = percent;
    updatePWM();
}

int8_t Motor::getSpeed() const {
    return speed;
}

bool Motor::getDirection() const {
    return direction;
}

void Motor::updatePWM() {
    if (speed == 0 && !brake) {
        analogWrite(pwmPin, 0);      
        digitalWrite(dirPin, LOW);
        return;
    }

    if (brake) {
        analogWrite(pwmPin, 255);     
        digitalWrite(dirPin, HIGH);  
        return;
    } 

    int duty = map(abs(speed), 0, 100, 0, 255);
    analogWrite(pwmPin, duty);
    digitalWrite(dirPin, direction ? LOW : HIGH);  
}

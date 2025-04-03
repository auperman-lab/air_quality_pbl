#include <led.h>
#include <Arduino.h>


Led::Led(int led_pin): led_pin(led_pin == 0 ? LED_BUILTIN : led_pin) {
    Serial.print("ledpin = ");
    Serial.println(led_pin);
    pinMode(this->led_pin, OUTPUT);
}

void Led::on() {
    return digitalWrite(led_pin, HIGH);
}

void Led::off() {
    return digitalWrite(led_pin, LOW);
}

bool Led::is_on() {
    return digitalRead(led_pin);
}
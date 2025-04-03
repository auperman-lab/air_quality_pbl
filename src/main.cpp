#include <Arduino.h>
#include "led.h"
#include "morse_code.h"

Led led(9);
Morse_Code mc(led);


void setup() {
  Serial.begin(9600);
  Serial.println("Hello World");

}

void loop() {
  mc.print("hello world");
  led.off();
  led.on();
  delay(1000);
}
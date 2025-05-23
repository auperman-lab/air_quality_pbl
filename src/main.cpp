#include <Arduino.h>
#include <GyverPID.h>
#include "led.h"
#include "morse_code.h"
#include "humiture.h"
#include "ds18b20.h"
#include "lcd_i2c.h"
#include "relay.h"
#include "salt_pepper.h"
#include "wma.h"
#include "motor.h"
#include "potentiometer.h"

LCDService lcd;
//sda a4
//scl a5

Relay relay(7);
Humiture dht(6);
Potentiometer potentiometer(2);
// Motor motor(6,5);

// SaltPepperFilter temp_filter;
// WeightedMovingAverageFilter temp_filter_second_stage;
int hyster = 1;


float hum;
float temp;
float pot;

// GyverPID setup: (Kp, Ki, Kd, direction = DIRECT)
GyverPID pid(2.0, 5.0, 1.0);

double input, setpoint, output;


void setup() {
  Serial.begin(9600);
  Serial.println("Hello Humiture");
  lcd.init();
  lcd.clear();
  potentiometer.begin();
}


// lab 5.2

// void loop (){
//   temp = dht.getTemperature();
//   pot = potentiometer.readMappedFloat(10.0, 30.0);
//   char tempStr[10]; 
//   dtostrf(temp, 3, 1, tempStr);
//   char potStr[10]; 
//   dtostrf(pot, 3, 1, potStr);

//   pid.input = temp;
//   pid.setpoint = pot;

//   float pidOutput = pid.getResult();  // calculate PID output

//   if (pidOutput > 128) {
//     relay.on();
//   } else {
//     relay.off();
//   }
  
//   Serial.print("Temp Sensor :");
//   Serial.println(temp);
//   Serial.print("Relay state:");
//   Serial.println(relay.isOn());
//   Serial.print("Potentiometer Value (10 ~ 30): ");
//   Serial.println(pot);
//   Serial.print("PID Output: ");
//   Serial.println(pidOutput);
//   Serial.println();

//   printf("Temp:%sC\nPot:%sC", tempStr, potStr);


//   delay(1000);
//   lcd.clear();
// }



//lab 5.1

// void loop (){
//   temp = dht.getTemperature();
//   pot = potentiometer.readMappedFloat(10.0, 30.0);
//   char tempStr[10]; 
//   dtostrf(temp, 3, 1, tempStr);
//   char potStr[10]; 
//   dtostrf(pot, 3, 1, potStr);
  
//   Serial.print("Temp Sensor :");
//   Serial.println(temp);
//   Serial.print("Relay state:");
//   Serial.println(relay.isOn());
//   Serial.print("Potentiometer Value (10 ~ 30): ");
//   Serial.println(pot);
//   Serial.println();

//   printf("Temp:%sC\nPot:%sC", tempStr, potStr);

//   if(temp < (pot-hyster)){
//     relay.on();
//   }else if(temp > (pot+hyster)){
//     relay.off();
//   } 


//   delay(1000);
//   lcd.clear();
// }


//lab 4.2
// void loop() {
//   if (Serial.available() > 0) {
//     read = Serial.readString();
//     read.trim(); // remove leading/trailing whitespace

//     Serial.println(read);

//     if (read.startsWith("motor set")) {
//       int value = read.substring(10).toInt();
//       motor.setSpeed(value);
//       Serial.print("Motor power set to: ");
//       Serial.println(value);

//     } else if (read.equals("motor stop")) {
//       motor.stop();
//       Serial.println("Motor stopped");

//     } else if (read.equals("motor max")) {
//       int8_t current = motor.getSpeed();
//       int8_t maxVal = current >= 0 ? 100 : -100;
//       motor.setSpeed(maxVal);
//       Serial.print("Motor set to max: ");
//       Serial.println(maxVal);

//     } else if (read.equals("motor inc")) {
//       motor.inc(10);
//       Serial.print("Motor increased to: ");
//       Serial.println(motor.getSpeed());

//     } else if (read.equals("motor dec")) {
//       motor.dec(10);
//       Serial.print("Motor decreased to: ");
//       Serial.println(motor.getSpeed());

//     } else {
//       Serial.println("Unknown command");
//     }

//     Serial.print("Current motor speed: ");
//     Serial.println(motor.getSpeed());
//   }

//   delay(200); // optional throttle
// }



// lab 4.1

// void loop(){
//   printf("Relay : %s\n", relay.isOn() ? "is On" : "is Off");

//   if (Serial.available() > 0) {
//     read = Serial.readString(); 

//     if (read.equals("relay on")) {
//       relay.on();
//       printf("Relay : %s\nRelay cmd on", relay.isOn() ? "is On" : "is Off");
//       delay(1000); 

//     } else if (read.equals("relay off")) {
//       relay.off();
//       printf("Relay : %s\nRelay cmd off", relay.isOn() ? "is On" : "is Off");
//       delay(1000); 
//     } else {
//       printf("Relay : %s\nRelay cmd unknown", relay.isOn() ? "is On" : "is Off");
//       delay(1000); 
//     }
    
//     Serial.println(read);  
//   }

//   delay(1000); 
//   lcd.clear();
// }


// lab 3.1
// void loop() {
//   hum = dht.getHumidity();
//   temp = dht.getTemperature();

//   char tempStr[10]; 
//   char humStr[10];  

//   dtostrf(temp, 3, 1, tempStr);
//   dtostrf(hum, 3, 1, humStr);    

//   if(hum > 70){

//   }
  
//   printf("Temp:%sC\nHum:%s", tempStr, humStr);

//   Serial.print("DHT22 =>");
//   Serial.print("Humidity ");
//   Serial.print(hum);
//   Serial.print(" % (error=> 2-5%), Temp: ");
//   Serial.print(temp);
//   Serial.println(" Celsius (error=> ±0.5°C)");

//   delay(1*1000);

//   lcd.clear();
// }


// lab 3.2
// void loop() {
//   hum = dht.getHumidity();
//   float rawtemp = dht.getTemperature();

//   float filteredTemp = temp_filter.apply(rawtemp);
//   filteredTemp = temp_filter_second_stage.apply(filteredTemp);

//   temp = filteredTemp;

//   char tempStr[10]; 
//   char humStr[10];  

//   dtostrf(temp, 3, 1, tempStr);
//   dtostrf(hum, 3, 1, humStr);    
  
//   printf("Temp:%sC\nHum:%s", tempStr, humStr);

//   Serial.print("DHT22 =>");
//   Serial.print("Humidity ");
//   Serial.print(hum);
//   Serial.print(" % (error=> 2-5%)\nFiltered Temp: ");
//   Serial.print(temp);
//   Serial.print(" Celsius (error=> ±0.5°C)");
//   Serial.print("\nRaw Temp: ");
//   Serial.print(rawtemp);
//   Serial.println(" Celsius (error=> ±0.5°C)\n");

//   delay(1*1000);

//   lcd.clear();


// }
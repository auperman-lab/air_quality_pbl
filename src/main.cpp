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


//lab 6.2

#define EW_RED_PIN    2
#define EW_YELLOW_PIN 3
#define EW_GREEN_PIN  4

// North-South Traffic Light
#define NS_RED_PIN    5
#define NS_YELLOW_PIN 6
#define NS_GREEN_PIN  7

// Potentiometer for "request" simulation
#define POTENTIOMETER_PIN A0

// Global Objects
// East-West LEDs
Led ewRed(EW_RED_PIN);
Led ewYellow(EW_YELLOW_PIN);
Led ewGreen(EW_GREEN_PIN);

// North-South LEDs
Led nsRed(NS_RED_PIN);
Led nsYellow(NS_YELLOW_PIN);
Led nsGreen(NS_GREEN_PIN);

Potentiometer requestPotentiometer(POTENTIOMETER_PIN);

typedef enum {
    RED,
    YELLOW,
    GREEN
} TrafficLightState;

TrafficLightState ewState = GREEN; // East-West starts green (priority)
TrafficLightState nsState = RED;   // North-South starts red

unsigned long ewLastStateChangeTime = 0;
unsigned long nsLastStateChangeTime = 0;
unsigned long potentiometerLastReadTime = 0;

const unsigned long GREEN_LIGHT_DELAY_MS = 5000;  // 5 seconds
const unsigned long YELLOW_LIGHT_DELAY_MS = 2000; // 2 seconds
const unsigned long RED_LIGHT_DELAY_MS = 1000;    // 1 second (minimal, before next green)

volatile bool nsRequestActive = false;
const int REQUEST_THRESHOLD = 50; // Potentiometer value threshold (0-100 mapped)

void updateTrafficLightLEDs(TrafficLightState ew, TrafficLightState ns) {
    // East-West LEDs
    ewRed.off();
    ewYellow.off();
    ewGreen.off();
    if (ew == GREEN) ewGreen.on();
    else if (ew == YELLOW) ewYellow.on();
    else ewRed.on();

    // North-South LEDs
    nsRed.off();
    nsYellow.off();
    nsGreen.off();
    if (ns == GREEN) nsGreen.on();
    else if (ns == YELLOW) nsYellow.on();
    else nsRed.on();
}

void setup() {
    Serial.begin(115200);
    Serial.println("Smart Traffic Light System Starting...");

    requestPotentiometer.begin();

    // Set initial state and update LEDs
    ewState = GREEN;
    nsState = RED;
    updateTrafficLightLEDs(ewState, nsState);
    ewLastStateChangeTime = millis();
    nsLastStateChangeTime = millis();
    potentiometerLastReadTime = millis();

    Serial.println("Initial State: EW_GREEN, NS_RED");
}

void loop() {
    unsigned long currentTime = millis();

    // Read potentiometer periodically
    if (currentTime - potentiometerLastReadTime >= 50) { // Read every 50ms
        int potValue = requestPotentiometer.readMapped(0, 100);
        if (potValue > REQUEST_THRESHOLD) {
            nsRequestActive = true;
        } else {
            nsRequestActive = false;
        }
        potentiometerLastReadTime = currentTime;
    }

    // East-West Traffic Light Logic
    if (ewState == GREEN) {
        if (nsRequestActive && (currentTime - ewLastStateChangeTime >= GREEN_LIGHT_DELAY_MS)) {
            ewState = YELLOW;
            updateTrafficLightLEDs(ewState, nsState);
            ewLastStateChangeTime = currentTime;
            Serial.println("Transition: EW_GREEN -> EW_YELLOW");
        }
    } else if (ewState == YELLOW) {
        if (currentTime - ewLastStateChangeTime >= YELLOW_LIGHT_DELAY_MS) {
            ewState = RED;
            updateTrafficLightLEDs(ewState, nsState);
            ewLastStateChangeTime = currentTime;
            Serial.println("Transition: EW_YELLOW -> EW_RED");
        }
    } else if (ewState == RED) {
        if (nsState == RED && (currentTime - ewLastStateChangeTime >= RED_LIGHT_DELAY_MS)) {
            ewState = GREEN;
            updateTrafficLightLEDs(ewState, nsState);
            ewLastStateChangeTime = currentTime;
            nsRequestActive = false; // Reset the request as EW is now green
            Serial.println("Transition: EW_RED -> EW_GREEN. NS request reset.");
        }
    }

    if (nsState == RED) {
        if (nsRequestActive && ewState == RED && (currentTime - nsLastStateChangeTime >= RED_LIGHT_DELAY_MS)) {
            nsState = GREEN;
            updateTrafficLightLEDs(ewState, nsState);
            nsLastStateChangeTime = currentTime;
            Serial.println("Transition: NS_RED -> NS_GREEN");
        }
    } else if (nsState == GREEN) {
        if (currentTime - nsLastStateChangeTime >= GREEN_LIGHT_DELAY_MS) {
            nsState = YELLOW;
            updateTrafficLightLEDs(ewState, nsState);
            nsLastStateChangeTime = currentTime;
            Serial.println("Transition: NS_GREEN -> NS_YELLOW");
        }
    } else if (nsState == YELLOW) {
        if (currentTime - nsLastStateChangeTime >= YELLOW_LIGHT_DELAY_MS) {
            nsState = RED;
            updateTrafficLightLEDs(ewState, nsState);
            nsLastStateChangeTime = currentTime;
            Serial.println("Transition: NS_YELLOW -> NS_RED");
        }
    }

    static TrafficLightState lastPrintedEWState = (TrafficLightState)-1; 
    static TrafficLightState lastPrintedNSState = (TrafficLightState)-1;
    static unsigned long lastPrintTime = 0;
    const unsigned long PRINT_INTERVAL_MS = 1000; 
    if (ewState != lastPrintedEWState || nsState != lastPrintedNSState || (currentTime - lastPrintTime >= PRINT_INTERVAL_MS)) {
        Serial.print("Current State: EW_");
        if (ewState == GREEN) Serial.print("GREEN");
        else if (ewState == YELLOW) Serial.print("YELLOW");
        else Serial.print("RED");

        Serial.print(", NS_");
        if (nsState == GREEN) Serial.println("GREEN");
        else if (nsState == YELLOW) Serial.println("YELLOW");
        else Serial.println("RED");

        lastPrintedEWState = ewState;
        lastPrintedNSState = nsState;
        lastPrintTime = currentTime;
    }
}


//lab 6.1

// const int LED_PIN = 2;
// const int POT_PIN = A0;

// enum LedState {
//   OFF_STATE,
//   ON_STATE
// };

// LedState currentState = OFF_STATE;

// Led led(LED_PIN);
// Potentiometer potentiometer(POT_PIN);

// const int POT_THRESHOLD = 512;

// bool prevPotThresholdExceeded = false;
// unsigned long lastStateChangeTime = 0;
// const long CHANGE_DEBOUNCE_DELAY = 100;

// void setup() {
//   Serial.begin(9600);
//   lcd.init();
//   lcd.clear();
//   led.off();
//   potentiometer.begin();
//   Serial.println("Initial State: OFF");
// }

// void loop() {
//   int currentPotValue = potentiometer.readRaw();
//   bool currentPotThresholdExceeded = (currentPotValue >= POT_THRESHOLD);

//   if (currentPotThresholdExceeded && !prevPotThresholdExceeded) {
//     if ((millis() - lastStateChangeTime) > CHANGE_DEBOUNCE_DELAY) {
//       lastStateChangeTime = millis();

//       switch (currentState) {
//         case OFF_STATE:
//           currentState = ON_STATE;
//           led.on();
//           Serial.println("Transition: OFF -> ON");
//           Serial.println("Current State: ON");
//           printf("Current State: ON");
//           break;

//         case ON_STATE:
//           currentState = OFF_STATE;
//           led.off();
//           Serial.println("Transition: ON -> OFF");
//           Serial.println("Current State: OFF");
//           printf("Current State: OFF");

//           break;
//       }
//     }
//   }
//   prevPotThresholdExceeded = currentPotThresholdExceeded;
// }


// Relay relay(7);
// Humiture dht(6);
// Potentiometer potentiometer(2);
// Motor motor(6,5);

// SaltPepperFilter temp_filter;
// WeightedMovingAverageFilter temp_filter_second_stage;
// int hyster = 1;


// float hum;
// float temp;
// float pot;

// GyverPID setup: (Kp, Ki, Kd, direction = DIRECT)
// GyverPID pid(2.0, 5.0, 1.0);

// double input, setpoint, output;


// void setup() {
//   Serial.begin(9600);
//   Serial.println("Hello Humiture");
//   lcd.init();
//   lcd.clear();
//   potentiometer.begin();
// }


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

//   float pidOutput = pid.getResult();

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


//   Serial.print(temp);        
//   Serial.print("\t");      
//   Serial.print(pot);       
//   Serial.print("\t");      
//   Serial.println(pidOutput); 


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
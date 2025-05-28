#include <Arduino.h>
#include <Wire.h>           

#include <air_quality_sensor.h>

#define BLYNK_TEMPLATE_ID   "TMPL4FgjNNjK7"   
#define BLYNK_TEMPLATE_NAME "Air Sensors"
#define BLYNK_AUTH_TOKEN    "E9i5BTPbRLAv4dCXbt3J8Zj_TDGJXDoj"   

char ssid[] = "MTC_k9pn";    
char pass[] = "gZSpR9t3"; 

#include <BlynkSimpleEsp8266.h>

AirQualitySensor airQualitySensor; 
BlynkTimer timer;                

float currentTemperature = NAN;
float currentHumidity = NAN;
uint16_t currentTVOC = 0;
uint16_t currentECO2 = 0;
uint8_t currentAQI = 0;

void sendSensorData() {
    currentTemperature = airQualitySensor.getTemperature();
    currentHumidity = airQualitySensor.getHumidity();

    if (!isnan(currentTemperature) && !isnan(currentHumidity)) {
        airQualitySensor.setENS160Compensation(currentTemperature, currentHumidity);
    } else {
        Serial.println("Invalid T/H data for ENS160 compensation.");
    }

    currentTVOC = airQualitySensor.getTVOC();
    currentECO2 = airQualitySensor.getECO2();
    currentAQI = airQualitySensor.getAQI();

    Serial.print("Temp: ");     Serial.print(currentTemperature); Serial.print(" C | ");
    Serial.print("Hum: ");      Serial.print(currentHumidity);    Serial.print(" % | ");
    Serial.print("TVOC: ");     Serial.print(currentTVOC);        Serial.print(" ppb | ");
    Serial.print("eCO2: ");     Serial.print(currentECO2);        Serial.print(" ppm | ");
    Serial.print("AQI: ");      Serial.println(currentAQI);

    if (Blynk.connected()) {
        Blynk.virtualWrite(V5, currentTemperature);
        Blynk.virtualWrite(V6, currentHumidity);
        Blynk.virtualWrite(V7, currentTVOC);
        Blynk.virtualWrite(V8, currentECO2);
        Blynk.virtualWrite(V9, currentAQI);
        Serial.println("Data sent to Blynk.");
    } else {
        Serial.println("Blynk not connected. Data not sent.");
    }
}

BLYNK_CONNECTED() {
  Serial.println("Blynk connected! Syncing and updating compensation.");
  Blynk.syncAll();
  currentTemperature = airQualitySensor.getTemperature();
  currentHumidity = airQualitySensor.getHumidity();
  if (!isnan(currentTemperature) && !isnan(currentHumidity)) {
    airQualitySensor.setENS160Compensation(currentTemperature, currentHumidity);
  }
}

void setup() {
    Serial.begin(9600);
    delay(100);
    Serial.println("\nStarting Air Quality Monitor...");

    if (!airQualitySensor.begin()) {
        Serial.println("Failed to initialize one or both sensors!");
        while (true) {
            delay(1000);
        }
    }

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    

    timer.setInterval(10000L, sendSensorData);
}

void loop() {
    Blynk.run();
    timer.run();
}
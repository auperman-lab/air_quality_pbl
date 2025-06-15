#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <ESP8266HTTPClient.h>
#include "config.h"
#include <window.h>
#include <humidifier.h>
#include <env_sensor.h>

// instantiate hardware drivers
WindowDriver windowDrv(
    PIN_MAIN_OPEN, PIN_MAIN_CLOSE,
    PIN_HAND_OPEN, PIN_HAND_CLOSE,
    PIN_END_MAIN_O, PIN_END_MAIN_C,
    PIN_END_HAND_O, PIN_END_HAND_C,
    TIME_OPEN_MAIN, TIME_OPEN_HANDLE);
HumidifierDriver humidifier(PIN_HUMIDIFIER);
EnvSensor envSensor(PIN_DHT_DATA);

// thresholds
float lowHum = 40.0f;
float highHum = 60.0f;

Ticker ticker;

void sendToServer(const String &deviceId, float hum, float temp)
{
  WiFiClient client;
  HTTPClient http;

  // Use the modern API with WiFiClient
  http.begin(client, "http://localhost:3000/api/readings");

  http.addHeader("Content-Type", "application/json");
  String payload = String("{\"deviceId\":\"") + deviceId +
                   "\",\"humidity\":" + String(hum) +
                   ",\"temperature\":" + String(temp) + "}";

  int httpCode = http.POST(payload);

  if (httpCode > 0)
  {
    Serial.printf("HTTP POST success, code: %d\n", httpCode);
  }
  else
  {
    Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
void controlLoop()
{
  float h = envSensor.readHumidity();
  float t = envSensor.readTemperature();

  // humidifier logic
  if (h < lowHum)
  {
    humidifier.turnOn();
    windowDrv.openToPercent(20.0f);
  }
  else if (h > highHum)
  {
    humidifier.turnOff();
    windowDrv.closeToPercent(0.0f);
  }

  // send data
  sendToServer("d1_mini", h, t);
}

void setup()
{
  Serial.begin(115200);

  Serial.printf("SSID: %s\n", WIFI_SSID);
  Serial.printf("PASS: %s\n", WIFI_PASS);

  // init drivers
  windowDrv.begin();
  humidifier.begin();
  envSensor.begin();

  // connect Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print('.');
  }
  Serial.println("\nWi-Fi connected");

  // OTA
  ArduinoOTA.begin();

  // schedule control loop every minute
  ticker.attach(60, controlLoop);
}

void loop()
{
  ArduinoOTA.handle();
  // Ticker interrupts run controlLoop automatically
}
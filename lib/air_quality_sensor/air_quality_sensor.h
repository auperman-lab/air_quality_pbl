#ifndef AirQualitySensor_h
#define AirQualitySensor_h

#include <Arduino.h>
#include <Wire.h>            // Required for I2C communication
#include <ScioSense_ENS16x.h> // Keep this for ENS160
#include <Adafruit_AHTX0.h>  // <<<<<< CHANGE THIS LINE BACK TO ADAFRUIT AHTX0

class AirQualitySensor {
public:
    // Constructor
    AirQualitySensor();

    // Initializes the sensors
    bool begin();

    // Reads and returns the ambient temperature from AHT21
    float getTemperature();

    // Reads and returns the ambient humidity from AHT21
    float getHumidity();

    // Reads and returns the TVOC equivalent from ENS160
    uint16_t getTVOC();

    // Reads and returns the eCO2 equivalent from ENS160
    uint16_t getECO2();

    // Reads and returns the Air Quality Index (AQI) from ENS160
    uint8_t getAQI();

    // Sets the ambient temperature for ENS160 compensation (optional, improves accuracy)
    void setENS160Compensation(float temperature, float humidity);

private:
    ENS160 ens160;
    Adafruit_AHTX0 aht21;    // <<<<<< CHANGE THIS LINE BACK TO ADAFRUIT AHTX0 object
    bool ens160_initialized = false;
    bool aht21_initialized = false;
};

#endif
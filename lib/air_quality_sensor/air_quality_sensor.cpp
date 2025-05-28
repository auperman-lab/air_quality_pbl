#include "air_quality_sensor.h"

AirQualitySensor::AirQualitySensor() {
    Wire.begin();
}

bool AirQualitySensor::begin() {
    if (aht21.begin()) {
        aht21_initialized = true;
        Serial.println("AHT21 sensor (using Adafruit AHTX0 lib) found and initialized.");
    } else {
        Serial.println("Could not find AHT21 sensor! Check wiring.");
    }

    ens160.begin(&Wire, 0x53);
                                                   
    if (ens160.init()) { // The init() function confirms connection and reads firmware
        ens160_initialized = true;
        Serial.println("ENS160 sensor (using ScioSense_ENS160 lib) found and initialized.");

        Result setResult = ens160.setOperatingMode(ENS16X_OPERATING_MODE_STANDARD);
        if (setResult == RESULT_OK) {
            Serial.println("ENS160 set to Standard Operating Mode.");
        } else {
            Serial.print("Failed to set ENS160 operating mode: ");
            Serial.println(setResult); // Prints a Result enum value
        }

    } else {
        Serial.println("Could not find ENS160 sensor! Check wiring.");
    }

    return ens160_initialized || aht21_initialized;
}

// Reads and returns the ambient temperature from AHT21
float AirQualitySensor::getTemperature() {
    if (!aht21_initialized) return NAN;

    sensors_event_t humidity_event, temp_event;
    if (aht21.getEvent(&humidity_event, &temp_event)) {
        return temp_event.temperature;
    }
    Serial.println("Failed to read temperature from AHT21.");
    return NAN;
}

// Reads and returns the ambient humidity from AHT21
float AirQualitySensor::getHumidity() {
    if (!aht21_initialized) return NAN;

    sensors_event_t humidity_event, temp_event;
    if (aht21.getEvent(&humidity_event, &temp_event)) {
        return humidity_event.relative_humidity;
    }
    Serial.println("Failed to read humidity from AHT21.");
    return NAN;
}

// Reads and returns the TVOC equivalent from ENS160
uint16_t AirQualitySensor::getTVOC() {
    if (!ens160_initialized) return 0;

    // Update sensor data
    Result updateResult = ens160.update(); // Update is needed to read new data
    if (updateResult != RESULT_OK) {
        // Serial.println("ENS160 update failed."); // Can be noisy
        return 0;
    }

    // Check if new data is available and sensor is in normal operation
    if (ens160.hasNewData() && ens160.getDeviceStatus() == ENS16X_DEVICE_STATUS_ACTIVE) {
        return ens160.getTvoc(); // Correct method for TVOC
    }
    // Serial.println("ENS160 data not ready or in error state for TVOC."); // Can be noisy
    return 0;
}

// Reads and returns the eCO2 equivalent from ENS160
uint16_t AirQualitySensor::getECO2() {
    if (!ens160_initialized) return 0;

    Result updateResult = ens160.update(); // Update is needed to read new data
    if (updateResult != RESULT_OK) {
        // Serial.println("ENS160 update failed."); // Can be noisy
        return 0;
    }

    if (ens160.hasNewData() && ens160.getDeviceStatus() == ENS16X_DEVICE_STATUS_ACTIVE) {
        return ens160.getEco2(); // Correct method for eCO2
    }
    // Serial.println("ENS160 data not ready or in error state for eCO2."); // Can be noisy
    return 0;
}

// Reads and returns the Air Quality Index (AQI) from ENS160
uint8_t AirQualitySensor::getAQI() {
    if (!ens160_initialized) return 0;

    Result updateResult = ens160.update(); // Update is needed to read new data
    if (updateResult != RESULT_OK) {
        // Serial.println("ENS160 update failed."); // Can be noisy
        return 0;
    }

    if (ens160.hasNewData() && ens160.getDeviceStatus() == ENS16X_DEVICE_STATUS_ACTIVE) {
        // The ScioSense library returns AQI as an enum or raw value from 1-5
        return ens160.getAirQualityIndex_UBA(); // Correct method for AQI
    }
    // Serial.println("ENS160 data not ready or in error state for AQI."); // Can be noisy
    return 0;
}

// Sets the ambient temperature and humidity for ENS160 compensation
void AirQualitySensor::setENS160Compensation(float temperature, float humidity) {
    if (ens160_initialized) {
        // ScioSense library expects temperature in Kelvin and humidity as relative humidity (0-100%).
        ens160.writeCompensation(temperature + 273.15, humidity);
    }
}
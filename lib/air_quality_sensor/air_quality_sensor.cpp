#include "air_quality_sensor.h"

// Constructor
AirQualitySensor::AirQualitySensor() {
    Wire.begin();
    aht21_initialized = false; // Initialize flags
    ens160_initialized = false; // Initialize flags
}

bool AirQualitySensor::begin() {
    Serial.println("AirQualitySensor::begin() called.");

    // --- AHT21 Initialization ---
    if (aht21.begin()) {
        aht21_initialized = true;
        Serial.println("AHT21 sensor (using Adafruit AHTX0 lib) found and initialized.");
    } else {
        Serial.println("ERROR: Could not find AHT21 sensor! Check wiring.");
    }

    // --- ENS160 Initialization ---
    // Make sure to pass the Wire object and address (0x53 or 0x52 based on your ADD pin)
    ens160.begin(&Wire, 0x53); 
                                                   
    if (ens160.init()) { // The init() function confirms connection and reads firmware
        ens160_initialized = true;
        Serial.print("ENS160 sensor (using ScioSense_ENS160 lib) found and initialized. Part ID: ");

        Result setResult = ens160.setOperatingMode(ENS16X_OPERATING_MODE_STANDARD);
        if (setResult == RESULT_OK) {
            Serial.println("ENS160 set to Standard Operating Mode.");
        } else {
            Serial.print("ERROR: Failed to set ENS160 operating mode: ");
            Serial.println(setResult); // Prints a Result enum value
        }

    } else {
        Serial.println("ERROR: Could not find ENS160 sensor! Check wiring/address (0x53).");
    }

    Serial.print("AirQualitySensor::begin() finished. ENS160 Init Status: ");
    Serial.println(ens160_initialized ? "Success" : "Failed");
    Serial.print("AirQualitySensor::begin() finished. AHT21 Init Status: ");
    Serial.println(aht21_initialized ? "Success" : "Failed");

    return ens160_initialized || aht21_initialized;
}

// Reads and returns the ambient temperature from AHT21
float AirQualitySensor::getTemperature() {
    if (!aht21_initialized) {
        // Serial.println("WARNING: AHT21 not initialized for temperature read."); // Too noisy
        return NAN;
    }

    sensors_event_t humidity_event, temp_event;
    if (aht21.getEvent(&humidity_event, &temp_event)) {
        // Serial.print("DEBUG: AHT21 Temp Read: "); Serial.println(temp_event.temperature); // Debugging
        return temp_event.temperature;
    }
    Serial.println("WARNING: Failed to read temperature from AHT21. Retrying next cycle.");
    return NAN;
}

// Reads and returns the ambient humidity from AHT21
float AirQualitySensor::getHumidity() {
    if (!aht21_initialized) {
        // Serial.println("WARNING: AHT21 not initialized for humidity read."); // Too noisy
        return NAN;
    }

    sensors_event_t humidity_event, temp_event;
    if (aht21.getEvent(&humidity_event, &temp_event)) {
        // Serial.print("DEBUG: AHT21 Hum Read: "); Serial.println(humidity_event.relative_humidity); // Debugging
        return humidity_event.relative_humidity;
    }
    Serial.println("WARNING: Failed to read humidity from AHT21. Retrying next cycle.");
    return NAN;
}

// Reads and returns the TVOC equivalent from ENS160
uint16_t AirQualitySensor::getTVOC() {
    if (!ens160_initialized) {
        // Serial.println("WARNING: ENS160 not initialized for TVOC read."); // Too noisy
        return 0;
    }

    // Always update sensor data before reading values
    Result updateResult = ens160.update(); 
    if (updateResult != RESULT_OK) {
        Serial.print("WARNING: ENS160 update failed for TVOC (Error: "); Serial.print(updateResult); Serial.println(").");
        return 0;
    }

    Serial.print("DEBUG: ENS160 Device Status: ");
    uint8_t dev_status = ens160.getDeviceStatus();
    switch (dev_status) {
        case ENS16X_DEVICE_STATUS_ACTIVE: Serial.print("Active Device Status (0x01)"); break;
        case ENS16X_DEVICE_STATUS_ERROR: Serial.print("Error Device Status (0x03)"); break;

        default: Serial.print("Unknown (0x"); Serial.print(dev_status, HEX); Serial.print(")"); break;
    }
    Serial.println();

    Serial.print("DEBUG: ENS160 Has New Data: ");
    Serial.println(ens160.hasNewData() ? "True" : "False");
    // --- END LOGGING ---

    // Check if new data is available and sensor is in normal operation state (not just device status)
    if (ens160.hasNewData()) { // Use op_status for better check
        return ens160.getTvoc(); 
    }
    Serial.println("INFO: ENS160 TVOC data not yet ready (still warming up/initializing, or no new data)."); 
    return 0;
}

// Reads and returns the eCO2 equivalent from ENS160
uint16_t AirQualitySensor::getECO2() {
    if (!ens160_initialized) {
        // Serial.println("WARNING: ENS160 not initialized for eCO2 read."); // Too noisy
        return 0;
    }

    // updateResult handled in getTVOC() - but ensuring a recent update for this call is good
    // However, calling update() multiple times in the same loop for each getX() is inefficient.
    // It's better to call update once at the start of sendSensorData() if possible,
    // but the library might refresh on each call. For now, keep it as is.
    Result updateResult = ens160.update(); 
    if (updateResult != RESULT_OK) {
        Serial.print("WARNING: ENS160 update failed for eCO2 (Error: "); Serial.print(updateResult); Serial.println(").");
        return 0;
    }

    Serial.println("INFO: ENS160 eCO2 data not yet ready (still warming up/initializing, or no new data)."); 
    return 0;
}

// Reads and returns the Air Quality Index (AQI) from ENS160
uint8_t AirQualitySensor::getAQI() {
    if (!ens160_initialized) {
        // Serial.println("WARNING: ENS160 not initialized for AQI read."); // Too noisy
        return 0;
    }

    Result updateResult = ens160.update(); 
    if (updateResult != RESULT_OK) {
        Serial.print("WARNING: ENS160 update failed for AQI (Error: "); Serial.print(updateResult); Serial.println(").");
        return 0;
    }

    Serial.println("INFO: ENS160 AQI data not yet ready (still warming up/initializing, or no new data)."); 
    return 0;
}

// Sets the ambient temperature and humidity for ENS160 compensation
void AirQualitySensor::setENS160Compensation(float temperature, float humidity) {
    if (ens160_initialized) {
        Serial.print("DEBUG: Setting ENS160 compensation Temp: "); Serial.print(temperature); 
        Serial.print("C, Hum: "); Serial.print(humidity); Serial.println("%");
        ens160.writeCompensation(temperature + 273.15, humidity); // ENS160 expects Kelvin
    } else {
        Serial.println("WARNING: ENS160 not initialized, cannot set compensation.");
    }
}
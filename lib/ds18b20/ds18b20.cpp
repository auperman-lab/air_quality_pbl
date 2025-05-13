#include "ds18b20.h"

DS18B20_Reader::DS18B20_Reader(int pin) : oneWire(pin), ds(&oneWire) {
}

void DS18B20_Reader::ds_setup(){
    ds.begin();
};



float DS18B20_Reader::getTemperature() {
    Serial.println("Requesting DS18B20 temperature...");
    ds.requestTemperatures();
    float temp = ds.getTempCByIndex(0);
    
    if (temp == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: DS18B20 not found!");
        return -127.0;  // Indicates sensor failure
    }
    
    Serial.print("DS18B20 Temperature: ");
    Serial.println(temp);
    return temp;
}

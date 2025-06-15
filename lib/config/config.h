#pragma once

// These will be provided by build_flags
#ifndef WIFI_SSID
#error "WIFI_SSID not defined"
#endif

#ifndef WIFI_PASS
#error "WIFI_PASS not defined"
#endif

#ifndef NOTIFY_EMAIL
#error "NOTIFY_EMAIL not defined"
#endif

// Use macros directly when calling WiFi.begin()

constexpr float TIME_OPEN_MAIN = 45.0f;
constexpr float TIME_OPEN_HANDLE = 15.0f;
constexpr uint32_t SLEEP_INTERVAL = 10 * 60; // seconds

constexpr uint8_t PIN_MAIN_OPEN = 14;
constexpr uint8_t PIN_MAIN_CLOSE = 10;
constexpr uint8_t PIN_HAND_OPEN = 13;
constexpr uint8_t PIN_HAND_CLOSE = 15;

constexpr uint8_t PIN_END_MAIN_O = 12;
constexpr uint8_t PIN_END_MAIN_C = 3;
constexpr uint8_t PIN_END_HAND_O = 5;
constexpr uint8_t PIN_END_HAND_C = 4;

constexpr uint8_t PIN_HUMIDIFIER = 2;
constexpr uint8_t PIN_DHT_DATA = 3;
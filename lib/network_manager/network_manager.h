#pragma once
#include <BlynkSimpleEsp8266.h>
#include <ArduinoOTA.h>

class NetworkManager
{
public:
    NetworkManager(const char *auth, const char *ssid, const char *pass);
    void begin();
    void run();
    BlynkTimer &timer();

private:
    const char *_auth;
    const char *_ssid;
    const char *_pass;
    BlynkTimer _timer;
};
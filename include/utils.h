#ifndef I2S_BELL_UTILS_H
#define I2S_BELL_UTILS_H
#include <Arduino.h>
#include <WiFi.h>

struct Settings
{
    // time
    uint32_t gmt_offset_s = 0;
    uint32_t dst_offset_s = 0;
    // audio
    float audio_volume = 0.f;
    // network
    String hostname = "";
    bool staticIP = false;
    IPAddress local;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress primaryDNS;   //optional
    IPAddress secondaryDNS; //optional 
};

enum DisplayState {
    TIME,
    WIFI_CONNECT,
    WIFI_CONNECTED,
    WIFI_PORTAL,
    SYNC,
    SD_ERR
};

String dowName(int dow);

#endif // I2S_BELL_UTILS_H
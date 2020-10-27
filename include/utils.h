#ifndef I2S_BELL_UTILS_H
#define I2S_BELL_UTILS_H
#include <Arduino.h>

enum DisplayState {
    TIME,
    WIFI_CONNECT,
    WIFI_CONNECTED,
    WIFI_PORTAL,
    SYNC,
    SD_ERR
};

String dowName(int dow);

#endif //I2S_BELL_UTILS_H
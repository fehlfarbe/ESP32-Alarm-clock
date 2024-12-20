#ifndef MP3ALARM_UTILS_H
#define MP3ALARM_UTILS_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Array.h>
#include <FS.h>

#define MAX_DOW 7

enum DisplayState {
    TIME,
    WIFI_CONNECT,
    WIFI_CONNECTED,
    WIFI_PORTAL,
    SYNC,
    SD_ERR,
    SD_ERR_NO_SD,
    FS_ERR
};

String dowName(uint8_t dow);
typedef Array<uint8_t, MAX_DOW> DaysOfWeek;

bool readJSONFile(fs::FS& fs, String filePath, JsonDocument& doc, DeserializationError& error);
bool writeJSONFile(fs::FS& fs, String filePath, JsonDocument& doc);

#endif // MP3ALARM_UTILS_H
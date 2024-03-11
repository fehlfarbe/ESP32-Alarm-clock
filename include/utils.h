#ifndef MP3ALARM_UTILS_H
#define MP3ALARM_UTILS_H
#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Array.h>

#define MAX_DOW 7

enum DisplayState {
    TIME,
    WIFI_CONNECT,
    WIFI_CONNECTED,
    WIFI_PORTAL,
    SYNC,
    SD_ERR
};

String dowName(uint8_t dow);
typedef Array<uint8_t, MAX_DOW> DaysOfWeek ;

bool readJSONFile(fs::FS &fs, String filePath, DynamicJsonDocument &doc, DeserializationError &error);
bool writeJSONFile(fs::FS &fs, String filePath, DynamicJsonDocument &doc);

#endif // MP3ALARM_UTILS_H
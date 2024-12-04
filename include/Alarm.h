#ifndef ALARM_H
#define ALARM_H
#include "MusicStream.h"
#include "utils.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Array.h>
#include <TimeLib.h>
#include <time.h>

#define TIME_MAX_SEC 604800

namespace AlarmClock {
class Alarm {
public:
    Alarm();
    Alarm(String name, DaysOfWeek dow, uint8_t hour, uint8_t minute, MusicStream stream,
        bool enabled, float volume = .5f);
    ~Alarm();

    String toString();
    void toJSON(JsonObject& alarm);

    bool isEnabled();
    String getName() const;
    DaysOfWeek& getDow();
    uint8_t getHour() const;
    uint8_t getMinute() const;
    float getVolume() const;
    MusicStream& getStream();

private:
    bool enabled;
    String name;
    DaysOfWeek dow;
    uint8_t hour;
    uint8_t minute;
    float volume;
    MusicStream stream;
};
}

#endif // ALARM_H
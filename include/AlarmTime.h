#ifndef ALARMTIME_H
#define ALARMTIME_H
#include "Alarm.h"
#include "MusicStream.h"
#include "utils.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <time.h>

namespace AlarmClock {
class AlarmTime {
public:
    AlarmTime();
    AlarmTime(Alarm alarm, uint8_t dow);
    ~AlarmTime();

    String toString();
    int toSec() const;
    int differenceSec(const struct tm timeinfo);

    MusicStream& getStream();
    String getName();
    float getVolume();

    bool operator<(const AlarmTime& b) const;
    bool operator>(const AlarmTime& b) const;
    bool operator<(const struct tm timeinfo) const;
    bool operator>(const struct tm timeinfo) const;

private:
    Alarm alarm;
    uint8_t dow;
    float volume;

    int tmToSec(const struct tm t) const;
};
}

#endif // ALARMTIME_H
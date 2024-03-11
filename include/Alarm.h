#ifndef ALARM_H
#define ALARM_H
#include <Arduino.h>
#include <time.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include <Array.h>
#include "utils.h"
#include "MusicStream.h"

#define TIME_MAX_SEC 604800

namespace AlarmClock
{
    class Alarm
    {
    public:
        Alarm();
        Alarm(String name, Array<int, MAX_DOW> dow, int hour, int minute, MusicStream stream);
        ~Alarm();

        String toString();
        void toJSON(JsonObject &alarm);

        MusicStream &getStream();

    public:
        bool enabled;
        String name;
        DaysOfWeek dow;
        int hour;
        int minute;
        MusicStream stream;
    };
}

#endif // ALARM_H
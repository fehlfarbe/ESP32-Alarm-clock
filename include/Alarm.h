#ifndef ALARMSETTINGS_H
#define ALARMSETTINGS_H
#include <Arduino.h>
#include <time.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include "utils.h"
#include "MusicStream.h"

#define TIME_MAX_SEC 604800

namespace AlarmClock
{
    class Alarm
    {
    public:
        Alarm();
        Alarm(String name, int dow, int hour, int minute, MusicStream stream);
        ~Alarm();

        String toString();
        JsonObject toJSON();
        int toSec() const;
        int differenceSec(const struct tm timeinfo);

        MusicStream &getStream();

        bool operator<(const Alarm &b);
        bool operator>(const Alarm &b);
        bool operator<(const struct tm timeinfo);
        bool operator>(const struct tm timeinfo);

    public:
        String name;
        int dow;
        int hour;
        int minute;
        MusicStream stream;

    private:
        int tmToSec(const struct tm t);
    };
}

#endif // ALARMSETTINGS_H
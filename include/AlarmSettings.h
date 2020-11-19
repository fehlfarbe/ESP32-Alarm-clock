#ifndef ALARMSETTINGS_H
#define ALARMSETTINGS_H
#include <Arduino.h>
#include <time.h>
#include <Time.h>
#include "utils.h"
#include "MusicStream.h"

#define TIME_MAX_SEC 604800

class AlarmSettings {
    public:
        AlarmSettings();
        AlarmSettings(String name, int dow, int hour, int minute, MusicStream stream);
        ~AlarmSettings();

        String toString();
        int toSec() const;
        int differenceSec(const struct tm timeinfo);

        MusicStream& getStream();

        bool operator <(const AlarmSettings &b);
        bool operator >(const AlarmSettings &b);
        bool operator <(const struct tm timeinfo);
        bool operator >(const struct tm timeinfo);     
    
    public:
        String name;
        int dow;
        int hour;
        int minute;
        MusicStream stream;

    private:
        int tmToSec(const struct tm t);
        
};


#endif //ALARMSETTINGS_H
#ifndef I2S_BELL_H
#define I2S_BELL_H
#include <Arduino.h>
#include <time.h>
#include <Time.h>
#include "utils.h"

#define TIME_MAX_SEC 604800

class AlarmSettings {
    public:
        AlarmSettings();
        AlarmSettings(String name, int dow, int hour, int minute, String file);
        ~AlarmSettings();

        String toString();
        int toSec() const;
        int differenceSec(const struct tm timeinfo);

        bool operator <(const AlarmSettings &b);
        bool operator >(const AlarmSettings &b);
        bool operator <(const struct tm timeinfo);
        bool operator >(const struct tm timeinfo);     
    
    public:
        String name;
        int dow;
        int hour;
        int minute;
        String file;

    private:
        int tmToSec(const struct tm t);
        
};


#endif //I2S_BELL_H
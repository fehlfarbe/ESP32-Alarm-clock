#include "AlarmTime.h"

namespace AlarmClock
{
    AlarmTime::AlarmTime()
    {
    }

    AlarmTime::AlarmTime(Alarm alarm, uint8_t dow) : alarm(alarm), dow(dow)
    {
    }

    AlarmTime::~AlarmTime()
    {
    }

    String AlarmTime::toString()
    {
        String s;
        s += alarm.name + ": " + dowName(dow) + " " + alarm.hour + ":" + alarm.minute + " (" + alarm.stream.getURL() + ")";
        return s;
    }

    int AlarmTime::toSec() const
    {
        return (dow * 86400) + (alarm.hour * 3600) + alarm.minute * 60;
    }

    int AlarmTime::differenceSec(const struct tm timeinfo)
    {
        int diff = abs(toSec() - tmToSec(timeinfo));
        if (diff > TIME_MAX_SEC / 2)
        {
            return TIME_MAX_SEC - diff;
        }
        return diff;
    }

    MusicStream &AlarmTime::getStream()
    {
        return alarm.stream;
    }

    String AlarmTime::getName()
    {
        return alarm.name;
    }

    bool AlarmTime::operator<(const AlarmTime &b) const
    {
        return toSec() < b.toSec();
        // check if dow is smaller
        // if(dow != b.dow){
        //     return dow < b.dow;
        // }

        // // check if hour is smaller
        // if(hour != b.hour){
        //     return hour < b.hour;
        // }

        // // check if minute is smaller
        // return minute < b.minute;
    }

    bool AlarmTime::operator>(const AlarmTime &b) const
    {
        return this->toSec() > b.toSec();
    }

    bool AlarmTime::operator<(const struct tm timeinfo) const
    {
        return this->toSec() < tmToSec(timeinfo);
        // check if dow is smaller
        // if(dow != timeinfo.tm_wday){
        //     return dow < timeinfo.tm_wday;
        // }

        // // check if hour is smaller
        // if(hour != timeinfo.tm_hour){
        //     return hour < timeinfo.tm_hour;
        // }

        // // check if minute is smaller
        // return minute < timeinfo.tm_min;
    }

    bool AlarmTime::operator>(const struct tm timeinfo) const
    {
        return this->toSec() > tmToSec(timeinfo);
    }

    int AlarmTime::tmToSec(const struct tm t) const
    {
        return (t.tm_wday * 86400) + (t.tm_hour * 3600) + (t.tm_min * 60) + t.tm_sec;
    }
}
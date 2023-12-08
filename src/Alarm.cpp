#include "Alarm.h"

namespace AlarmClock
{
    Alarm::Alarm()
    {
        name = "null";
    }
    Alarm::Alarm(String name, int dow, int hour, int minute, MusicStream stream) : name(name), dow(dow), hour(hour), minute(minute), stream(stream)
    {
    }

    Alarm::~Alarm()
    {
    }

    String Alarm::toString()
    {
        String s;
        s += name + ": " + dowName(dow) + " " + hour + ":" + minute + " (" + stream.getURL() + ")";
        return s;
    }

    JsonObject Alarm::toJSON() {
        JsonObject alarm;
        // read music type first
        alarm["name"] = name;
        alarm["hour"] = hour;
        alarm["minute"] = minute;
        alarm["dow"] = dow;
        alarm["type"] = stream.typeToString(stream.getType());

        switch(stream.getType()){
            case MusicType::FM:
            alarm["file"] = stream.getFMFrequency();
            break;
            case MusicType::FILESYSTEM:
            case MusicType::STREAM:
            alarm["file"] = stream.getURL();
            break;
            default:
            Serial.printf("Wrong MusicType %s for alarm %s\n", stream.typeToString(stream.getType()).c_str(), name.c_str());
        }

        return alarm;
    }

    int Alarm::toSec() const
    {
        return (dow * 86400) + (hour * 3600) + minute * 60;
    }

    int Alarm::differenceSec(const struct tm timeinfo)
    {
        int diff = abs(toSec() - tmToSec(timeinfo));
        if (diff > TIME_MAX_SEC / 2)
        {
            return TIME_MAX_SEC - diff;
        }
        return diff;
    }

    MusicStream &Alarm::getStream()
    {
        return stream;
    }

    bool Alarm::operator<(const Alarm &b)
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

    bool Alarm::operator>(const Alarm &b)
    {
        return this->toSec() > b.toSec();
    }

    bool Alarm::operator<(const struct tm timeinfo)
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

    bool Alarm::operator>(const struct tm timeinfo)
    {
        return this->toSec() > tmToSec(timeinfo);
    }

    int Alarm::tmToSec(const struct tm t)
    {
        return (t.tm_wday * 86400) + (t.tm_hour * 3600) + (t.tm_min * 60) + t.tm_sec;
    }
}
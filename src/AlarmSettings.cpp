#include "AlarmSettings.h"

AlarmSettings::AlarmSettings(){
    name = "None...";
    
}
AlarmSettings::AlarmSettings(String name, int dow, int hour, int minute, MusicStream stream) : 
name(name), dow(dow), hour(hour), minute(minute), stream(stream) {
    
}

AlarmSettings::~AlarmSettings(){
    
}

String AlarmSettings::toString(){
    String s;
    s += name + ": " + dowName(dow) + " " + hour + ":" + minute + " (" + stream.getURL() + ")";
    return s;
}

int AlarmSettings::toSec() const{
    return (dow * 86400) + (hour * 3600) + minute * 60;
}

int AlarmSettings::differenceSec(const struct tm timeinfo){
    int diff = abs(toSec() - tmToSec(timeinfo));
    if(diff > TIME_MAX_SEC / 2){
        return TIME_MAX_SEC - diff;
    }
    return diff;
}

MusicStream& AlarmSettings::getStream(){
    return stream;
}

bool AlarmSettings::operator <(const AlarmSettings &b){
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

bool AlarmSettings::operator >(const AlarmSettings &b){
    return this->toSec() > b.toSec();
}

bool AlarmSettings::operator <(const struct tm timeinfo){
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

bool AlarmSettings::operator >(const struct tm timeinfo){
    return this->toSec() > tmToSec(timeinfo);
}

int AlarmSettings::tmToSec(const struct tm t){
    return (t.tm_wday * 86400) + (t.tm_hour * 3600) + (t.tm_min * 60) + t.tm_sec;
}
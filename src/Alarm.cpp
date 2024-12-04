#include "Alarm.h"

namespace AlarmClock
{
    Alarm::Alarm()
    {
        name = "null";
    }
    Alarm::Alarm(String name, DaysOfWeek dow, uint8_t hour, uint8_t minute, MusicStream stream, bool enabled, float volume) : name(name), dow(dow), hour(hour), minute(minute), stream(stream), enabled(enabled), volume(volume)
    {
    }

    Alarm::~Alarm()
    {
    }

    String Alarm::toString()
    {
        String s;
        String dowNames = "";
        for (const auto &day : dow)
        {
            dowNames += dowName(day) + ", ";
        }

        if (enabled)
        {
            s += "[x] ";
        }
        else
        {
            s += "[ ] ";
        }
        s += name + ": " + dowNames + " " + hour + ":" + minute + " (" + stream.getURL() + ") at volume " + volume;
        return s;
    }

    void Alarm::toJSON(JsonObject &alarm)
    {
        // read music type first
        alarm["name"] = name;
        alarm["hour"] = hour;
        alarm["minute"] = minute;
        alarm["type"] = stream.typeToString(stream.getType());
        auto dowJSON = alarm.createNestedArray("dow");

        for (auto &day : dow)
        {
            dowJSON.add(day);
        }

        switch (stream.getType())
        {
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
        alarm["enabled"] = enabled;
        alarm["volume"] = volume;
    }
    bool Alarm::isEnabled()
    {
        return enabled;
    }
    String Alarm::getName() const
    {
        return name;
    }
    DaysOfWeek &Alarm::getDow()
    {
        return dow;
    }
    uint8_t Alarm::getHour() const
    {
        return hour;
    }
    uint8_t Alarm::getMinute() const
    {
        return minute;
    }

    float Alarm::getVolume() const
    {
        return volume;
    }

    MusicStream &Alarm::getStream()
    {
        return stream;
    }
}
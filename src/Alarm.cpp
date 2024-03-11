#include "Alarm.h"

namespace AlarmClock
{
    Alarm::Alarm()
    {
        name = "null";
    }
    Alarm::Alarm(String name, Array<int, MAX_DOW> dow, int hour, int minute, MusicStream stream) : name(name), dow(dow), hour(hour), minute(minute), stream(stream)
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
        s += name + ": " + dowNames + " " + hour + ":" + minute + " (" + stream.getURL() + ")";
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
    }
}
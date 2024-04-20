#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ALARMS 100

#include <FS.h>
#include <ArduinoJson.h>
#include <Array.h>
#include "utils.h"
#include "Alarm.h"
#include "AlarmTime.h"

namespace AlarmClock
{
    struct GlobalSettings
    {
        // timezone
        String tz = "Etc/UTC|UTC0";
        // audio
        float audio_volume = 0.5f;
        // network
        String hostname = "esp32alarm";
        bool isStaticIPEnabled = false;
        IPAddress local;
        IPAddress gateway;
        IPAddress subnet;
        IPAddress primaryDNS;   // optional
        IPAddress secondaryDNS; // optional
    };

    class Config
    {
    public:
        Config();

        bool LoadFromPath(fs::FS &fs, const String &path);
        bool Load(JsonDocument &doc);

        void Save(fs::FS &fs, const String &path);
        void Save(JsonDocument &doc);

        GlobalSettings &GetGlobalConfig();
        Array<AlarmTime, MAX_ALARMS> &getAalarmTimes();
        AlarmTime &GetNextAlarmTime();
        void SortAlarmTimes();
        void SelectNextAlarmTime(struct tm &timeinfo);
        void IncrementNextAlarmTime();

        String GetPOSIXTimezone();

    private:
        GlobalSettings global;
        Array<Alarm, MAX_ALARMS> alarms;
        Array<AlarmTime, MAX_ALARMS> alarmTimes;
        size_t alarmNext = 0;
    };
}
#endif // CONFIG_H
#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ALARMS 100
#define JSON_BUFFER 2048 * 2

#include <FS.h>
#include <ArduinoJson.h>
#include "utils.h"
#include "Alarm.h"

namespace AlarmClock
{
    struct GlobalSettings
    {
        // time
        uint32_t gmt_offset_s = 0;
        uint32_t dst_offset_s = 0;
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
        bool Load(DynamicJsonDocument &doc);

        void Save(fs::FS &fs, const String &path);
        void Save(DynamicJsonDocument &doc);

        GlobalSettings &GetGlobalConfig();

    public:
        GlobalSettings global;
        Alarm alarms[MAX_ALARMS];

        size_t alarmSize = 0;
        size_t alarmNext = 0;
    };
}
#endif // CONFIG_H
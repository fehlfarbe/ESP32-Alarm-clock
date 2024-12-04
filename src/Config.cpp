#include "Config.h"
#include <ArduinoSort.h>

namespace AlarmClock {
Config::Config() { }

bool Config::LoadFromPath(fs::FS& fs, const String& path)
{
    File configFile = fs.open(path, FILE_READ, false);
    String json = configFile.readString();
    configFile.close();
    Serial.println(json);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        Serial.println("----- parseObject() for config.json failed -----");
        Serial.println(error.c_str());
        return false;
    }

    return Load(doc);
}

bool Config::Load(JsonDocument& doc)
{

    // load general settings
    auto general = doc["general"];

    if (general.containsKey("audio_volume")) {
        global.audio_volume = general["audio_volume"].as<float>();
    }

    if (general.containsKey("tz")) {
        global.tz = general["tz"].as<String>();
    }

    // load network settings
    auto network = doc["network"];
    if (network.containsKey("hostname")) {
        global.hostname = network["hostname"].as<String>();
    } else {
        // set default hostname
        char hostname[20];
        uint64_t chipid = ESP.getEfuseMac();
        sprintf(hostname, "esp32-%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);
        global.hostname = String(hostname);
    }
    if (network.containsKey("static_ip_enabled")) {
        global.isStaticIPEnabled = network["static_ip_enabled"].as<bool>();
    }
    if (network.containsKey("static_ip")) {
        global.local.fromString(network["static_ip"].as<String>());
    }
    if (network.containsKey("subnet")) {
        global.subnet.fromString(network["subnet"].as<String>());
    }
    if (network.containsKey("gateway")) {
        global.gateway.fromString(network["gateway"].as<String>());
    }
    if (network.containsKey("primary_dns")) {
        global.primaryDNS.fromString(network["primary_dns"].as<String>());
    }
    if (network.containsKey("secondary_dns")) {
        global.secondaryDNS.fromString(network["secondary_dns"].as<String>());
    }

    // load alarm settings
    JsonArray alarmsJSON = doc["alarms"];
    size_t i = 0;
    alarms.clear();
    alarmTimes.clear();
    for (auto a : alarmsJSON) {
        if (i == MAX_ALARMS) {
            Serial.println("Maximum number of alarms reached!");
            break;
        }
        // read music type first
        String music_name = a["music"];
        String music_url = a["file"];
        auto music_type = AlarmClock::MusicStream::stringToType(a["type"]);
        auto stream = AlarmClock::MusicStream(music_name, music_url, music_type);

        // alarm params
        String name = a["name"];
        String file = a["file"];
        int hour = (int)a["hour"];
        int minute = (int)a["minute"];
        DaysOfWeek dow;
        for (const auto& d : a["dow"].as<JsonArray>()) {
            int day = (int)d;
            dow.push_back(d.as<int>());
        }
        bool enabled = a["enabled"].as<bool>();
        float volume = a["volume"] | 0.11;
        volume = min(1.f, max(volume, 0.f));

        // push to list
        AlarmClock::Alarm alarm(name, dow, hour, minute, stream, enabled, volume);
        alarms.push_back(alarm);
        Serial.printf("Alarm %s\n", alarm.toString().c_str());
        if (alarm.isEnabled()) {
            for (const auto& day : alarm.getDow()) {
                if (alarmTimes.full()) {
                    Serial.println("Maximum number of alarm times reached!");
                    break;
                }
                alarmTimes.push_back(AlarmTime(alarm, day));
            }
        }
    }

    return true;
}

void Config::Save(fs::FS& fs, const String& path)
{
    JsonDocument doc;
    serializeJsonPretty(doc, Serial);
    Save(doc);
    serializeJsonPretty(doc, Serial);
    writeJSONFile(fs, path, doc);
}

void Config::Save(JsonDocument& doc)
{
    // save alarms
    auto alarmsJSON = doc["alarms"].to<JsonArray>();
    for (auto& alarm : alarms) {
        auto alarmJSON = alarmsJSON.add<JsonObject>();
        alarm.toJSON(alarmJSON);
    }

    // save general settings
    auto generalJSON = doc["general"].to<JsonObject>();
    generalJSON["audio_volume"] = global.audio_volume;
    generalJSON["tz"] = global.tz;

    // save network settings
    auto networkJSON = doc["network"].to<JsonObject>();
    networkJSON["hostname"] = global.hostname;
    networkJSON["static_ip_enabled"] = global.isStaticIPEnabled;
    networkJSON["static_ip"] = global.local;
    networkJSON["subnet"] = global.subnet;
    networkJSON["gateway"] = global.gateway;
    networkJSON["primary_dns"] = global.primaryDNS;
    networkJSON["secondary_dns"] = global.secondaryDNS;
}

GlobalSettings& Config::GetGlobalConfig() { return global; }

Array<AlarmTime, MAX_ALARMS>& Config::getAalarmTimes() { return alarmTimes; }

AlarmTime& Config::GetNextAlarmTime() { return alarmTimes[alarmNext]; }

void Config::SortAlarmTimes()
{
    // sort alarms by date
    sortArray(alarmTimes.data(), alarmTimes.size());
}

void Config::SelectNextAlarmTime(tm& timeinfo)
{
    bool selected = false;
    for (size_t i = 0; i < alarmTimes.size(); i++) {
        if (alarmTimes[i] > timeinfo) {
            alarmNext = i;
            selected = true;
            break;
        }
    }

    // if no alarm time is bigger, alarm is the first one in the new week
    if (!selected) {
        alarmNext = 0;
    }
}

void Config::IncrementNextAlarmTime() { alarmNext = (alarmNext + 1) % alarmTimes.size(); }

String Config::GetPOSIXTimezone()
{
    auto idx = global.tz.indexOf("|");
    return global.tz.substring(idx + 1);
}
}
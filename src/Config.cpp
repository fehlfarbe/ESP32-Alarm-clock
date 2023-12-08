#include "Config.h"

namespace AlarmClock
{
    Config::Config()
    {
    }

    bool Config::LoadFromPath(fs::FS &fs, const String &path)
    {
        File configFile = fs.open(path, FILE_READ, false);
        String json = configFile.readString();
        configFile.close();
        Serial.println(json);
        DynamicJsonDocument doc(JSON_BUFFER);
        DeserializationError error = deserializeJson(doc, json);
        if (error)
        {
            Serial.println("----- parseObject() for config.json failed -----");
            Serial.println(error.c_str());
            return false;
        }

        return Load(doc);
    }

    bool Config::Load(DynamicJsonDocument &doc)
    {
        // load alarm settings
        JsonArray alarmsJSON = doc["alarms"];
        size_t i = 0;
        for (auto a : alarmsJSON)
        {
            if (i == MAX_ALARMS)
            {
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
            int min = (int)a["minute"];
            JsonArray dow = a["dow"];

            for (const auto &d : dow)
            {
                int day = (int)d;
                Serial.printf("Alarm %s %s %02d:%02d %s\n", name.c_str(), dowName(day).c_str(), hour, min, file.c_str());
                alarms[i++] = AlarmClock::Alarm(name, day, hour, min, stream);
            }
        }
        alarmSize = i;

        // load general settings
        auto general = doc["general"];

        if (general.containsKey("audio_volume"))
        {
            global.audio_volume = general["audio_volume"].as<float>();
        }

        if (general.containsKey("gmt_offset"))
        {
            global.gmt_offset_s = (uint32_t)general["gmt_offset"];
        }

        if (general.containsKey("dst_offset"))
        {
            global.dst_offset_s = (uint32_t)general["dst_offset"];
        }

        // load network settings
        auto network = doc["network"];
        if (network.containsKey("hostname"))
        {
            global.hostname = network["hostname"].as<String>();
        }
        else
        {
            // set default hostname
            char hostname[20];
            uint64_t chipid = ESP.getEfuseMac();
            sprintf(hostname, "esp32-%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);
            global.hostname = String(hostname);
        }
        if (network.containsKey("static_ip_enabled"))
        {
            global.isStaticIPEnabled = network["static_ip_enabled"].as<bool>();
        }
        if (network.containsKey("static_ip"))
        {
            global.local.fromString(network["static_ip"].as<String>());
        }
        if (network.containsKey("subnet"))
        {
            global.subnet.fromString(network["subnet"].as<String>());
        }
        if (network.containsKey("gateway"))
        {
            global.gateway.fromString(network["gateway"].as<String>());
        }
        if (network.containsKey("primary_dns"))
        {
            global.primaryDNS.fromString(network["primary_dns"].as<String>());
        }
        if (network.containsKey("secondary_dns"))
        {
            global.secondaryDNS.fromString(network["secondary_dns"].as<String>());
        }

        return true;
    }

    void Config::Save(fs::FS &fs, const String &path)
    {
        DynamicJsonDocument doc(JSON_BUFFER);
        serializeJsonPretty(doc, Serial);
        Save(doc);
        serializeJsonPretty(doc, Serial);
        writeJSONFile(fs, path, doc);
    }

    void Config::Save(DynamicJsonDocument &doc)
    {
        // save alarms
        auto alarmsJSON = doc.createNestedArray("alarms");
        for (size_t i = 0; i < alarmSize; i++)
        {
            alarmsJSON.add(alarms[i].toJSON());
        }

        // save general settings
        auto generalJSON = doc.createNestedObject("general");
        generalJSON["audio_volume"] = global.audio_volume;
        generalJSON["gmt_offset"] = global.gmt_offset_s;
        generalJSON["dst_offset"] = global.dst_offset_s;

        // save network settings
        auto networkJSON = doc.createNestedObject("network");
        networkJSON["hostname"] = global.hostname;
        networkJSON["static_ip_enabled"] = global.isStaticIPEnabled;
        networkJSON["static_ip"] = global.local;
        networkJSON["subnet"] = global.subnet;
        networkJSON["gateway"] = global.gateway;
        networkJSON["primary_dns"] = global.primaryDNS;
        networkJSON["secondary_dns"] = global.secondaryDNS;
    }

    GlobalSettings &Config::GetGlobalConfig()
    {
        return global;
    }
}
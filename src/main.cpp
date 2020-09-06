#include <Arduino.h>
#include <Esp.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <time.h>
#include <Time.h>
#include <ArduinoJson.h>
#include <Audio.h>
#include <ArduinoSort.h>
#include <FS.h>
#include <LITTLEFS.h>
#include <SD.h>
#include <SPI.h>
#include <Int64String.h>

#include "AlarmSettings.h"
#include "utils.h"

// Digital I/O used
#define I2S_DOUT 25
#define I2S_BCLK 4
#define I2S_LRC 27

#define BTN_PLAY 14
#define BTN_WIFI_RESET 23
#define LED_BUILTIN 5

#define JSON_BUFFER 4096

// Global variables
Audio audio;

// settings
uint32_t time_offset_s = 0;
uint8_t audio_volume = 0;

// alarms
#define MAX_ALARMS 150
AlarmSettings alarms[MAX_ALARMS];
size_t alarms_size = 0;
size_t alarms_next = 0;

// data sources
fs::FS fsSongs = SD;
fs::FS fsConfig = SD;
fs::FS fsWWW = LITTLEFS;

//WebServer
AsyncWebServer server(80);
DNSServer dns;
File fsUploadFile;

// WiFi
AsyncWiFiManager wifiManager(&server, &dns);

// function declarations
void loadSettings(fs::FS &fs);
void nextAlarm();
void printAlarms();
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
time_t getNtpTime();
void timerCallback();
void printTime(struct tm);
bool compareAlarm(AlarmSettings first, AlarmSettings second);
bool checkPlayAlarm();

bool readJSONFile(fs::FS &fs, String file, DynamicJsonDocument &doc, DeserializationError &error);
bool writeJSONFile(fs::FS &fs, String file, DynamicJsonDocument &doc);

void handleAPIConfigUpdate(AsyncWebServerRequest *request);
void handleAPIConfig(AsyncWebServerRequest *request);
void handleAPISongs(AsyncWebServerRequest *request);
void handleAPISongsUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void handleAPIPlayback(AsyncWebServerRequest *request);
void handleAPIState(AsyncWebServerRequest *request);

// setup
void setup()
{

    // setup pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(BTN_WIFI_RESET, INPUT_PULLUP);
    // ledcSetup(0, 5000, 8);
    // ledcAttachPin(LED_BUILTIN, 0);

    // setup serial
    Serial.begin(115200);

    // setup I2S audio
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(audio_volume); // 0...21
    audio.stopSong();

    // Initialize LITTLEFS
    if (!LITTLEFS.begin())
    {
        Serial.println("An Error has occurred while mounting LITTLEFS");
    }

    Serial.println("Files on flash:");
    listDir(LITTLEFS, "/", 5);

    // Initialize SD
    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
    }
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
    }
    Serial.println("Files on SD:");
    listDir(SD, "/", 5);

    // load config
    loadSettings(fsConfig);

    // connect WiFi
    digitalWrite(LED_BUILTIN, LOW);
    // if (digitalRead(BTN_WIFI_RESET) == LOW)
    // {
    //     Serial.println("Reset WiFi setting...");
    //     wifiManager.resetSettings();
    // }
    wifiManager.setConnectTimeout(10);
    if (!wifiManager.autoConnect())
    {
        Serial.println("failed to connect, we should reset as see if it connects");
    }

    digitalWrite(LED_BUILTIN, HIGH);

    // print network settings
    Serial.println(WiFi.localIP().toString());

    // setup mDNS
    char hostname[20];
    uint64_t chipid = ESP.getEfuseMac();
    sprintf(hostname, "esp32-%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);
    if (!MDNS.begin(hostname))
    {
        Serial.println("Error setting up MDNS responder!");
    }
    else
    {
        Serial.println("mDNS responder started");
    }

    // sync time
    setSyncProvider(getNtpTime);

    // print current time
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        Serial.printf("Current time: ");
        printTime(timeinfo);
        Serial.println();
    }

    // sort alarms by date
    sortArray(alarms, alarms_size, compareAlarm);
    Serial.println("Alarms after sort:");
    printAlarms();
    // select next alarm
    nextAlarm();

    // HTTP Server
    server.begin();
    server.serveStatic("/", fsWWW, "/www/");
    server.on("/api/config", handleAPIConfig);
    server.on("/api/config/update", HTTP_POST, handleAPIConfigUpdate);
    server.on("/api/state", handleAPIState);
    server.on("/api/songs", handleAPISongs);
    server.on("/api/songs/update", HTTP_POST, handleAPISongs);
    server.on(
        "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200);
        },
        handleAPISongsUpload);
    server.on("/api/playback", handleAPIPlayback);

    // setup alarm
    // ToDo: for debug set alarm to current time + 10s
    // if (getLocalTime(&timeinfo)){
    //     Serial.printf("Setting alarm to %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec+8);
    //     Alarm.alarmRepeat(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec+10, timerCallback);
    // }

    // open audiofile
    // audio.connecttoFS(LITTLEFS, "/song.mp3");
    //    audio.connecttoFS(SD, "test.wav");
    // audio.connecttoFS(LITTLEFS, "/songs/song1.mp3");
    //    audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");
    //    audio.setVolume(15);
    //    audio.connecttohost("http://macslons-irish-pub-radio.com/media.asx");
    //    audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac"); //  128k aac
    //   audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3
    //    audio.connecttospeech("Wenn die Hunde schlafen, kann der Wolf gut Schafe stehlen.", "de");
    //    audio.connecttohost("http://media.ndr.de/download/podcasts/podcast4161/AU-20190404-0844-1700.mp3"); // podcast
}

void loop()
{
    // check for alarm and play
    checkPlayAlarm();
    audio.loop();
}

/**
 * @brief load settings an initialize alarms
 * 
 * @param fs FileSystem
 */
void loadSettings(fs::FS &fs)
{
    File configFile = fs.open("/config.json");
    String json = configFile.readString();
    configFile.close();
    Serial.println(json);
    DynamicJsonDocument doc(JSON_BUFFER);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        Serial.println("----- parseObject() for config.json failed -----");
        Serial.println(error.c_str());
        return;
    }
    else
    {
        // load sensor settings
        JsonArray alarmsJSON = doc["alarms"];
        size_t i = 0;
        for (auto a : alarmsJSON)
        {
            if (i == MAX_ALARMS)
            {
                Serial.println("Maximum number of alarms reached!");
                break;
            }
            const char *name = (const char *)a["name"];
            const char *file = (const char *)a["file"];
            int hour = (int)a["hour"];
            int min = (int)a["minute"];
            JsonArray dow = a["dow"];
            for (auto d : dow)
            {
                int day = (int)d;
                Serial.printf("Alarm %s %s %02d:%02d %s\n", name, dowName(day).c_str(), hour, min, file);
                alarms[i++] = AlarmSettings(name, day, hour, min, file);
            }
        }
        alarms_size = i;

        // load general settings
        auto general = doc["general"];

        if (general.containsKey("audio_volume"))
        {
            audio_volume = (uint8_t)general["audio_volume"];
        }

        if (general.containsKey("time_offset"))
        {
            time_offset_s = (uint32_t)general["time_offset"];
        }
    }
}
/**************************************
 *
 * selects next alarm, based on current time
 *
 *************************************/
void nextAlarm()
{
    // select next alarm
    struct tm timeinfo;
    bool selected = false;
    if (getLocalTime(&timeinfo))
    {
        for (size_t i = 0; i < alarms_size; i++)
        {
            if (alarms[i] > timeinfo)
            {
                alarms_next = i;
                selected = true;
                break;
            }
        }
    }

    // if no alarm time is bigger, alarm is the first one in the new week
    if (!selected)
    {
        alarms_next = 0;
    }

    Serial.println("Current time is:");
    printTime(timeinfo);
    Serial.println();
    Serial.println("Next Alarm is " + alarms[alarms_next].toString());
}

/**************************************
 *
 * check and play alarm, select next one
 *
 *************************************/
bool checkPlayAlarm()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Cannot get local time");
        return false;
    }

    // check if timer is reached and play file
    if (alarms[alarms_next] < timeinfo && alarms[alarms_next].differenceSec(timeinfo) < 10)
    {
        Serial.println("Playing Alarm " + alarms[alarms_next].toString());
        audio.setVolume(audio_volume);
        audio.connecttoFS(fsSongs, alarms[alarms_next].url);
        // select next alarm
        alarms_next = (alarms_next + 1) % alarms_size;
        return true;
    }

    return false;
}

/**************************************
 *
 * Print all falarms for debug reasons :)
 *
 *************************************/
void printAlarms()
{
    for (size_t i = 0; i < alarms_size; i++)
    {
        auto a = alarms[i];
        Serial.printf("[%02d] Alarm %s\n", i, a.toString().c_str());
    }
}
/**************************************
 *
 * Print all files on LITTLEFS for debug reasons :)
 *
 *************************************/
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

/**
 * @brief updates the config
 * 
 */
void handleAPIConfigUpdate(AsyncWebServerRequest *request)
{
    Serial.println("TODO: config update");
    // for (size_t i = 0; i < request->params(); i++)
    // {
    //     Serial.println("Param [" + String(i) + "]: " + request->getParam(i)->value());
    // }
    // DynamicJsonDocument doc(JSON_BUFFER);
    // DeserializationError error = deserializeJson(doc, request->getParam(0)->value());

    // // Test if parsing succeeds.
    // if (error)
    // {
    //     Serial.print(F("deserializeJson() failed: "));
    //     Serial.println(error.c_str());
    //     request->send(500, "text/html", error.c_str());
    //     return;
    // }

    // // open file
    // File configFile = LITTLEFS.open("/config.json", "w");
    // Serial.println("Config file: " + configFile.size());
    // Serial.println("Config file: " + configFile.getWriteError());
    // Serial.println("Config file: " + configFile.readString());

    // // Serialize JSON to file
    // if (serializeJson(doc, configFile) == 0)
    // {
    //     Serial.println(F("Failed to write to file"));
    //     request->send(500, "text/html", "Failed to write to file " + configFile.getWriteError());
    // }

    // // Close the file
    // configFile.close();

    // // String name = server.arg("name"); //root["name"];
    // // uint8_t value = server.arg("value").toInt();
    // request->send(200, "text/html", "saved");
}

bool readJSONFile(fs::FS &fs, String file, DynamicJsonDocument &doc, DeserializationError &error)
{
    // add webstreams from streams
    File streamsFile = fs.open(file, FILE_READ);
    String json = streamsFile.readString();
    streamsFile.close();
    Serial.println(json);
    error = deserializeJson(doc, json);
    if (error)
    {
        Serial.println("----- parseObject() for streams.json failed -----");
        Serial.println(error.c_str());
        return false;
    }
    return true;
}

bool writeJSONFile(fs::FS &fs, String file, DynamicJsonDocument &doc)
{
    // open file
    File f = fsConfig.open(file, FILE_WRITE);

    // Serialize JSON to file
    if (serializeJson(doc, f) == 0)
    {
        Serial.println(F("Failed to write to file"));
        return false;
    }

    // Close the file
    f.close();
    return true;
}

/**
 * @brief returns the config (alarms, general config, ...)
 * 
 */
void handleAPIConfig(AsyncWebServerRequest *request)
{
    // if method is POST and param exists
    if (request->method() == HTTP_POST && request->params())
    {
        Serial.println("Update config");
        for (size_t i = 0; i < request->params(); i++)
        {
            Serial.println("Param [" + String(i) + "]: " + request->getParam(i)->value());
        }
        DynamicJsonDocument doc(JSON_BUFFER);
        DeserializationError error = deserializeJson(doc, request->getParam(0)->value());

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            request->send(500, "text/html", error.c_str());
            return;
        }

        // open file
        File file = fsConfig.open("/config.json", "w");

        // Serialize JSON to file
        if (serializeJson(doc, file) == 0)
        {
            Serial.println(F("Failed to write to file"));
            request->send(500, "text/html", "Failed to write to file");
            return;
        }

        // Close the file
        file.close();
    }

    // File config = LITTLEFS.open("/config.json");
    // server.streamFile(config, "text/json");
    request->send(fsConfig, "/config.json", "application/json");
}

/**
 * @brief return list of songs on LITTLEFS/SD and webstreams in streams.json
 * 
 */
void handleAPISongs(AsyncWebServerRequest *request)
{
    // if method is POST and param exists
    if (request->method() == HTTP_POST && request->params())
    {
        Serial.printf("POST with %d params\n", request->params());
        if (request->getParam(0)->isPost())
        {
            // create JSON buffer
            DynamicJsonDocument req(JSON_BUFFER);
            DeserializationError error = deserializeJson(req, request->getParam(0)->value());
            if (error)
            {
                Serial.println("----- parseObject() failed -----");
                Serial.println(error.c_str());
                request->send(500, "application/json", "{\"error\" : \"cannot parse JSON\"");
                return;
            }

            String action = req["action"];
            Serial.println(request->getParam(0)->value());
            Serial.println("Got Action " + action);
            Serial.println("JSON request:");
            serializeJson(req, Serial);

            if (action == "delete")
            {
                String url = req["song"]["url"];
                Serial.println("Delete song " + url);
                // delete http streams from streams.json or delete file from SD card
                if (url.startsWith("http://"))
                {
                    // read current streams
                    DynamicJsonDocument doc(JSON_BUFFER);
                    DeserializationError error;
                    if (!readJSONFile(fsSongs, "/streams.json", doc, error))
                    {
                        Serial.println("----- parseObject() for streams.json failed -----");
                        Serial.println(error.c_str());
                        request->send(500, "application/json", "{\"error\" : \"cannot read streams\"");
                        return;
                    }
                    auto streams = doc.to<JsonArray>();
                    for (size_t i = 0; i < streams.size(); i++)
                    {
                        if (streams[i]["url"] == url)
                        {
                            streams.remove(i);
                            Serial.println("Removed");
                            break;
                        }
                    }
                    if (!writeJSONFile(fsSongs, "/streams.json", doc))
                    {
                        request->send(500, "application/json", "{\"error\" : \"cannot save streams\"");
                        return;
                    }
                }
                else if (!fsSongs.remove(url))
                {
                    request->send(500, "application/json", "{\"error\" : \"cannot remove song\"");
                }
            }
            else if (action == "addStream")
            {
                // add new stream
                // read current streams
                DynamicJsonDocument doc(JSON_BUFFER);
                JsonArray streams = doc.to<JsonArray>();
                DeserializationError error;
                if (!readJSONFile(fsSongs, "/streams.json", doc, error))
                {
                    Serial.println("----- parseObject() for streams.json failed -----");
                    Serial.println(error.c_str());
                    request->send(500, "application/json", "{\"error\" : \"cannot read streams\"");
                    return;
                }
                // copy only necessary fields
                JsonObject stream = streams.createNestedObject();
                stream["name"] = req["stream"]["name"];
                stream["url"] = req["stream"]["url"];

                // add new stream
                // streams.add(stream);

                // Serial.println();
                // serializeJson(streams, Serial);
                // Serial.println();

                // write back to streams
                if (!writeJSONFile(fsSongs, "/streams.json", doc))
                {
                    request->send(500, "application/json", "{\"error\" : \"cannot save streams\"");
                    return;
                }
            }
            else if (action == "addSong")
            {
                // upload new song to sd card
                Serial.println("ToDo: upload song to sd card");
            }
        }
    }
    // create JSON buffer and read streams
    DynamicJsonDocument doc(JSON_BUFFER);
    JsonArray array = doc.to<JsonArray>();
    DeserializationError error;
    if (!readJSONFile(fsSongs, "/streams.json", doc, error))
    {
        Serial.println("----- parseObject() for streams.json failed -----");
        Serial.println(error.c_str());
        request->send(500, "application/json", "{\"error\" : \"cannot read streams\"");
    }

    // iterate through songs directory
    File root = fsSongs.open("/songs");
    File file = root.openNextFile();
    while (file)
    {
        Serial.println(file.name());
        auto f = array.createNestedObject();
        f["name"] = String(file.name()).substring(String(file.name()).lastIndexOf("/") + 1);
        f["url"] = String(file.name());
        f["size"] = file.size();
        file = root.openNextFile();
    }

    // send JSON
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, Serial);
    serializeJson(doc, *response);
    request->send(response);
}

void handleAPISongsUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    Serial.printf("Uploading %s (%u bytes written)\n", filename.c_str(), index);

    String path = "/songs/" + filename;

    // open file
    if (!index)
    {
        Serial.printf("Open file %s\n", filename.c_str());
        if(fsUploadFile){
            Serial.println("File is already opened!");
            request->send(500, "text/plain", "file already opened");
            return;
        } else {
            fsUploadFile = fsSongs.open(path, FILE_WRITE);
        }
    }

    // write data
    for (size_t i = 0; i < len; i++)
    {
        fsUploadFile.write(data[i]);
    }

    if (final)
    {
        Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
        fsUploadFile.close();
        request->send(202, "text/plain", "ok");
    }


// if (request->getParam(0)->isFile())
//         {

//             Serial.println("Wrong upload function :(");
//             Serial.println(request->getParam(0)->name());
//             Serial.println(request->getParam(0)->size());

//             // write file to SD
//             String path = "/songs/" + request->getParam(0)->name();
//             File file = fsSongs.open(path, FILE_WRITE);
//             if (!file)
//             {
//                 Serial.printf("Wiriting to %s failed!\n", path.c_str());
//                 request->send(500, "application/json", "{\"error\" : \"cannot write file to SD\"");
//                 return;
//             }
//             size_t len = file.write((uint8_t *)request->getParam(0)->value().c_str(), request->getParam(0)->size());
//             Serial.printf("Wrote %d bytes to %s", len, file.name());
//         }
//         else 



    // if method is POST and param exists
    // if (request->method() == HTTP_POST && request->params())
    // {
    //     Serial.printf("POST with %d params\n", request->params());
    //     if (request->getParam(0)->isFile())
    //     {

    //         Serial.println("ToDo: file upload");
    //         Serial.println(request->getParam(0)->name());
    //         Serial.println(filename);
    //         Serial.println(final);
    //         Serial.println(len);
    //         Serial.println(index);

    //         //     // write file to SD
    //         //     String path = "/songs/" + request->getParam(0)->name();
    //         //     File file = fsSongs.open(path, FILE_WRITE);
    //         //     if(!file){
    //         //         Serial.printf("Wiriting to %s failed!\n", path.c_str());
    //         //         request->send(500, "application/json", "{\"error\" : \"cannot write file to SD\"");
    //         //         return;
    //         //     }
    //         //     size_t len = file.write((uint8_t*)request->getParam(0)->value().c_str(), request->getParam(0)->size());
    //         //     Serial.printf("Wrote %d bytes to %s", len, file.name());
    //         // }
    //     }
    // }
}

void handleAPIPlayback(AsyncWebServerRequest *request)
{
    // if method is POST and param exists
    if (request->method() == HTTP_POST && request->params())
    {
        if (request->getParam(0)->isPost())
        {
            Serial.println(request->getParam(0)->value());
            // create JSON buffer
            DynamicJsonDocument doc(512);
            DeserializationError error = deserializeJson(doc, request->getParam(0)->value());
            if (error)
            {
                Serial.println("----- parseObject() failed -----");
                Serial.println(error.c_str());
                request->send(500, "application/json", "{\"error\" : \"cannot parse JSON\"");
                return;
            }

            String action = doc["action"];

            if (action == "play")
            {
                String url = doc["url"];
                Serial.printf("Playing song %s\n", url.c_str());
                if (url.startsWith("http"))
                {
                    audio.connecttohost(url);
                }
                else
                {
                    File file = fsSongs.open(url, FILE_READ);
                    if (!file)
                    {
                        request->send(500, "application/json", "{\"error\" : \"cannot open file\"");
                        file.close();
                        return;
                    }
                    audio.connecttoFS(fsSongs, url);
                }
            }
            else if (action == "stop")
            {
                audio.stopSong();
            }
            else if (action == "pause")
            {
                audio.pauseResume();
            }
            else if (action == "volume")
            {
                int volume = doc["volume"];
                audio.setVolume(volume);
            }
        }
    }

    // create JSON response
    DynamicJsonDocument state(JSON_BUFFER);
    state["volume"] = audio.getVolume();
    state["position"] = audio.getFilePos();
    state["current"] = audio.getAudioCurrentTime();
    state["duration"] = audio.getAudioFileDuration();
    state["playing"] = audio.isRunning();

    // send JSON response
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(state, Serial);
    serializeJson(state, *response);
    request->send(response);
}


void handleAPIState(AsyncWebServerRequest *request){
    
    // create JSON response
    DynamicJsonDocument doc(JSON_BUFFER);
    doc["heap_free"] = ESP.getFreeHeap();
    doc["heap_size"] = ESP.getHeapSize();
    doc["chip_rev"] = ESP.getChipRevision();
    doc["sdk"] = ESP.getSdkVersion();
    doc["wifi"] = WiFi.RSSI();
    doc["flash_used"] = LITTLEFS.usedBytes();
    doc["flash_total"] = LITTLEFS.totalBytes();
    doc["sd_used"] = int64String(SD.usedBytes());
    doc["sd_total"] = int64String(SD.totalBytes());
    doc["cpu_frequ"] = ESP.getCpuFreqMHz();
    doc["alarm_next"] = alarms[alarms_next].toString();

    // send JSON response
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, Serial);
    serializeJson(doc, *response);
    request->send(response);
}
// void handleAPIVolume(AsyncWebServerRequest *request){
//     // if method is POST and param exists
//     if (request->method() == HTTP_POST && request->params())
//     {
//         if (request->getParam(0)->isPost())
//         {
//             int param = request->getParam(0)->value().toInt();
//             audio.setVolume(param);
//         }
//     }

//     request->send(200, "application/text", String(audio.getVolume()));
// }

/**************************************
 *
 * NTP time sync provider
 *
 *************************************/
time_t getNtpTime()
{
    struct tm timeinfo;
    if (!WiFi.isConnected())
    {
        Serial.println("WiFi not connected, cannot config time!");
        return mktime(&timeinfo) + time_offset_s;
    }
    //configTime(-3600, -3600, "69.10.161.7");

    Serial.printf("Update time with GMT+%02d and DST: %d\n",
                  time_offset_s / 3600, 0);

    configTime(0, time_offset_s, "69.10.161.7");

    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
    }
    else
    {
        Serial.print("Synced time to: ");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    }
    return mktime(&timeinfo) + time_offset_s;
}

/**************************************
 *
 * gets called when an Alarm was triggered. Checks all Timers for the right alarm and activates button
 *
 *************************************/
// void timerCallback() {
//     AlarmId id = Alarm.getTriggeredAlarmId();
//     Serial.printf("Timer %d got triggered", id);
//     audio.connecttoFS(LITTLEFS, "/songs/song1.mp3");
//     // audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");
// }

/**************************************
 *
 * print date and time
 *
 *************************************/
void printTime(struct tm info)
{
    Serial.printf("%02d.%02d.%04d %02d:%02d:%02d %s, dst: %d",
                  info.tm_mday, info.tm_mon, info.tm_year,
                  info.tm_hour, info.tm_min, info.tm_sec,
                  dowName(info.tm_wday).c_str(),
                  info.tm_isdst);
}

/**************************************
 *
 * get dow name
 *
 *************************************/
bool compareAlarm(AlarmSettings a, AlarmSettings b)
{
    if (a > b)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**************************************
 *
 * Audio callbacks/functions
 *
 *************************************/
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ //id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ //end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreaminfo(const char *info)
{
    Serial.print("streaminfo  ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ //duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ //homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ //stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
void audio_eof_speech(const char *info)
{
    Serial.print("eof_speech  ");
    Serial.println(info);
}
#include <Arduino.h>
#include <Esp.h>
#include <WiFi.h>
#include <DNSServer.h>
// #include <WebServer.h>
// #include <WiFiManager.h>
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

#include "AlarmSettings.h"
#include "utils.h"

// Digital I/O used
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

#define BTN_PLAY 14
#define LED_BUILTIN 5

#define JSON_BUFFER 4096

// Global variables
Audio audio;

// settings
uint32_t time_offset_s = 0;
uint8_t audio_volume = 0;

// alarms
#define MAX_ALARMS 100
AlarmSettings alarms[100];
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
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
time_t getNtpTime();
void timerCallback();
void printTime(struct tm);
bool compareAlarm(AlarmSettings first, AlarmSettings second);
bool checkPlayAlarm();

void handleAPIConfigUpdate(AsyncWebServerRequest *request);
void handleAPIConfig(AsyncWebServerRequest *request);
void handleAPIFiles(AsyncWebServerRequest *request);
void handleAPIFilesUpload(AsyncWebServerRequest *request);
void handleAPIFilesDelete(AsyncWebServerRequest *request);

// setup
void setup()
{

    // setup pins
    pinMode(LED_BUILTIN, OUTPUT);

    // setup serial
    Serial.begin(115200);

    // Initialize LITTLEFS
    if (!LITTLEFS.begin())
    {
        Serial.println("An Error has occurred while mounting LITTLEFS");
        return;
    }

    Serial.println("Files on flash:");
    listDir(LITTLEFS, "/", 5);

    // Initialize SD
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
    Serial.println("Files on SD:");
    listDir(SD, "/", 5);

    // load config
    loadSettings(fsConfig);

    // connect WiFi
    digitalWrite(LED_BUILTIN, HIGH);
    wifiManager.autoConnect();
    digitalWrite(LED_BUILTIN, LOW);

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

    // setup I2S audio
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(audio_volume); // 0...21

    // sort alarms by date
    Serial.println("Alarms before sort:");
    printAlarms();
    sortArray(alarms, alarms_size, compareAlarm);
    Serial.println("Alarms after sort:");
    printAlarms();
    // select next alarm
    nextAlarm();

    // HTTP Server
    server.begin();
    server.serveStatic("/", fsWWW, "/www/");
    // server.rewrite("/", "/index.hmtl");
    server.serveStatic("/index.html", fsWWW, "/www/index.html");
    server.serveStatic("/css/boostrap.min.css", fsWWW, "/www/css/boostrap.min.css.gz");
    server.serveStatic("/css/style.css", fsWWW, "/www/css/style.css");
    server.serveStatic("/js/bootstrap.min.js", fsWWW, "/www/js/bootstrap.min.js.gz");
    server.serveStatic("/js/jquery.min.js", fsWWW, "/www/js/jquery.min.js.gz");
    server.serveStatic("/js/knockout.js", fsWWW, "/www/js/knockout.js.gz");
    server.serveStatic("/js/script.js", fsWWW, "/www/js/script.js");

    server.on("/api/config", handleAPIConfig);
    server.on("/api/config/update", HTTP_POST, handleAPIConfigUpdate);
    server.on("/api/time", [](AsyncWebServerRequest *request) {
        // return current device time
        String time = String(now());
        request->send(200, "application/json", "{\"time\": \"" + time + "\"}");
    });
    server.on("/api/stats", [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"free_heap\": \"" + String(ESP.getFreeHeap()) + "\", "
                                                                                                  "\"chip_rev\" : \"" +
                                                   ESP.getChipRevision() + "\", "
                                                                           "\"sdk\" : \"" +
                                                   ESP.getSdkVersion() + "\", "
                                                                         "\"wifi\" : \"" +
                                                   WiFi.RSSI() + "\", "
                                                                 "\"flash_used\" : \"" +
                                                   LITTLEFS.usedBytes() + "\", "
                                                                "\"sd_used\" : \"" +
                                                   String((uint32_t)SD.usedBytes()) + "\", "
                                                                        "\"cpu_freq\" : \"" +
                                                   ESP.getCpuFreqMHz() + "\"}");
    });
    server.on("/api/files", handleAPIFiles);
    server.on("/api/files/upload", HTTP_POST, handleAPIFilesUpload);
    server.on("/api/files/delete", HTTP_POST, handleAPIFilesDelete);
    
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
            for (auto d : dow){
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
        audio.connecttoFS(fsSongs, alarms[alarms_next].file);
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
void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
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
 * @brief return list of songs on LITTLEFS/SD
 * 
 */
void handleAPIFiles(AsyncWebServerRequest *request)
{
    // if method is POST and param exists
    if (request->method() == HTTP_POST && request->params())
    {
        if(request->getParam(0)->isFile()){
            Serial.println("ToDo: file upload");
        } else if(request->getParam(0)->isPost()){
            Serial.println("TODO: File delete " + request->getParam(0)->value());
            
        }
    }
    // create JSON buffer
    DynamicJsonDocument doc(JSON_BUFFER);
    JsonArray array = doc.to<JsonArray>();

    // iterate through songs directory
    File root = fsSongs.open("/songs");
    File file = root.openNextFile();
    while (file)
    {
        Serial.println(file.name());
        auto f = array.createNestedObject();
        f["name"] = String(file.name()).substring(String(file.name()).lastIndexOf("/")+1);
        f["size"] = file.size();
        file = root.openNextFile();
    }

    // send JSON
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(array, Serial);
    serializeJson(array, *response);
    request->send(response);
}

/**
 * @brief handles upload of new songs
 * 
 */
void handleAPIFilesUpload(AsyncWebServerRequest *request)
{
    Serial.println("TODO: File upload");
}

/**
 * @brief delete song
 * 
 */
void handleAPIFilesDelete(AsyncWebServerRequest *request)
{
    Serial.println("TODO: File delete " + request->getParam(0)->value());
}

/**************************************
 *
 * NTP time sync provider
 *
 *************************************/
time_t getNtpTime()
{
    //configTime(-3600, -3600, "69.10.161.7");

    Serial.printf("Update time with GMT+%02d and DST: %d\n",
                  time_offset_s / 3600, 0);

    configTime(0, time_offset_s, "69.10.161.7");
    struct tm timeinfo;
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
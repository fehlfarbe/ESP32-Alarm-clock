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
#include <Update.h>
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
#include <TM1637Display.h>
#include <RDA5807.h>
#include <FastLED.h>

#include "AlarmSettings.h"
#include "MusicStream.h"
#include "utils.h"

// Digital I/O used
#define I2S_DOUT 25
#define I2S_BCLK 4
#define I2S_LRC 27

#define SW0 34
#define SW1 35
#define SW2 39
#define LED_BUILTIN 2
#define LED_STATUS 26

// Display and I2C
#define CLK 16
#define DIO 17
#define SCL 22
#define SDA 21

// ADC, GPIO33
#define ADC_UNIT ADC_UNIT_1
#define ADC_CHANNEL ADC1_CHANNEL_5

#define JSON_BUFFER 4096

// Global variables
Audio audio;
RDA5807 radio;

// LED
CRGB led_status[1];

// settings
uint32_t gmt_offset_s = 0;
uint32_t dst_offset_s = 0;
uint8_t audio_volume = 0;

// alarms
#define MAX_ALARMS 100
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

// Display
TM1637Display display(CLK, DIO);

// parallel task
TaskHandle_t pTask;

enum LED_STATE {
    STATE_INIT = HUE_RED,
    STATE_WIFI_CONNECTING = HUE_YELLOW,
    STATE_WIFI_AP = HUE_BLUE,
    STATE_WIFI_CONNECTED = HUE_GREEN
};

// function declarations
void parallelTask(void *parameter);
void loadSettings(fs::FS &fs);
void updateAlarms();
void nextAlarm();
void printAlarms();
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
time_t getNtpTime();
void timerCallback();
void printTime(struct tm);
bool compareAlarm(AlarmSettings first, AlarmSettings second);
bool checkPlayAlarm();
void showDisplay(DisplayState state);
void setLEDSTate(LED_STATE state);

bool readJSONFile(fs::FS &fs, String file, DynamicJsonDocument &doc, DeserializationError &error);
bool writeJSONFile(fs::FS &fs, String file, DynamicJsonDocument &doc);

void WiFiEvent( WiFiEvent_t event );

// void handleAPIConfigUpdate(AsyncWebServerRequest *request);
void handleAPIConfig(AsyncWebServerRequest *request);
void handleAPISongs(AsyncWebServerRequest *request);
void handleAPISongsUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void handleAPIPlayback(AsyncWebServerRequest *request);
void handleAPIState(AsyncWebServerRequest *request);
void handleOTAUpdateForm(AsyncWebServerRequest *request);
void handleOTAUpdateResponse(AsyncWebServerRequest *request);
void handleOTAUpdateUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

// setup
void setup()
{
    // setup pins
    pinMode(SW0, INPUT_PULLUP);
    pinMode(SW1, INPUT_PULLUP);
    pinMode(SW2, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);
    
    // setup LEDs
    FastLED.addLeds<NEOPIXEL, LED_STATUS>(led_status, 1);
    setLEDSTate(LED_STATE::STATE_INIT);

    // setup serial
    Serial.begin(115200);
    delay(1000);

    // setup I2S audio
    Serial.println("Setup I2S output");
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(audio_volume); // 0...21
    audio.stopSong();

    // setup I2C radio
    Serial.println("Setup RDA5807M I2C radio");
    Wire.begin(SDA, SCL);
    Wire.beginTransmission (0x10);
    if (Wire.endTransmission() == 0) {
        Serial.print (F("I2C device found at 0x"));
        radio.setup();
        radio.setVolume(15);
        radio.powerDown();
    } else {
        Serial.println("RDA5807M Radio not found");
    }

    // setup display
    display.clear();
    display.setBrightness(3, true);

    // Initialize LITTLEFS
    if (!LITTLEFS.begin())
    {
        Serial.println("An Error has occurred while mounting LITTLEFS");
    }

    Serial.println("Files on flash:");
    listDir(LITTLEFS, "/", 5);

    // Initialize SD
    while (!SD.begin())
    {
        showDisplay(DisplayState::SD_ERR);
        Serial.println("Card Mount Failed");
        delay(1000);
        while (true);
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        showDisplay(DisplayState::SD_ERR);
        while (true);
    }
    Serial.println("Files on SD:");
    listDir(SD, "/", 5);

    // load config
    loadSettings(fsConfig);

    // connect WiFi
    // if (digitalRead(BTN_WIFI_RESET) == LOW)
    // {
    //     Serial.println("Reset WiFi setting...");
    //     wifiManager.resetSettings();
    // }
    showDisplay(DisplayState::WIFI_CONNECT);
    setLEDSTate(LED_STATE::STATE_WIFI_CONNECTING);
    WiFi.onEvent( WiFiEvent );
    wifiManager.setConnectTimeout(10);
    if (!wifiManager.autoConnect())
    {
        Serial.println("failed to connect, we should reset as see if it connects");
        setLEDSTate(LED_STATE::STATE_WIFI_AP);
    }
    setLEDSTate(LED_STATE::STATE_WIFI_CONNECTED);

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
        Serial.printf("mDNS responder started with hostname %s.local\n", hostname);
    }

    // show IP
    Serial.println(WiFi.localIP());

    // sync time
    showDisplay(DisplayState::SYNC);
    setSyncProvider(getNtpTime);

    // print current time
    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        Serial.printf("Current time: ");
        printTime(timeinfo);
        Serial.println();
    }

    // update alarms
    updateAlarms();

    // HTTP Server
    server.begin();
    server.serveStatic("/", fsWWW, "/www/");
    server.on("/api/config", handleAPIConfig);
    // server.on("/api/config/update", HTTP_POST, handleAPIConfigUpdate);
    server.on("/api/state", handleAPIState);
    server.on("/api/songs", handleAPISongs);
    server.on("/api/songs/update", HTTP_POST, handleAPISongs);
    server.on(
        "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
            request->send(200);
        },
        handleAPISongsUpload);
    server.on("/api/playback", handleAPIPlayback);

    // Simple Firmware Update
    server.on("/update", HTTP_GET, handleOTAUpdateForm);
    server.on("/update", HTTP_POST, handleOTAUpdateResponse, handleOTAUpdateUpload);
    
    // setup parallel task
    xTaskCreatePinnedToCore(
        parallelTask, /* Function to implement the task */
        "task2",      /* Name of the task */
        10000,        /* Stack size in words */
        NULL,         /* Task input parameter */
        1,            /* Priority of the task */
        &pTask,       /* Task handle. */
        0);           /* Core where the task should run */

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
    // audio.setVolume(15);
    //    ahudio.connecttohost("http://media.ndr.de/download/podcasts/podcast4161/AU-20190404-0844-1700.mp3"); // podcast
}

void loop()
{
    audio.loop();
    // digitalWrite(LED_BUILTIN, audio.isRunning());
}

void parallelTask(void *parameter)
{
    while (true)
    {
        // check for alarm and play
        checkPlayAlarm();
        // update display time
        showDisplay(DisplayState::TIME);
        delay(100);
    }
    Serial.println("Exit task...");
}

/**
 * @brief load settings an initialize alarms from /config.json
 * 
 * @param fs FileSystem (LITTLEFS, SPIFFS, SD; ...)
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
            // read music type first
            String music_name = a["music"];
            String music_url = a["file"];
            MusicType music_type = MusicStream::stringToType(a["type"]);
            MusicStream stream = MusicStream(music_name, music_url, music_type);

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
                alarms[i++] = AlarmSettings(name, day, hour, min, stream);
            }
        }
        alarms_size = i;

        // load general settings
        auto general = doc["general"];

        if (general.containsKey("audio_volume"))
        {
            audio_volume = (uint8_t)general["audio_volume"];
            //audio.setVolume(audio_volume);
        }

        if (general.containsKey("gmt_offset"))
        {
            gmt_offset_s = (uint32_t)general["gmt_offset"];
        }

        if (general.containsKey("dst_offset"))
        {
            dst_offset_s = (uint32_t)general["dst_offset"];
        }
    }
}
/**************************************
 *
 * sort and update alarms
 *
 *************************************/
void updateAlarms(){
    // sort alarms by date
    sortArray(alarms, alarms_size, compareAlarm);
    Serial.println("Alarms after sort:");
    printAlarms();
    // select next alarm
    nextAlarm();
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

        auto alarm = alarms[alarms_next];
        auto stream = alarm.getStream();
        Serial.printf("Playing %s of type %s from %s\n", alarm.name.c_str(),
                                                         MusicStream::typeToString(stream.getType()).c_str(),
                                                         stream.getURL().c_str());
        Serial.println("Audio volume: " + audio_volume);

        switch (stream.getType())
        {
        case MusicType::FILESYSTEM:
            audio.connecttoFS(fsSongs, stream.getURL());
            break;
        case MusicType::STREAM:
            audio.connecttohost(stream.getURL());
            break;
        case MusicType::FM:
            radio.powerUp();
            radio.setFrequency(stream.getFMFrequency());
            radio.setVolume(15);
            audio.connecttoADC(ADC_UNIT, ADC_CHANNEL);
            break;
        default:
            break;
        }

        // select next alarm
        alarms_next = (alarms_next + 1) % alarms_size;
        return true;
    }

    return false;
}

/**
 * @brief Updates TM1637 display
 * 
 * @param state 
 */
void showDisplay(DisplayState state)
{
    struct tm timeinfo;
    const uint8_t sync[] = {
        SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, // S
        SEG_F | SEG_G | SEG_B | SEG_C,         // Y
        SEG_E | SEG_G | SEG_C,                 // n
        SEG_E | SEG_G | SEG_D                  // c
    };
    const uint8_t conn[] = {
        SEG_E | SEG_G | SEG_D,         // c
        SEG_E | SEG_G | SEG_D | SEG_C, // o
        SEG_E | SEG_G | SEG_C,         // n
        SEG_E | SEG_G | SEG_C          // n
    };
    const uint8_t sd_err[] = {
        SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, // S
        SEG_E | SEG_G | SEG_D | SEG_C | SEG_B, // d
        0,
        0};
    const uint8_t ap[] = {
        SEG_A | SEG_F | SEG_B | SEG_B | SEG_E | SEG_C, // A
        SEG_A | SEG_F | SEG_G | SEG_B | SEG_E,         // P
        0,
        0};

    switch (state)
    {
    case DisplayState::WIFI_CONNECT:
        display.setSegments(conn);
        break;
    case DisplayState::WIFI_CONNECTED:
        break;
    case DisplayState::WIFI_PORTAL:
        display.setSegments(ap);
        break;
    case DisplayState::SYNC:
        display.setSegments(sync);
        break;
    case DisplayState::TIME:
        if (!getLocalTime(&timeinfo, 1000))
        {
            display.showNumberDec(0, true);
        }
        else
        {
            display.showNumberDecEx(timeinfo.tm_hour * 100 + timeinfo.tm_min, 0b01000000, true);
        }
        break;
    case DisplayState::SD_ERR:
        display.setSegments(sd_err);
        break;
    default:
        break;
    }
}

/**************************************
 *
 * Set LED state
 *
 *************************************/
void setLEDSTate(LED_STATE state){
    led_status[0] = CHSV(state, 255, 70);
    FastLED.show();
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
 * @brief Reads JSON file from filesystem
 * 
 * @param fs  source filesystem (SPIFFS, LITTLEFS, SD, ...)
 * @param file  filename/path (start with /)
 * @param doc JSONDocument
 * @param error error info if it didn't work
 * @return true success
 * @return false not success, more info in error argument
 */
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

/**
 * @brief serializes JSON document to filesystem
 * 
 * @param fs target filesystem (SPIFFS, LITTLEFS, SD, ...)
 * @param file filename/path (start with /)
 * @param doc JSONDocument
 * @return true if successful
 * @return false if not successful
 */
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
 * @brief handles WiFi events and auto reconnects
 * 
 */
void WiFiEvent( WiFiEvent_t event ) {
  switch ( event ) {
    case SYSTEM_EVENT_AP_START:
      ESP_LOGI( TAG, "AP Started");
      setLEDSTate(LED_STATE::STATE_WIFI_AP);
      //WiFi.softAPsetHostname(AP_SSID);
      break;
    case SYSTEM_EVENT_AP_STOP:
      ESP_LOGI( TAG, "AP Stopped");
      break;
    case SYSTEM_EVENT_STA_START:
      ESP_LOGI( TAG, "STA Started");
      //WiFi.setHostname( DEFAULT_HOSTNAME_PREFIX.c_str( );
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      ESP_LOGI( TAG, "STA Connected");
      setLEDSTate(LED_STATE::STATE_WIFI_CONNECTED);
      //WiFi.enableIpV6();
      break;
    case SYSTEM_EVENT_AP_STA_GOT_IP6:
      ESP_LOGI( TAG, "STA IPv6: ");
      //ESP_LOGI( TAG, "%s", WiFi.localIPv6().toString());
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      //ESP_LOGI( TAG, "STA IPv4: ");
      //ESP_LOGI( TAG, "%s", WiFi.localIP());
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      ESP_LOGI( TAG, "STA Disconnected -> reconnect");
      setLEDSTate(LED_STATE::STATE_INIT);
      WiFi.begin();
      break;
    case SYSTEM_EVENT_STA_STOP:
      ESP_LOGI( TAG, "STA Stopped");
      break;
    default:
      break;
  }
}

/**
 * @brief returns the config (alarms, general config, ...) as JSON
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
        File file = fsConfig.open("/config.json", FILE_WRITE);

        // Serialize JSON to file
        if (serializeJson(doc, file) == 0)
        {
            Serial.println(F("Failed to write to file"));
            request->send(500, "text/html", "Failed to write to file");
            return;
        }

        // Close the file
        file.close();

        // update config and reinit alarms
        loadSettings(fsConfig);
        updateAlarms();
    }

    request->send(fsConfig, "/config.json", "application/json");
}

/**
 * @brief return list of songs on LITTLEFS/SD and webstreams in streams.json 
 * and adds/deletes songs/streams if action is addStream/delete. To upload a new
 * song, use handleAPISongsUpload
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
            // Serial.println(request->getParam(0)->value());
            // Serial.println("Got Action " + action);
            // Serial.println("JSON request:");
            // serializeJson(req, Serial);

            if (action == "delete")
            {
                String url = req["song"]["url"];
                auto type = MusicStream::stringToType(req["song"]["type"]);
                Serial.println("Delete song " + url);
                // delete http streams from streams.json or delete file from SD card
                if (type == MusicType::FM || type == MusicType::STREAM)
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
                    // find stream/fm on array, delete and write back
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
                // add new stream / fm station
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
                stream["type"] = req["stream"]["type"];

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
        f["type"] = MusicStream::typeToString(MusicType::FILESYSTEM);
        file = root.openNextFile();
    }

    // send JSON
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(doc, Serial);
    serializeJson(doc, *response);
    request->send(response);
}

/**
 * @brief Uploads a song to SD
 * 
 * @param request 
 * @param filename song filename
 * @param index data offset
 * @param data data array
 * @param len length of data array
 * @param final true if last chunk
 */
void handleAPISongsUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    Serial.printf("Uploading %s (%u bytes written)\n", filename.c_str(), index);

    String path = "/songs/" + filename;

    // open file if there is no other file opened
    if (!index)
    {
        Serial.printf("Open file %s\n", filename.c_str());
        if (fsUploadFile)
        {
            Serial.println("File is already opened!");
            request->send(500, "text/plain", "file already opened");
            return;
        }
        else
        {
            fsUploadFile = fsSongs.open(path, FILE_WRITE);
        }
    }

    // write data
    for (size_t i = 0; i < len; i++)
    {
        fsUploadFile.write(data[i]);
    }

    // close file
    if (final)
    {
        Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
        fsUploadFile.close();
        DynamicJsonDocument buffer(JSON_BUFFER);
        buffer["name"] = filename;
        buffer["url"] = path;
        buffer["size"] = len*index + len;
        // send JSON response
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(buffer, *response);
        request->send(response);
    }
}

/**
 * @brief If POST data contains JSON object with play/pause/stop/volume action it will 
 * play/pause/stop song or set volume. Always returns JSON with current audio state 
 * (playing, postion, duration, volume, ...)
 * 
 * @param request 
 */
void handleAPIPlayback(AsyncWebServerRequest *request)
{
    // if method is POST and param exists
    if (request->method() == HTTP_POST && request->params())
    {
        if (request->getParam(0)->isPost())
        {
            // Serial.println(request->getParam(0)->value());
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
                else if (url.toFloat())
                {
                    Serial.printf("Set radio frequency to %.2f\n", url.toFloat());
                    radio.powerUp();
                    radio.setFrequency((uint16_t)(url.toFloat()*100));
                    radio.setVolume(15);
                    audio.connecttoADC(ADC_UNIT, ADC_CHANNEL);
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
    // serializeJson(state, Serial);
    serializeJson(state, *response);
    request->send(response);
}

/**
 * @brief Sends JSON with heap size, cpu speed, used flash/sd space etc.
 * 
 * @param request 
 */
void handleAPIState(AsyncWebServerRequest *request)
{

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

void handleOTAUpdateForm(AsyncWebServerRequest *request)
{
    request->send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
}

void handleOTAUpdateResponse(AsyncWebServerRequest *request)
{
    bool shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? "OK" : "FAIL");
    response->addHeader("Connection", "close");
    request->send(response);
}

void handleOTAUpdateUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
        Serial.printf("Update Start: %s\n", filename.c_str());
        //   Update.runAsync(true);
        if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000))
        {
            Update.printError(Serial);
        }
        else
        {
            Serial.println("Update begin...");
        }
    }
    if (!Update.hasError())
    {
        Serial.printf("writing...%d bytes\n", len);
        if (Update.write(data, len) != len)
        {
            Update.printError(Serial);
        }
    }
    if (final)
    {
        if (Update.end(true))
        {
            Serial.printf("Update Success: %uB\n", index + len);
            ESP.restart();
        }
        else
        {
            Update.printError(Serial);
        }
    }
}

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
        return mktime(&timeinfo) + gmt_offset_s + dst_offset_s;
    }
    //configTime(-3600, -3600, "69.10.161.7");

    Serial.printf("Update time with GMT+%02d and DST: %d\n",
                  gmt_offset_s / 3600, dst_offset_s / 3600);

    configTime(gmt_offset_s, dst_offset_s, "0.de.pool.ntp.org", "1.de.pool.ntp.org", "69.10.161.7");

    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
    }
    else
    {
        Serial.print("Synced time to: ");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    }
    return mktime(&timeinfo) + gmt_offset_s + dst_offset_s;
}

/**************************************
 *
 * print date and time
 *
 *************************************/
void printTime(struct tm info)
{
    Serial.printf("%02d.%02d.%d %02d:%02d:%02d %s, dst: %d",
                  info.tm_mday, info.tm_mon, info.tm_year + 1900,
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
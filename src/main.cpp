#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <FastLED.h>
#include <HTTPClient.h>
#include <Int64String.h>
#include <LittleFS.h>
#include <SD_MMC.h>
#include <SPI.h>
#include <TM1637Display.h>
#include <TimeLib.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include <esp_log.h>

#include "Alarm.h"
#include "AudioProvider.h"
#include "Config.h"
#include "MusicStream.h"
#include "utils.h"

// Digital I/O used
#define I2S_DOUT 1
#define I2S_BCLK 2
#define I2S_WS 42
#define I2S_RADIO_OUT 9
#define I2S_MUTE 18
#define I2S_DEEM 3

// SD_MMC
#define SD_MMC_CLK 14
#define SD_MMC_CMD 21
#define SD_MMC_D0 13
#define SD_MMC_D1 12
#define SD_MMC_D2 48
#define SD_MMC_D3 47

#define SW0 40
#define SW1 41
#define SW_WIFI_RESET SW0
#define LED_STATUS 8

// Display and I2C
#define CLK 39
#define DIO 38
#define SCL 10
#define SDA 11

// Global variables
AudioProvider audio;
unsigned long startTime = millis();
const char* TAG = "ESP32Alarm";

// LED
enum WIFI_LED_STATE {
    STATE_INIT = HUE_PINK,
    STATE_WIFI_CONNECTING = HUE_ORANGE,
    STATE_WIFI_AP = HUE_BLUE,
    STATE_WIFI_AP_CONNECTED = HUE_AQUA,
    STATE_WIFI_CONNECTED = HUE_GREEN,
    STATE_WIFI_DISCONNECTED = HUE_RED
};

enum SYSTEM_LED_STATE {
    STATE_NONE,
    STATE_FS_ERR,
    STATE_SD_ERR,
    STATE_SD_NO_SD,
    STATE_TIME_NOT_SYNCED
};

#define LED_MAX_BRIGHTNESS 150
#define LED_STATUS_IDX 0
#define LED_WIFI_IDX 1
CRGB led_status[2];

SYSTEM_LED_STATE ledSystem = SYSTEM_LED_STATE::STATE_NONE;
WIFI_LED_STATE ledWiFi = WIFI_LED_STATE::STATE_INIT;

// config
AlarmClock::Config config;

// data sources
#define fsSongs SD_MMC
#define fsConfig SD_MMC
const String configPath = "/config.json";
const String streamsPath = "/streams.json";
const String songsDir = "/songs/";

// WebServer
AsyncWebServer server(80);
DNSServer dns;
File fsUploadFile;

// WiFi
AsyncWiFiManager wifiManager(&server, &dns);

// Display
TM1637Display display(CLK, DIO);

// parallel task
TaskHandle_t pTask;
TaskHandle_t pLedTask;

// function declarations
void checkAlarmTask(void* parameter);
void ledTask(void* parameter);
void initSDCardStructure(fs::FS& fs);
void updateAlarms();
void nextAlarm();
void printAlarms();
void listDir(fs::FS& fs, const char* dirname, uint8_t levels);
time_t updateNTPTime();
void printTime(struct tm);
bool checkPlayAlarm();
void showDisplay(DisplayState state);
void setWiFiLEDState(WIFI_LED_STATE state);
void setSystemLEDState(SYSTEM_LED_STATE state);
void configModeCallback(AsyncWiFiManager* myWiFiManager);
void WiFiEvent(WiFiEvent_t event);

void handleAPIConfig(AsyncWebServerRequest* request);
void handleAPISongs(AsyncWebServerRequest* request);
void handleAPISongsUpload(AsyncWebServerRequest* request, String filename, size_t index,
    uint8_t* data, size_t len, bool final);
void handleAPISongsUploadResponse(AsyncWebServerRequest* request);
void handleAPIPlayback(AsyncWebServerRequest* request);
void handleAPIState(AsyncWebServerRequest* request);
void handleOTAUpdateForm(AsyncWebServerRequest* request);
void handleOTAUpdateResponse(AsyncWebServerRequest* request);
void handleOTAUpdateUploadFirmware(AsyncWebServerRequest* request, String filename, size_t index,
    uint8_t* data, size_t len, bool final);
void handleOTAUpdateUploadFilesystem(AsyncWebServerRequest* request, String filename, size_t index,
    uint8_t* data, size_t len, bool final);
void handleReboot(AsyncWebServerRequest* request);
void handleCrash(AsyncWebServerRequest* request);
void handleCoredump(AsyncWebServerRequest* request);

// setup
void setup()
{
    // setup pins
    pinMode(SW0, INPUT_PULLUP);
    pinMode(SW1, INPUT_PULLUP);
    pinMode(I2S_MUTE, OUTPUT);
    pinMode(I2S_DEEM, OUTPUT);

    // setup LEDs
    FastLED.addLeds<NEOPIXEL, LED_STATUS>(led_status, 2);
    // setup parallel task
    xTaskCreatePinnedToCore(ledTask, /* Function to implement the task */
        "ledTask", /* Name of the task */
        getArduinoLoopTaskStackSize(), /* Stack size in words */
        NULL, /* Task input parameter */
        1, /* Priority of the task */
        &pLedTask, /* Task handle. */
        0); /* Core where the task should run */

    // setup serial
    Serial.begin(115200);
    // delay(5000);
    // while (!Serial)
    //     ;

    // init audio
    Serial.println("Setup I2S output");
    // AudioLogger::instance().begin(Serial,AudioLogger::Info);
    audio.init(I2S_BCLK, I2S_DOUT, I2S_WS, I2S_RADIO_OUT, SCL, SDA);
    audio.setVolume(config.GetGlobalConfig().audio_volume); // 0...1
    digitalWrite(I2S_MUTE, LOW);
    digitalWrite(I2S_DEEM, HIGH);

    // setup display
    display.clear();
    display.setBrightness(3, true);
    display.showNumberDec(1, true);

    // Initialize LITTLEFS
    Serial.println("Init flash filesystem");
    if (!LittleFS.begin(false, "/littlefs", 10, "littlefs")) {
        showDisplay(DisplayState::FS_ERR);
        Serial.println("An Error has occurred while mounting flash");
        ledSystem = SYSTEM_LED_STATE::STATE_FS_ERR;
        while (true)
            ;
    }

    listDir(LittleFS, "/", 5);

    // Initialize SD
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0, SD_MMC_D1, SD_MMC_D2, SD_MMC_D3);
    while (!SD_MMC.begin("/sdcad", false, false, 20000)) {
        showDisplay(DisplayState::SD_ERR);
        ledSystem = SYSTEM_LED_STATE::STATE_SD_ERR;
        Serial.println("Card Mount Failed");
        while (true)
            ;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        showDisplay(DisplayState::SD_ERR_NO_SD);
        ledSystem = SYSTEM_LED_STATE::STATE_SD_NO_SD;
        while (true)
            ;
    }

    // init SD card (create needed files)
    initSDCardStructure(SD_MMC);
    Serial.println("Files on SD:");
    listDir(SD_MMC, "/", 5);

    // load config
    config.LoadFromPath(fsConfig, configPath);
    audio.setVolume(config.GetGlobalConfig().audio_volume);

    // connect WiFi
    showDisplay(DisplayState::WIFI_CONNECT);
    WiFi.setAutoReconnect(true);
    WiFi.onEvent(WiFiEvent);
    // setup WiFI manager
    // wifiManager.setConnectTimeout(10);
    // wifiManager.setConfigPortalTimeout(0);
    // wifiManager.setAPCallback(configModeCallback);
    // connect to WiFi
    if (digitalRead(SW1) == LOW) {
        Serial.println("Starting AP...");
        config.GetGlobalConfig().isStaticIPEnabled = false; // disable static ip settings
        wifiManager.setTryConnectDuringConfigPortal(false); // don't connect while config portal is forced active
        if(!wifiManager.startConfigPortal(config.GetGlobalConfig().hostname.c_str())) {
            Serial.println("Cannot start config portal");
        };
    } else {
        // setup static ip if it's set in config and SW_WIFI_RESET is not pressed
        if (config.GetGlobalConfig().isStaticIPEnabled) {
            Serial.printf("Found static IP config, set IP to %s\n",
                config.GetGlobalConfig().local.toString().c_str());
            wifiManager.setSTAStaticIPConfig(config.GetGlobalConfig().local,
                config.GetGlobalConfig().gateway, config.GetGlobalConfig().subnet,
                config.GetGlobalConfig().primaryDNS, config.GetGlobalConfig().secondaryDNS);
        }
        if (!wifiManager.autoConnect()) {
            Serial.println("failed to connect, we should reset as see if it connects");
        }
    }
    Serial.println("WiFi connected!");

    // setup mDNS
    if (!MDNS.begin(config.GetGlobalConfig().hostname.c_str())) {
        Serial.println("Error setting up MDNS responder!");
    } else {
        Serial.printf("mDNS responder started with hostname %s.local\n",
            config.GetGlobalConfig().hostname.c_str());
    }

    // show IP
    Serial.println(WiFi.localIP().toString());

    // sync time
    showDisplay(DisplayState::SYNC);
    setSyncProvider(updateNTPTime);

    // print current time
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.printf("Current time: ");
        printTime(timeinfo);
        Serial.println();
    }

    // update alarms
    updateAlarms();

    // HTTP Server
    server.begin();
    server.serveStatic("/", LittleFS, "/www/");
    server.on("/api/config", handleAPIConfig);
    // server.on("/api/config/update", HTTP_POST, handleAPIConfigUpdate);
    server.on("/api/state", handleAPIState);
    server.on("/api/songs", handleAPISongs);
    server.on("/api/songs/update", HTTP_POST, handleAPISongs);
    server.on( "/upload", HTTP_POST,handleAPISongsUploadResponse, handleAPISongsUpload);
    server.on("/api/playback", handleAPIPlayback);

    // Simple Firmware Update
    server.on("/update", HTTP_GET, handleOTAUpdateForm);
    server.on(
        "/update_firmware", HTTP_POST, handleOTAUpdateResponse, handleOTAUpdateUploadFirmware);
    server.on(
        "/update_filesystem", HTTP_POST, handleOTAUpdateResponse, handleOTAUpdateUploadFilesystem);
    server.on("/reboot", HTTP_GET, handleReboot);
    server.on("/coredump", HTTP_GET, handleCoredump);

    // setup parallel task
    xTaskCreatePinnedToCore(checkAlarmTask, /* Function to implement the task */
        "checkAlarmTask", /* Name of the task */
        getArduinoLoopTaskStackSize(), /* Stack size in words */
        NULL, /* Task input parameter */
        1, /* Priority of the task */
        &pTask, /* Task handle. */
        0); /* Core where the task should run */
}

void loop()
{
    audio.loop();

    // if(millis() > 10000 && !audio.isPlaying()){
    //     Serial.println("Playing...");
    //     audio.playUrl("http://stream.srg-ssr.ch/m/rsj/mp3_128");
    //     // audio.playFile(fsSongs, "/songs/URSULA - URSULA - 11 Auf der anderen Seite.mp3");
    //     // audio.playRadio(10240);
    // }
    // digitalWrite(LED_BUILTIN, audio.isPlaying());
}

void checkAlarmTask(void* parameter)
{
    auto lastRSSI = millis();
    while (true) {
        // check for alarm and play
        checkPlayAlarm();
        // update display time
        showDisplay(DisplayState::TIME);

        // check buttons
        if (digitalRead(SW1) == LOW) {
            Serial.printf("Button SW1 pressed, stopping music.\n");
            audio.pause();
        }

        // Serial.printf("Free heap %d\n", ESP.getFreeHeap());
        // Serial.printf("Free stack %d\n", uxTaskGetStackHighWaterMark(NULL));
        if (millis() - lastRSSI > 5000) {
            // print current time
            struct tm timeinfo;
            if (getLocalTime(&timeinfo)) {
                printTime(timeinfo);
            }
            Serial.printf(
                ", uptime %ds WiFi [%d]: %s IP: %s, RSSI %d, heap %d, stack %d next alarm: %s\n",
                (millis() - startTime) / 1000, WiFi.isConnected(), WiFi.SSID().c_str(),
                WiFi.localIP().toString().c_str(), WiFi.RSSI(), ESP.getFreeHeap(),
                uxTaskGetStackHighWaterMark(NULL), config.GetNextAlarmTime().toString().c_str());
            lastRSSI = millis();
        }

        delay(100);
    }
    Serial.println("Exit task...");
}

void ledTask(void* parameter)
{
    while (true) {
        setSystemLEDState(ledSystem);
        setWiFiLEDState(ledWiFi);
        FastLED.show();
        delay(100);
    }
}

/**************************************
 *
 * initialized sd card file structure
 *
 * @param fs FileSystem (LITTLEFS, SPIFFS, SD; ...)
 *************************************/
void initSDCardStructure(fs::FS& fs)
{
    if (!fs.exists(configPath)) {
        Serial.println("Config file does not exist! Save default values to file");
        config.Save(fsConfig, configPath);
    }
    if (!fs.exists(streamsPath)) {
        Serial.println("Streams file does not exist! Creating empty file.");
        File streams = fs.open(streamsPath, FILE_WRITE);
        streams.println("[]");
        streams.close();
    }
    if (!fs.exists(songsDir)) {
        Serial.println("Songs directory does not exist! Creating empty directory.");
        fs.mkdir(songsDir);
    }
}

/**************************************
 *
 * sort and update alarms
 *
 *************************************/
void updateAlarms()
{
    // sort alarms by date
    config.SortAlarmTimes();
    Serial.println("Alarms after sort:");
    printAlarms();
    Serial.println();
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
    if (getLocalTime(&timeinfo)) {
        config.SelectNextAlarmTime(timeinfo);
        Serial.println("Current time is:");
        printTime(timeinfo);
        Serial.println();
        Serial.println("Next Alarm is " + config.GetNextAlarmTime().toString());
    }
}

/**************************************
 *
 * check and play alarm, select next one
 *
 *************************************/
bool checkPlayAlarm()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Cannot get local time");
        ledSystem = SYSTEM_LED_STATE::STATE_TIME_NOT_SYNCED;
        return false;
    }
    ledSystem = SYSTEM_LED_STATE::STATE_NONE;

    // check if timer is reached and play file
    auto& nextAlarm = config.GetNextAlarmTime();
    if (nextAlarm < timeinfo && nextAlarm.differenceSec(timeinfo) < 10) {
        Serial.println("Playing Alarm " + nextAlarm.toString());

        // stop current playback and set audio volume
        audio.stop();
        audio.setVolume(nextAlarm.getVolume());

        auto stream = nextAlarm.getStream();
        Serial.printf("Playing %s of type %s from %s with volume %.2f\n",
            nextAlarm.getName().c_str(),
            AlarmClock::MusicStream::typeToString(stream.getType()).c_str(),
            stream.getURL().c_str(), nextAlarm.getVolume());

        switch (stream.getType()) {
        case AlarmClock::MusicType::FILESYSTEM:
            audio.playFile(fsSongs, stream.getURL());
            break;
        case AlarmClock::MusicType::STREAM:
            audio.playUrl(stream.getURL());
            break;
        case AlarmClock::MusicType::FM:
            audio.playRadio(stream.getFMFrequency());
            break;
        default:
            break;
        }

        // select next alarm
        config.IncrementNextAlarmTime();
        Serial.println("Next Alarm is " + config.GetNextAlarmTime().toString());
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
        SEG_F | SEG_G | SEG_B | SEG_C, // Y
        SEG_E | SEG_G | SEG_C, // n
        SEG_E | SEG_G | SEG_D // c
    };
    const uint8_t conn[] = {
        SEG_E | SEG_G | SEG_D, // c
        SEG_E | SEG_G | SEG_D | SEG_C, // o
        SEG_E | SEG_G | SEG_C, // n
        SEG_E | SEG_G | SEG_C // n
    };
    const uint8_t sd_err[] = { SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, // S
        SEG_E | SEG_G | SEG_D | SEG_C | SEG_B, // d
        0, 0 };
    const uint8_t sd_err_no_sd[] = { SEG_E | SEG_G | SEG_C, // n
        SEG_E | SEG_G | SEG_D | SEG_C, // o
        SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, // S
        SEG_E | SEG_G | SEG_D | SEG_C | SEG_B, // d
        0, 0 };
    const uint8_t fs_err[] = { SEG_E | SEG_G | SEG_C, // n
        SEG_E | SEG_G | SEG_D | SEG_C, // o
        SEG_A | SEG_F | SEG_G | SEG_B, // F
        SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, // S
        0, 0 };
    const uint8_t ap[] = { SEG_A | SEG_F | SEG_B | SEG_B | SEG_E | SEG_C, // A
        SEG_A | SEG_F | SEG_G | SEG_B | SEG_E, // P
        0, 0 };

    switch (state) {
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
        if (!getLocalTime(&timeinfo, 1000)) {
            display.showNumberDec(0, true);
        } else {
            display.showNumberDecEx(timeinfo.tm_hour * 100 + timeinfo.tm_min, 0b01000000, true);
        }
        break;
    case DisplayState::SD_ERR:
        display.setSegments(sd_err);
        break;
    case DisplayState::SD_ERR_NO_SD:
        display.setSegments(sd_err_no_sd);
        break;
    case DisplayState::FS_ERR:
        display.setSegments(fs_err);
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
void setWiFiLEDState(WIFI_LED_STATE state)
{
    auto now = millis();
    uint8_t v = LED_MAX_BRIGHTNESS;
    switch (state) {
    case WIFI_LED_STATE::STATE_INIT:
        v = 0;
        break;
    case WIFI_LED_STATE::STATE_WIFI_CONNECTING:
        v *= (now % 2000) / 2000.;
        break;
    default:
        break;
    }

    led_status[LED_WIFI_IDX] = CHSV(state, 255, v);

    if (WiFi.isConnected()) {
        // show wifi strength
        auto rssi = WiFi.RSSI();
        auto rssiColor = 0;
        if (rssi != 0) {
            rssiColor = map(abs(rssi), 50, 90, 96, 0);
        }
        // Serial.printf("RSSI %d\n", rssi);
        led_status[LED_WIFI_IDX] = CHSV(rssiColor, 255, 70);
    }
}

void setSystemLEDState(SYSTEM_LED_STATE state)
{
    auto now = millis();
    switch (state) {
    case SYSTEM_LED_STATE::STATE_SD_ERR:
        // flash with 1 Hz
        led_status[LED_STATUS_IDX] = CHSV(HUE_RED, 255, now % 1000 < 500 ? LED_MAX_BRIGHTNESS : 0);
        break;
    case SYSTEM_LED_STATE::STATE_SD_NO_SD:
        // flash with 0.5 Hz
        led_status[LED_STATUS_IDX] = CHSV(HUE_RED, 255, now % 2000 < 1000 ? LED_MAX_BRIGHTNESS : 0);
        break;
        break;
    case SYSTEM_LED_STATE::STATE_FS_ERR:
        led_status[LED_STATUS_IDX] = CHSV(HUE_ORANGE, 255, LED_MAX_BRIGHTNESS);
        break;
    case SYSTEM_LED_STATE::STATE_TIME_NOT_SYNCED:
        led_status[LED_STATUS_IDX] = CHSV(HUE_YELLOW, 255, LED_MAX_BRIGHTNESS);
        break;
    default:
        led_status[LED_STATUS_IDX] = CHSV(0, 0, 0);
        break;
    }
}
/**************************************
 *
 * Print all falarms for debug reasons :)
 *
 *************************************/
void printAlarms()
{
    for (size_t i = 0; i < config.getAalarmTimes().size(); i++) {
        auto a = config.getAalarmTimes()[i];
        Serial.printf("[%02d] Alarm %s\n", i, a.toString().c_str());
    }
}
/**************************************
 *
 * Print all files on LITTLEFS for debug reasons :)
 *
 *************************************/
void listDir(fs::FS& fs, const char* dirname, uint8_t levels)
{
    ESP_LOGI(TAG, "Listing directory: %s\n", dirname);

    File root = fs.open(dirname, FILE_READ, false);
    if (!root) {
        ESP_LOGI(TAG, "Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        ESP_LOGI(TAG, "Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            ESP_LOGI(TAG, "  DIR : %s", file.name());
            if (levels) {
                listDir(
                    fs, (String(dirname) + String("/") + String(file.name())).c_str(), levels - 1);
            }
        } else {
            ESP_LOGI(TAG, "  FILE: %s SIZE %d", file.name(), file.size());
        }
        file = root.openNextFile();
    }
}

void configModeCallback(AsyncWiFiManager* myWiFiManager)
{
    // setLWiFiLEDState(LED_STATE::STATE_WIFI_AP);
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    Serial.println(myWiFiManager->getConfigPortalSSID());
    ledWiFi = WIFI_LED_STATE::STATE_WIFI_AP;
}

/**
 * @brief handles WiFi events and auto reconnects
 *
 */
void WiFiEvent(WiFiEvent_t event)
{
    switch (event) {
    case ARDUINO_EVENT_WIFI_AP_START:
        ESP_LOGI(TAG, "AP Started");
        ledWiFi = WIFI_LED_STATE::STATE_WIFI_AP;
        break;
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        ESP_LOGI(TAG, "AP user connected");
        ledWiFi = WIFI_LED_STATE::STATE_WIFI_AP_CONNECTED;
        break;
    case ARDUINO_EVENT_WIFI_AP_STOP:
        ESP_LOGI(TAG, "AP Stopped");
        ledWiFi = WIFI_LED_STATE::STATE_INIT;
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        ESP_LOGI(TAG, "STA Started");
        if (WiFi.getMode() == WIFI_MODE_APSTA) {
            ledWiFi = WIFI_LED_STATE::STATE_WIFI_AP;
        } else {
            ledWiFi = WIFI_LED_STATE::STATE_WIFI_CONNECTING;
        }
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        ESP_LOGI(TAG, "STA Connected");
        ledWiFi = WIFI_LED_STATE::STATE_WIFI_CONNECTED;
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        ESP_LOGI(TAG, "STA IPv6: ");
        // ESP_LOGI( TAG, "%s", WiFi.localIPv6().toString());
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        // ESP_LOGI( TAG, "STA IPv4: ");
        ESP_LOGI(TAG, "%s", WiFi.localIP().toString().c_str());
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        ESP_LOGI(TAG, "STA Disconnected -> reconnect");
        WiFi.reconnect();
        ledWiFi = WIFI_LED_STATE::STATE_WIFI_DISCONNECTED;
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        ESP_LOGI(TAG, "STA Stopped");
        break;
    default:
        break;
    }
    ESP_LOGI(TAG, "WiFi mode %d", WiFi.getMode());
}

/**
 * @brief returns the config (alarms, general config, ...) as JSON
 *
 */
void handleAPIConfig(AsyncWebServerRequest* request)
{
    // if method is POST and param exists
    if (request->method() == HTTP_POST && request->params()) {
        Serial.println("Update config");
        for (size_t i = 0; i < request->params(); i++) {
            Serial.println("Param [" + String(i) + "]: " + request->getParam(i)->value());
        }
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, request->getParam(0)->value());

        // Test if parsing succeeds.
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            request->send(500, "text/html", error.c_str());
            return;
        }

        // open file
        File file = fsConfig.open(configPath, FILE_WRITE);

        // Serialize JSON to file
        if (serializeJson(doc, file) == 0) {
            Serial.println(F("Failed to write to file"));
            request->send(500, "text/html", "Failed to write to file");
            return;
        }

        // Close the file
        file.close();

        // update config and reinit alarms
        config.LoadFromPath(fsConfig, configPath);
        updateNTPTime();
        updateAlarms();
    }

    // send loaded config
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    JsonDocument doc;
    config.Save(doc);
    serializeJsonPretty(doc, Serial);
    serializeJson(doc, *response);
    request->send(response);
}

/**
 * @brief return list of songs on LITTLEFS/SD and webstreams in streams.json
 * and adds/deletes songs/streams if action is addStream/delete. To upload a new
 * song, use handleAPISongsUpload
 *
 */
void handleAPISongs(AsyncWebServerRequest* request)
{
    // if method is POST and param exists
    if (request->method() == HTTP_POST && request->params()) {
        Serial.printf("POST with %d params\n", request->params());
        if (request->getParam(0)->isPost()) {
            // create JSON buffer
            JsonDocument req;
            DeserializationError error = deserializeJson(req, request->getParam(0)->value());
            if (error) {
                Serial.println("----- parseObject() failed -----");
                Serial.println(error.c_str());
                Serial.println(request->getParam(0)->value());
                request->send(500, "application/json",
                    "{\"error\" : \"cannot parse JSON: " + String(error.c_str()) + "\"}");
                return;
            }

            String action = req["action"];
            // Serial.println(request->getParam(0)->value());
            // Serial.println("Got Action " + action);
            // Serial.println("JSON request:");
            // serializeJson(req, Serial);

            if (action == "delete") {
                String url = req["song"]["url"];
                String name = req["song"]["name"];
                auto type = AlarmClock::MusicStream::stringToType(req["song"]["type"]);
                Serial.println("Delete song " + url);
                // delete http streams from streams.json or delete file from SD card
                if (type == AlarmClock::MusicType::FM || type == AlarmClock::MusicType::STREAM) {
                    // read current streams
                    JsonDocument doc;
                    DeserializationError error;
                    if (!readJSONFile(fsSongs, streamsPath, doc, error)) {
                        Serial.println("----- parseObject() for streams.json failed -----");
                        Serial.println(error.c_str());
                        request->send(
                            500, "application/json", "{\"error\" : \"cannot read streams\"}");
                        return;
                    }
                    // find stream/fm on array, delete and write back
                    Serial.println("DOC before");
                    auto streams = doc.as<JsonArray>();
                    for (JsonArray::iterator it = streams.begin(); it != streams.end(); ++it) {
                        if ((*it)["url"] == url && (*it)["name"] == name) {
                            streams.remove(it);
                            break;
                        }
                    }
                    if (!writeJSONFile(fsSongs, streamsPath, doc)) {
                        request->send(
                            500, "application/json", "{\"error\" : \"cannot save streams\"}");
                        return;
                    }
                } else {
                    url = songsDir + url;
                    Serial.printf(
                        "Delete song %s (exists: %d)\n", url.c_str(), fsSongs.exists(url));
                    if (!fsSongs.remove(url)) {
                        request->send(
                            500, "application/json", "{\"error\" : \"cannot remove song\"}");
                    }
                }
            } else if (action == "addStream") {
                // add new stream / fm station
                // read current streams
                JsonDocument doc;
                JsonArray streams = doc.to<JsonArray>();
                DeserializationError error;
                if (!readJSONFile(fsSongs, streamsPath, doc, error)) {
                    Serial.println("----- parseObject() for streams.json failed -----");
                    Serial.println(error.c_str());
                    request->send(500, "application/json",
                        "{\"error\" : \"cannot read streams: " + String(error.c_str()) + "\"}");
                    return;
                }
                // copy only necessary fields
                JsonObject stream = streams.add<JsonObject>();
                stream["name"] = req["stream"]["name"];
                stream["url"] = req["stream"]["url"];
                stream["type"] = req["stream"]["type"];

                // add new stream
                // streams.add(stream);

                // Serial.println();
                // serializeJson(streams, Serial);
                // Serial.println();

                // write back to streams
                if (!writeJSONFile(fsSongs, streamsPath, doc)) {
                    request->send(500, "application/json", "{\"error\" : \"cannot save streams\"");
                    return;
                }
            } else if (action == "addSong") {
                // upload new song to sd card is handled by handleAPISongsUpload()
            }
        }
    }
    // create JSON buffer and read streams
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();
    DeserializationError error;
    if (!readJSONFile(fsSongs, streamsPath, doc, error)) {
        Serial.println("----- parseObject() for streams.json failed -----");
        Serial.println(error.c_str());
        request->send(500, "application/json", "{\"error\" : \"cannot read streams\"");
    }

    // iterate through songs directory
    File root = fsSongs.open("/songs");
    File file = root.openNextFile();
    while (file) {
        Serial.println(file.name());
        auto f = array.add<JsonObject>();
        f["name"] = String(file.name()).substring(String(file.name()).lastIndexOf("/") + 1);
        f["url"] = String(file.name());
        f["size"] = file.size();
        f["type"] = AlarmClock::MusicStream::typeToString(AlarmClock::MusicType::FILESYSTEM);
        file = root.openNextFile();
    }

    // send JSON
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    serializeJsonPretty(doc, Serial);
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
void handleAPISongsUpload(AsyncWebServerRequest* request, String filename, size_t index,
    uint8_t* data, size_t len, bool final)
{
    // Serial.printf("Uploading %s (%u bytes written, blocksize %u bytes)\n", filename.c_str(),
    // index, len);

    String path = songsDir + filename;

    // open file if there is no other file opened
    if (!index) {
        Serial.printf("Open file %s\n", filename.c_str());
        if (fsUploadFile) {
            Serial.println("File is already opened!");
            request->send(500, "text/plain", "file already opened");
            return;
        } else {
            fsUploadFile = fsSongs.open(path, FILE_WRITE, true);
            if (!fsUploadFile) {
                Serial.println("Cannot open file");
                request->send(500, "text/plain", "Cannot open file " + path);
            }
        }
    }

    // write data
    fsUploadFile.write(data, len);

    // close file
    if (final) {
        Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
        JsonDocument buffer;
        buffer["name"] = filename;
        buffer["url"] = filename;
        buffer["size"] = fsUploadFile.position();
        fsUploadFile.close();
        // send JSON response
        AsyncResponseStream* response = request->beginResponseStream("application/json");
        serializeJson(buffer, *response);
        serializeJson(buffer, Serial);
        request->send(response);
    }
}

/**
 * 
 */
void handleAPISongsUploadResponse(AsyncWebServerRequest* request) {
    // do nothing, so upload handler can handle the request
}

/**
 * @brief If POST data contains JSON object with play/pause/stop/volume action it will
 * play/pause/stop song or set volume. Always returns JSON with current audio state
 * (playing, postion, duration, volume, ...)
 *
 * @param request
 */
void handleAPIPlayback(AsyncWebServerRequest* request)
{
    // if method is POST and param exists
    if (request->method() == HTTP_POST && request->params()) {
        if (request->getParam(0)->isPost()) {
            // Serial.println(request->getParam(0)->value());
            // create JSON buffer
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, request->getParam(0)->value());
            if (error) {
                Serial.println("----- parseObject() failed -----");
                Serial.println(error.c_str());
                request->send(500, "application/json", "{\"error\" : \"cannot parse JSON\"");
                return;
            }

            String action = doc["action"];

            if (action == "play") {
                String url = doc["url"];
                float volume = doc["volume"] | config.GetGlobalConfig().audio_volume;
                Serial.printf("Playing song %s with volume %f\n", url.c_str(), volume);
                // first stop current audio and set volume
                audio.stop();
                audio.setVolume(volume);
                // play media
                if (url.startsWith("http")) {
                    audio.playUrl(url);
                } else if (fsSongs.exists(songsDir + url)) {
                    audio.playFile(fsSongs, songsDir + url);
                } else if (url.toFloat()) {
                    Serial.printf("Set radio frequency to %.2f\n", url.toFloat());
                    audio.playRadio((uint16_t)(url.toFloat() * 100));
                } else {
                    File file = fsSongs.open(url, FILE_READ);
                    if (!file) {
                        String errorMsg = "{\"error\" : \"cannot open file " + url + "\"}";
                        request->send(500, "application/json", errorMsg);
                        file.close();
                        return;
                    }
                    file.close();
                    audio.playFile(fsSongs, url);
                }
            } else if (action == "stop") {
                audio.stop();
            } else if (action == "pause") {
                if (audio.isPlaying()) {
                    audio.pause();
                } else {
                    audio.resume();
                }
            } else if (action == "volume") {
                audio.setVolume(doc["volume"].as<float>());
            }
        }
    }

    // create JSON response
    JsonDocument state;
    Serial.printf("Get volume %f\n", audio.getVolume());
    state["volume"] = audio.getVolume();
    state["position"] = audio.getFilePosition();
    state["current"] = audio.getCurrentTime();
    state["duration"] = audio.getTotalTime();
    state["playing"] = audio.isPlaying();

    // send JSON response
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    // serializeJson(state, Serial);
    serializeJson(state, *response);
    request->send(response);
}

/**
 * @brief Sends JSON with heap size, cpu speed, used flash/sd space etc.
 *
 * @param request
 */
void handleAPIState(AsyncWebServerRequest* request)
{

    // create JSON response
    JsonDocument doc;
    doc["heap_free"] = ESP.getFreeHeap();
    doc["heap_size"] = ESP.getHeapSize();
    doc["chip_rev"] = ESP.getChipRevision();
    doc["sdk"] = ESP.getSdkVersion();
    doc["wifi"] = WiFi.RSSI();
    doc["flash_used"] = LittleFS.usedBytes();
    doc["flash_total"] = LittleFS.totalBytes();
    doc["sd_used"] = int64String(SD_MMC.usedBytes());
    doc["sd_total"] = int64String(SD_MMC.totalBytes());
    doc["cpu_frequ"] = ESP.getCpuFreqMHz();
    doc["alarm_next"] = config.GetNextAlarmTime().toString();

    // send JSON response
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    serializeJson(doc, Serial);
    serializeJson(doc, *response);
    request->send(response);
}

void handleOTAUpdateForm(AsyncWebServerRequest* request)
{
    request->send(200, "text/html",
        "<form method='POST' action='/update_firmware' enctype='multipart/form-data'>"
        "Firmware: <input type='file' name='firmware'>"
        "<input type='submit' value='Update firmware'></form>"
        "<form method='POST' action='/update_filesystem' enctype='multipart/form-data'>"
        "Filesystem: <input type='file' name='filesystem'>"
        "<input type='submit' value='Update filesystem'></form>");
}

void handleOTAUpdateResponse(AsyncWebServerRequest* request)
{
    bool shouldReboot = !Update.hasError();
    AsyncWebServerResponse* response
        = request->beginResponse(200, "text/plain", shouldReboot ? "OK" : "FAIL");
    response->addHeader("Refresh", "20");
    response->addHeader("Location", "/");
    request->send(response);
}

void handleOTAUpdateUploadFirmware(AsyncWebServerRequest* request, String filename, size_t index,
    uint8_t* data, size_t len, bool final)
{
    if (filename != "firmware.bin") {
        return;
    }

    if (!index) {
        Serial.printf("Update firmware start: %s\n", filename.c_str());
        //   Update.runAsync(true);
        if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
            Update.printError(Serial);
        } else {
            Serial.println("Update firmware begin...");
        }
    }
    if (!Update.hasError()) {
        float progress = (index + len) / (float)request->contentLength();
        Serial.printf("writing...%d bytes (%d/%d, %.2f%%)\n", len, index + len,
            request->contentLength(), progress * 100);
        if (Update.write(data, len) != len) {
            Update.printError(Serial);
        }

        led_status[LED_STATUS_IDX] = CHSV(map(progress * 100, 0, 100, 0, 80), 255, 70);
        FastLED.show();
    }
    if (final) {
        if (Update.end(true)) {
            Serial.printf("Update firmware success: %uB\n", index + len);
            ESP.restart();
        } else {
            Update.printError(Serial);
        }
    }
}

void handleOTAUpdateUploadFilesystem(AsyncWebServerRequest* request, String filename, size_t index,
    uint8_t* data, size_t len, bool final)
{
    if (filename != "littlefs.bin") {
        return;
    }

    if (!index) {
        Serial.printf("Update filesystem start: %s\n", filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
            Update.printError(Serial);
        } else {
            Serial.println("Update filesystem begin...");
        }
    }
    if (!Update.hasError()) {
        float progress = (index + len) / (float)request->contentLength();
        Serial.printf("writing...%d bytes (%d/%d, %.2f%%)\n", len, index + len,
            request->contentLength(), progress * 100);
        if (Update.write(data, len) != len) {
            Update.printError(Serial);
        }

        led_status[LED_STATUS_IDX] = CHSV(map(progress * 100, 0, 100, 0, 80), 255, 70);
        FastLED.show();
    }
    if (final) {
        if (Update.end(true)) {
            Serial.printf("Update filesystem success: %uB\n", index + len);
            ESP.restart();
        } else {
            Update.printError(Serial);
        }
    }
}

void handleReboot(AsyncWebServerRequest* request)
{
    request->send(
        200, "text/html", "Rebooting...please try to reload page in a couple of seconds.");
    ESP.restart();
}

void handleCoredump(AsyncWebServerRequest* request)
{
    auto resp = request->getResponse();

    esp_partition_iterator_t partition_iterator = esp_partition_find(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, "coredump");

    const esp_partition_t* partition = esp_partition_get(partition_iterator);

    // determine partition size (0 if not found)
    size_t file_size = partition ? partition->size : 0;
    const size_t chunk_size = 1024;

    // release iterator as we keep the partition pointer
    if (partition_iterator) {
        esp_partition_iterator_release(partition_iterator);
    }

    AsyncWebServerResponse* response = request->beginChunkedResponse("application/octet-stream",
        [partition, file_size, chunk_size](uint8_t* buffer, size_t maxlen, size_t offset) -> size_t {
            Serial.printf("maxlen=%d, offset=%d\n", maxlen, offset);
            if (!partition) {
                Serial.printf("No partition...\n");
                return 0;
            }
            // compute offset and remaining bytes
            if (offset >= file_size) {
                Serial.printf("Offset >= file_size %d>=%d\n", offset, file_size);
                return 0;
            }
            size_t bytesToRead
                = (file_size - offset) > chunk_size ? chunk_size : (file_size - offset);
            if (bytesToRead > maxlen) {
                bytesToRead = maxlen;
            }

            esp_err_t ret = esp_partition_read(partition, offset, buffer, bytesToRead);
            if (ret != ESP_OK) {
                Serial.printf("esp_partition_read failed: %d\n", ret);
                return 0;
            }
            Serial.printf(
                "Sending %u bytes from offset %u\n", (unsigned)bytesToRead, (unsigned)offset);
            return bytesToRead;
        });
    response->addHeader("Content-Disposition", "attachment;filename=core.bin");

    request->send(response);
}

/**************************************
 *
 * NTP time sync provider
 *
 *************************************/
time_t updateNTPTime()
{
    struct tm timeinfo;
    if (!WiFi.isConnected()) {
        Serial.println("WiFi not connected, cannot config time!");
        return mktime(&timeinfo);
    }
    // configTime(-3600, -3600, "69.10.161.7");

    Serial.printf("Update time with TZ: %s\n", config.GetPOSIXTimezone().c_str());

    configTime(0, 0, "0.de.pool.ntp.org", "1.de.pool.ntp.org", "69.10.161.7");

    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
    } else {
        Serial.print("Synced time to: ");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    }

    //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
    Serial.print("Time before TZ set: ");
    printTime(timeinfo);
    Serial.println();
    setenv("TZ", config.GetPOSIXTimezone().c_str(), 1);
    tzset();
    Serial.print("Time after TZ set: ");
    printTime(timeinfo);
    Serial.println();

    return mktime(&timeinfo);
}

/**************************************
 *
 * print date and time
 *
 *************************************/
void printTime(struct tm info)
{
    String tz = getenv("TZ");
    Serial.printf("%s %02d.%02d.%d %02d:%02d:%02d %s, dst: %d", dowName(info.tm_wday).c_str(),
        info.tm_mday, info.tm_mon + 1, info.tm_year + 1900, info.tm_hour, info.tm_min, info.tm_sec,
        tz.c_str(), info.tm_isdst);
}

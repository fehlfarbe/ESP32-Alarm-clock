#include "utils.h"

/**************************************
 *
 * get dow name
 *
 *************************************/
String dowName(uint8_t dow)
{
    switch (dow)
    {
    case 0:
        return "Sunday";
    case 1:
        return "Monday";
    case 2:
        return "Tuesday";
    case 3:
        return "Wednesday";
    case 4:
        return "Thursday";
    case 5:
        return "Friday";
    case 6:
        return "Saturday";
    }

    return "Unknown";
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
bool readJSONFile(fs::FS &fs, String filePath, JsonDocument &doc, DeserializationError &error)
{
    // add webstreams from streams
    File file = fs.open(filePath, FILE_READ, false);
    String json = file.readString();
    file.close();
    Serial.println(json);
    error = deserializeJson(doc, json);
    if (error)
    {
        Serial.printf("----- parseObject() for %s failed -----\n", filePath.c_str());
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
bool writeJSONFile(fs::FS &fs, String filePath, JsonDocument &doc)
{
    // open file
    File f = fs.open(filePath, FILE_WRITE, true);

    // Serialize JSON to file
    if (serializeJson(doc, f) == 0)
    {
        Serial.printf("Failed to writing to file %s\n", filePath.c_str());
        return false;
    }

    // Close the file
    f.close();
    return true;
}
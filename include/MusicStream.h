#ifndef MUSICSTREAM_H
#define MUSICSTREAM_H
#include <Arduino.h>

namespace AlarmClock
{

    enum MusicType
    {
        UNKNOWN,
        FILESYSTEM,
        STREAM,
        FM
    };

    class MusicStream
    {
    public:
        MusicStream();
        MusicStream(String name, String url, MusicType type);
        ~MusicStream();

        String toString();
        static String typeToString(MusicType t);
        static MusicType stringToType(String t);
        bool isFile();
        bool isStream();
        bool isFM();

        String getURL();
        uint16_t getFMFrequency();
        MusicType getType();

    private:
        String name;
        String url;
        MusicType type;
    };
}

#endif // MUSICSTREAM_H
#include <MusicStream.h>

MusicStream::MusicStream()
{
}

MusicStream::MusicStream(String name, String url, MusicType type) : name(name), url(url), type(type)
{
}

MusicStream::~MusicStream()
{
}

String MusicStream::toString()
{
    return name + " (" + typeToString(type) + ")";
}

String MusicStream::typeToString(MusicType t)
{
    switch (t)
    {
    case MusicType::FILESYSTEM:
        return "file";
    case MusicType::STREAM:
        return "stream";
    case MusicType::FM:
        return "fm";
    case MusicType::UNKNOWN:
        break;
    }

    return "unknown";
}

MusicType MusicStream::stringToType(String t)
{
    if (t == "file")
    {
        return MusicType::FILESYSTEM;
    }
    if (t == "stream")
    {
        return MusicType::STREAM;
    }
    if (t == "fm")
    {
        return MusicType::FM;
    }

    return MusicType::UNKNOWN;
}

bool MusicStream::isFile()
{
    return type == MusicType::FILESYSTEM;
}

bool MusicStream::isStream()
{
    return type == MusicType::STREAM;
}

bool MusicStream::isFM()
{
    return type == MusicType::FM;
}

String MusicStream::getURL()
{
    return url;
}

int MusicStream::getFMFrequency()
{
    return url.toInt();
}

MusicType MusicStream::getType()
{
    return type;
}
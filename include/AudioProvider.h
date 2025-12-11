#ifndef AUDIOPROVIDER_H
#define AUDIOPROVIDER_H

#include <AudioTools.h>
#include <FS.h>
#include <RDA5807FP.h>
#include <AudioTools/Communication/HTTP/URLStream.h>
#include <AudioTools/AudioCodecs/CodecMP3Helix.h>


enum PlayMode { PLAY_STREAM, PLAY_RADIO };

enum MediaType { NONE, STREAM, AUDIOFILE, FM };

struct Media {
    String source;
    fs::FS* filesystem;
    MediaType type;
};

class AudioProvider {
public:
    AudioProvider();
    ~AudioProvider();

    bool init(uint8_t pinBCK, uint8_t pinData, uint8_t pinWS, uint8_t pinDin, uint8_t scl = SCL,
        uint8_t sda = SDA);

    void playUrl(String url);
    void playFile(fs::FS& fs, String path);
    void playRadio(uint16_t freq);

    void pause();
    void resume();
    void stop();
    bool isPlaying();

    void setVolume(float vol);
    float getVolume();

    uint32_t getFilePosition();
    uint32_t getCurrentTime();
    uint32_t getTotalTime();

    void loop();

protected:
    void initI2S(uint8_t pinBCK, uint8_t pinData, uint8_t pinWS, uint8_t pinDin);
    void initRadio(uint8_t scl = SCL, uint8_t sda = SDA);
    void resetStreams();

    RDA5807FP radio;
    I2SStream i2s;
    URLStream urlStream;
    MP3DecoderHelix mp3decoder;
    VolumeStream volumeStream;
    EncodedAudioStream decoder = EncodedAudioStream(&volumeStream, &mp3decoder); // Decoding stream
    StreamCopy copier; // copy url to decoder

    uint16_t sample_rate = 44100; // 44.1 kHz
    uint8_t channels = 2; // The stream will have 2 channels
    float volume = 0.f; // volume in VolumeStream has bug

    bool playing = false;
    Media nextMedia;
    fs::File currentAudioFile;
    PlayMode playMode;

    SemaphoreHandle_t playbackMutex = nullptr;
};

#endif // AUDIOPROVIDER_H
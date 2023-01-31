#ifndef AUDIOPROVIDER_H
#define AUDIOPROVIDER_H

#include <FS.h>
#include <radio.h>
#include <RDA5807FP.h>
#include <AudioTools.h>
// #include <AudioCodecs/CodecMP3Helix.h>

enum PlayMode {
    PLAY_STREAM,
    PLAY_RADIO
};


class AudioProvider
{
public:
    AudioProvider(uint8_t pinBCK, uint8_t pinData, uint8_t pinWS, uint8_t pinDin);
    ~AudioProvider();

    void playUrl(String url);
    void playFile(FS fs, String path);
    void playRadio(uint16_t freq);

    void pause();
    void resume();
    void stop();
    bool isPlaying();

    void setVolume(uint8_t vol);
    uint8_t getVolume();

    uint32_t getFilePosition();
    uint32_t getCurrentTime();
    uint32_t getTotalTime();

    void loop();

protected:

    void initI2S(uint8_t pinBCK, uint8_t pinData, uint8_t pinWS, uint8_t pinDin);
    void initRadio();

    RDA5807FP radio;
    I2SStream i2s;
    URLStream urlStream;
    EncodedAudioStream *dec = nullptr;// Decoding stream
    StreamCopy *copierRadio = nullptr;// copy url to decoder
    StreamCopy *copierUrl = nullptr;  // copy url to decoder

    uint16_t sample_rate = 44100;
    uint8_t channels = 2;                          // The stream will have 2 channels

    uint8_t volume = 0;
    bool playing = false;
    PlayMode playMode = PLAY_STREAM;

};

#endif // AUDIOPROVIDER_H
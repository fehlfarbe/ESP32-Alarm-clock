#include "AudioProvider.h"
// #include <AudioCodecs/CodecMP3MAD.h>
#include <AudioCodecs/CodecMP3Helix.h>


AudioProvider::AudioProvider()
{
    AudioLogger::instance().begin(Serial, AudioLogger::Warning); 
    // setup decoder
    decoder.setOutput(&volumeStream);
    MP3DecoderHelix *mp3decoder = new MP3DecoderHelix();
    mp3decoder->setMaxFrameSize(4800); // increase mp3 frame size buffer to prevent stuttering
    decoder.setDecoder(mp3decoder);

    // increase URKL Stream buffer size
    urlStream.setReadBufferSize(4096);

    // setup volumeStream
    VolumeStreamConfig cfg;
    cfg.channels = channels;
    cfg.bits_per_sample = 16;
    cfg.sample_rate = sample_rate;
    cfg.volume = 1.0f;
    volumeStream.begin(cfg);
    volumeStream.setTarget(i2s);
}

AudioProvider::~AudioProvider()
{
}

bool AudioProvider::init(uint8_t pinBCK, uint8_t pinData, uint8_t pinWS, uint8_t pinDin, uint8_t scl, uint8_t sda)
{
    // init I2S
    initI2S(pinBCK, pinData, pinWS, pinDin);

    // init radio
    initRadio(scl, sda);

    return true;
}

void AudioProvider::playUrl(String url)
{
    nextMedia = {url, nullptr, STREAM};
}

void AudioProvider::playFile(fs::FS &fs, String path)
{
    nextMedia = {path, &fs, AUDIOFILE};
}

void AudioProvider::playRadio(uint16_t freq)
{
    nextMedia = {String(freq), nullptr, FM};
}

void AudioProvider::pause()
{
    playing = false;
}

void AudioProvider::resume()
{
    playing = true;
}

void AudioProvider::stop()
{
    playing = false;
}

bool AudioProvider::isPlaying()
{
    return playing;
}

void AudioProvider::setVolume(float vol)
{
    Serial.printf("Set volume to %.2g\n", vol);
    volume = vol;
    volumeStream.setVolume(vol);
}

float AudioProvider::getVolume()
{
    // Serial.printf("Get volume ch0 %.2f\n", volumeStream.volume(0));
    // Serial.printf("Get volume ch1 %.2f\n", volumeStream.volume(1));
    return volume;
}

uint32_t AudioProvider::getFilePosition()
{
    return 0;
}

uint32_t AudioProvider::getCurrentTime()
{
    return 0;
}

uint32_t AudioProvider::getTotalTime()
{
    return 0;
}

void AudioProvider::loop()
{
    if (nextMedia.type != NONE)
    {
        // mute and cleanup first
        radio.setMute(true);
        copier.end();
        decoder.end();
        urlStream.end();

        // open next media
        switch (nextMedia.type)
        {
        case STREAM:
            // activate decoder
            decoder.setNotifyAudioChange(i2s);
            decoder.begin();
            // open URL
            urlStream.begin(nextMedia.source.c_str(), "audio/mp3");
            copier.begin(decoder, urlStream);
            // set state
            playMode = PLAY_STREAM;
            playing = true;
            break;
        case AUDIOFILE:
        {
            // activate decoder
            decoder.setNotifyAudioChange(i2s);
            decoder.begin();
            // open file
            Serial.printf("Open %s\n", nextMedia.source.c_str());
            if (!nextMedia.filesystem->exists(nextMedia.source.c_str()))
            {
                Serial.println("File does not exist!");
                break;
            }
            currentAudioFile = nextMedia.filesystem->open(nextMedia.source.c_str());
            copier.begin(decoder, currentAudioFile);
            // set state
            playMode = PLAY_STREAM;
            playing = true;
            break;
        }
        case FM:
        {
            // setup radio
            uint16_t freq = nextMedia.source.toInt();
            Serial.printf("Playing radio %d\n", freq);
            radio.setFrequency(freq);
            radio.setMute(false);
            copier.begin(volumeStream, i2s);
            // set state
            playMode = PLAY_RADIO;
            playing = true;
            break;
        }
        default:
            playing = false;
            break;
        }

        // media already opened, set nextMedia to NONE
        nextMedia.type = NONE;
    }

    if (playing)
    {
        copier.copy();
    }
}

void AudioProvider::initI2S(uint8_t pinBCK, uint8_t pinData, uint8_t pinWS, uint8_t pinDin)
{
    // start I2S
    auto config = i2s.defaultConfig(RXTX_MODE);
    config.pin_bck = pinBCK;
    config.pin_data = pinData;
    config.pin_ws = pinWS;
    config.pin_data_rx = pinDin;
    config.auto_clear = true;
    i2s.begin(config);
}

void AudioProvider::initRadio(uint8_t scl, uint8_t sda)
{
    // list devices
    Wire.begin(sda, scl);
    // Wire.begin();
    byte error, address; // variable for error and I2C address
    int nDevices = 0;

    // Serial.println("Scanning...");

    //   for (address = 1; address < 127; address++)
    //   {
    //     // The i2c_scanner uses the return value of
    //     // the Write.endTransmisstion to see if
    //     // a device did acknowledge to the address.
    //     Wire.beginTransmission(address);
    //     error = Wire.endTransmission();

    //     if (error == 0)
    //     {
    //       Serial.print("I2C device found at address 0x");
    //       if (address < 16)
    //         Serial.print("0");
    //       Serial.print(address, HEX);
    //       Serial.println("  !");
    //       nDevices++;
    //     }
    //     else if (error == 4)
    //     {
    //       Serial.print("Unknown error at address 0x");
    //       if (address < 16)
    //         Serial.print("0");
    //       Serial.println(address, HEX);
    //     }
    //   }
    //   if (nDevices == 0)
    //   {
    //     Serial.println("No I2C devices found\n");
    //   }

    // Serial.printf("Free heap %d\n", ESP.getFreeHeap());
    // Serial.printf("Free stack %d\n", uxTaskGetStackHighWaterMark(NULL));

    RDA5807FP_I2SConfig config;
    config.enabled = true;
    config.slave = true;
    config.rate = RDA5807FP_I2S_WS_CNT::WS_STEP_44_1;
    config.data_signed = true;

    //   radio.debugEnable(true);
    radio.initWire(Wire);
    radio.init();
    radio.SetupI2S(config);

    radio.setBandFrequency(RADIO_BAND_FM, 9020); // preset.

    radio.setMono(false);
    radio.setMute(false);
    Serial.println("Init radio finished!");
    //   radio.setVolume(volume);
}

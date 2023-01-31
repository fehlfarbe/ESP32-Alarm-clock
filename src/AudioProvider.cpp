#include "AudioProvider.h"
#include <AudioCodecs/CodecMP3MAD.h>

AudioProvider::AudioProvider(uint8_t pinBCK, uint8_t pinData, uint8_t pinWS, uint8_t pinDin)
{
    // audio stream stuff
    dec = new EncodedAudioStream(&i2s, new MP3DecoderMAD());
    copierRadio = new StreamCopy(i2s, i2s);
    copierUrl = new StreamCopy(*dec, urlStream);

    initI2S(pinBCK, pinData, pinWS, pinDin);

    // init radio
    initRadio();
}

AudioProvider::~AudioProvider()
{
    delete dec;
    delete copierRadio;
    delete copierUrl;
}

void AudioProvider::playUrl(String url)
{
    // activate decoder
    dec->setNotifyAudioChange(i2s);
    dec->begin();
    // open URL
    urlStream.begin(url.c_str(), "audio/mp3");
    // set state
    playMode = PLAY_STREAM;
    playing = true;
}

void AudioProvider::playFile(FS fs, String path)
{
    // ToDo
}

void AudioProvider::playRadio(uint16_t freq)
{
    // stop url player
    dec->end();
    // setup radio
    radio.setFrequency(freq);
    // set state
    playMode = PLAY_RADIO;
    playing = true;
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

void AudioProvider::setVolume(uint8_t vol)
{
    volume = vol;
}

uint8_t AudioProvider::getVolume()
{
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
    if(playing) {
        switch(playMode){
            case PLAY_STREAM:
            copierUrl->copy();
            break;
            case PLAY_RADIO:
            copierRadio->copy();
            break;
        }
    }
}

void AudioProvider::initI2S(uint8_t pinBCK, uint8_t pinData, uint8_t pinWS, uint8_t pinDin)
{
    // start I2S
  auto config = i2s.defaultConfig(RXTX_MODE);
  config.sample_rate = sample_rate;
  config.channels = channels;
  config.bits_per_sample = 16;
  // config.i2s_format = I2S_STD_FORMAT;
  config.pin_bck = pinBCK;
  config.pin_data = pinData;
  config.pin_ws = pinWS;
  config.pin_data_rx = pinDin;
  i2s.begin(config);
}

void AudioProvider::initRadio()
{
    RDA5807FP_I2SConfig config;
  config.enabled = true;
  config.slave = true;
  config.rate = RDA5807FP_I2S_WS_CNT::WS_STEP_44_1;
  config.data_signed = true;
//   radio.SetupI2S(config);

//   radio.setBandFrequency(RADIO_BAND_FM, 9020); // preset.

//   radio.setMono(false);
//   radio.setMute(false);
//   radio.setVolume(volume);
}

#include <Audio.h>
#include "Envelope.h"
#include "samples.h"

class Hat : public InstrumentI
{
public:
  Hat(float _decay)
  {
    Value frequencyValue(30.0, 200.0, 40.0, "TOM", 100, true);
    values.push_back(frequencyValue);

    Value decayValue(2.0, 1000.0, 100.0, "DURACAO", 100, true);
    values.push_back(decayValue);

    Value highpassValue(200.0, 10000.0, 7000.0, "HIGH PASS", 100, true);
    values.push_back(highpassValue);

    Value bandpassValue(200.0, 10000.0, 10000.0, "BAND PASS", 100, true);
    values.push_back(bandpassValue);

    Value noiseValue(0.0, 3.0, 0.9, "NOISE", 30);
    values.push_back(noiseValue);

    Value attackValue(2.0, 100.0, 2.0, "ATAQUE", 98, true);
    values.push_back(attackValue);

    Value indexValue(0, 4, 0, "INDICE", 4);
    values.push_back(indexValue);

    setupParams();
    outMixer.gain(0, 7);
    decayTime = _decay;

    patch1 = new AudioConnection(sq1, 0, mixer1, 0);
    patch2 = new AudioConnection(sq2, 0, mixer1, 1);
    patch3 = new AudioConnection(sq3, 0, mixer1, 2);
    patch4 = new AudioConnection(sq4, 0, mixer1, 3);
    patch5 = new AudioConnection(mixer1, 0, mixer2, 0);
    patch6 = new AudioConnection(sq5, 0, mixer2, 1);
    patch7 = new AudioConnection(sq6, 0, mixer2, 2);
    patch8 = new AudioConnection(noise, 0, mixer2, 3);
    patch9 = new AudioConnection(mixer2, filter);
    patch11 = new AudioConnection(filter, 0, ampAM, 0);
    patch12 = new AudioConnection(ampEnv, 0, ampAM, 1);
    patch14 = new AudioConnection(ampAM, 0, outMixer, 0);
    patch15 = new AudioConnection(wav, 0, outMixer, 1);
  }

  void setIndex(byte idx)
  {
    index = idx;
  }

  void noteOn(byte velocity)
  {

    s_out.gain(velocity * velocity / 16129.0f);
    if (floor(values[6].value) == 0)
    {
      ampEnv.noteOn();
    }
    else if (floor(values[6].value) == 1)
    {
      //        wav.play(fileName.c_str());
      wav.play(AudioSampleHat1);
    }
    else
    {
      wav.play(AudioSampleHat2);
    }
  }

  void setupParams()
  {
    filter.setBandpass(0, values[3].value, 1.95);
    filter.setHighpass(1, values[2].value, 1);
    ampEnv.setCoefficients(values[1].value, 0.001, values[5].value);
    sq1.begin(1, values[0].value * 2, WAVEFORM_SQUARE);
    sq2.begin(1, values[0].value * 3, WAVEFORM_SQUARE);
    sq3.begin(1, values[0].value * 4.16, WAVEFORM_SQUARE);
    sq4.begin(1, values[0].value * 5.43, WAVEFORM_SQUARE);
    sq5.begin(1, values[0].value * 6.79, WAVEFORM_SQUARE);
    sq6.begin(1, values[0].value * 8.21, WAVEFORM_SQUARE);
    noise.amplitude(values[4].value);

    // fileName = "HAT";
    // fileName += (int)floor(values[6].value);
    // fileName += ".WAV";
  }

  //    AudioMixer4      outMixer;
  float baseFrequency = 40;
  float decayTime = 100;
  float highpass = 7000;
  float bandPass = 10000;
  float noiseAmount = 0.9;
  float attackTime = 2.0;

private:
  AudioMixer4 mixer1;
  AudioMixer4 mixer2;
  AudioSynthWaveform sq1;
  AudioSynthWaveform sq2;
  AudioSynthWaveform sq3;
  AudioSynthWaveform sq4;
  AudioSynthWaveform sq5;
  AudioSynthWaveform sq6;
  AudioSynthNoiseWhite noise;
  AudioFilterBiquad filter;
  Envelope ampEnv;
  AudioEffectMultiply ampAM;
  AudioPlayMemory wav;

  AudioConnection *patch1;
  AudioConnection *patch2;
  AudioConnection *patch3;
  AudioConnection *patch4;
  AudioConnection *patch5;
  AudioConnection *patch6;
  AudioConnection *patch7;
  AudioConnection *patch8;
  AudioConnection *patch9;
  //    AudioConnection*    patch10;
  AudioConnection *patch11;
  AudioConnection *patch12;
  //    AudioConnection*    patch13;
  AudioConnection *patch14;
  AudioConnection *patch15;
  byte index = 0;
};

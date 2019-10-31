#include <Audio.h>
#include "Envelope.h"
#include "instrumentInterface.h"

class Kick : public InstrumentI
{
public:
  Kick()
  {
    sine.amplitude(1);
    Value attackValue(2.0, 200.0, 2.0, "ATAQUE", 100); // 0
    values.push_back(attackValue);

    Value noiseValue(0.0, 3.0, 0.2, "NOISE", 30); // 1
    values.push_back(noiseValue);

    Value frequencyValue(30.0, 300.0, 55, "TOM", 135); // 2
    values.push_back(frequencyValue);

    Value lengthValue(30.0, 1000.0, 200.0, "DURACAO", 100); // 3
    values.push_back(lengthValue);

    Value shapeValue(0.01, 0.99, 0.3, "FORMA", 100); // 4
    values.push_back(shapeValue);

    Value cutoffValue(30.0, 4000.0, 440.0, "FILTRO", 100); // 5
    values.push_back(cutoffValue);

    Value indexValue(0, 2, 0, "INDICE", 2); // 6
    values.push_back(indexValue);

    setupParams();

    patch1 = new AudioConnection(pitchEnv, sine);
    patch2 = new AudioConnection(sine, 0, sineAM, 0);
    patch3 = new AudioConnection(ampEnv, 0, sineAM, 1);
    patch4 = new AudioConnection(noiseMixer, filter);
    patch5 = new AudioConnection(noiseEnv, 0, noiseAM, 0);
    patch6 = new AudioConnection(noise, 0, noiseAM, 1);
    patch7 = new AudioConnection(noiseAM, 0, noiseMixer, 0);
    patch8 = new AudioConnection(sineAM, 0, noiseMixer, 1);
    patch9 = new AudioConnection(filter, 0, outMixer, 0);
    patch9 = new AudioConnection(wav, 0, outMixer, 1);
  }

  void noteOn(byte velocity)
  {
    s_out.gain(velocity * velocity / 16129.0f);
    if (floor(values[6].value) == 0)
    {
      pitchEnv.noteOn();
      ampEnv.noteOn();
      noiseEnv.noteOn();
      sine.phase(0);
    }
    else
    {
      wav.play(fileName.c_str());
      //        delay(5);
    }
  }

  void setupParams()
  {

    sine.frequency(values[2].value);
    pitchEnv.setCoefficients(values[3].value * 0.9, 0.001, values[0].value);
    ampEnv.setCoefficients(values[3].value, 0.001, values[0].value, values[4].value);
    noiseEnv.setCoefficients(values[3].value * 0.9, 0.0001, values[0].value);
    filter.setLowpass(0, values[5].value, 0.6);
    noise.amplitude(values[1].value);

    fileName = "KICK";
    fileName += (int)floor(values[6].value);
    fileName += ".WAV";
  }

  void setIndex(byte _idx)
  {
    index = _idx;
  }

  float *filterCutoff;
  float *bodyLength;
  float *bodyFrequency;
  float *noiseAmount;
  float *attackTime;

private:
  String filePath = "KICK";
  byte index = 0;
  AudioSynthWaveformSineModulated sine;
  Envelope pitchEnv;
  Envelope ampEnv;
  Envelope noiseEnv;
  AudioFilterBiquad filter;
  AudioSynthNoisePink noise;
  AudioEffectMultiply sineAM;
  AudioEffectMultiply noiseAM;
  AudioMixer4 noiseMixer;
  AudioPlaySdRaw wav;

  AudioConnection *patch0;
  AudioConnection *patch1;
  AudioConnection *patch2;
  AudioConnection *patch3;
  AudioConnection *patch4;
  AudioConnection *patch5;
  AudioConnection *patch6;
  AudioConnection *patch7;
  AudioConnection *patch8;
  AudioConnection *patch9;
  AudioConnection *patch10;
};

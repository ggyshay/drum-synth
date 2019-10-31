#include <Audio.h>
#include "Envelope.h"
#include <string>
#include "samples.h"

class Clap : public InstrumentI
{
public:
  Clap()
  {
    Value attackValue(2.0, 200.0, 2.0, "ATAQUE");
    values.push_back(attackValue);

    Value decayValue(2.0, 1000.0, 300.0, "DURACAO");
    values.push_back(decayValue);

    Value bandpassValue(100.0, 7000.0, 1140.0, "BAND PASS");
    values.push_back(bandpassValue);

    Value trainIntervalValue(2.0, 100.0, 11.0, "INTERVALO");
    values.push_back(trainIntervalValue);

    Value indexValue(0, 2, 0, "INDICE", 3);
    values.push_back(indexValue);

    setupParams();

    patch1 = new AudioConnection(noise, BPF);

    patch2 = new AudioConnection(BPF, 0, AM1, 0);
    patch3 = new AudioConnection(env1, 0, AM1, 1);
    patch4 = new AudioConnection(AM1, 0, outMixer, 0);

    patch5 = new AudioConnection(BPF, 0, AM2, 0);
    patch6 = new AudioConnection(env2, 0, AM2, 1);
    patch7 = new AudioConnection(AM2, 0, outMixer, 1);

    patch8 = new AudioConnection(BPF, 0, AM3, 0);
    patch9 = new AudioConnection(env3, 0, AM3, 1);
    patch10 = new AudioConnection(AM3, 0, outMixer, 2);

    patch11 = new AudioConnection(wav, 0, outMixer, 3);
  }
  void noteOn(byte velocity)
  {
    s_out.gain(velocity * velocity / 16129.0f);
    if (floor(values[4].value) == 0)
    {
      env1.noteOn();
      env2.noteOn();
      env3.noteOn();
    }
    else
    {
      wav.play(AudioSampleClap1);
    }
  }

  void setIndex(byte idx)
  {
    index = idx;
  }

  void setupParams()
  {
    noise.amplitude(1);

    BPF.setBandpass(0, values[2].value, 1.95);
    env1.setCoefficients(60.0, 0.001, values[0].value, 0.0);
    env2.setCoefficients(60.0, 0.001, values[0].value, values[3].value);
    env3.setCoefficients(values[1].value, 0.001, values[0].value, values[3].value * 2);

    fileName = "CLAP";
    fileName += (int)floor(values[4].value);
    fileName += ".WAV";
  }

  float bandpass = 1140;
  float decayTime = 300.0f;
  float trainInterval = 11.f;
  float attackTime = 2.0f;

private:
  EnvelopeWithDelay env1;
  EnvelopeWithDelay env2;
  EnvelopeWithDelay env3;
  byte index = 0;
  AudioPlayMemory wav;
  AudioEffectMultiply AM1;
  AudioEffectMultiply AM2;
  AudioEffectMultiply AM3;
  AudioFilterBiquad BPF;
  AudioSynthNoiseWhite noise;
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
  AudioConnection *patch11;
};

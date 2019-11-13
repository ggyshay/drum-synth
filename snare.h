#include <Audio.h>
#include "Envelope.h"
#include "instrumentInterface.h"
#include <functional>

class Snare : public InstrumentI
{
public:
  Snare()
  {
    Value attackValue(2.0, 200.0, 2.0, "ATAQUE", 100, true);
    values.push_back(attackValue);

    Value decayValue(2.0, 1000.0, 200.0, "DURACAO", 100, true);
    values.push_back(decayValue);

    Value sizzleFrequencyValue(500, 7000.0, 1500.0f, "TOM", 100, true);
    values.push_back(sizzleFrequencyValue);

    Value indexValue(0, 2, 0, "INDICE", 2);
    values.push_back(indexValue);
    setupParams();

    patch1 = new AudioConnection(bodyPitchEnv, sine);
    patch2 = new AudioConnection(sine, 0, bodyAM, 0);
    patch3 = new AudioConnection(bodyAmpEnv, 0, bodyAM, 1);
    patch4 = new AudioConnection(bodyAM, 0, outMixer, 0);

    patch5 = new AudioConnection(snapNoise, 0, snapAM, 0);
    patch6 = new AudioConnection(snapEnv, 0, snapAM, 1);
    patch7 = new AudioConnection(snapAM, snapLPF);
    patch8 = new AudioConnection(snapLPF, 0, outMixer, 1);

    patch9 = new AudioConnection(sizzleNoise, 0, sizzleFilter, 0);
    patch10 = new AudioConnection(sizzleEnv, 0, sizzleFilter, 1);
    patch11 = new AudioConnection(sizzleEnv, 0, sizzleAM, 0);
    patch12 = new AudioConnection(sizzleFilter, 0, sizzleAM, 1);
    patch13 = new AudioConnection(sizzleAM, 0, outMixer, 2);
    patch14 = new AudioConnection(wav, 0, outMixer, 3);
  }

  void noteOn(byte velocity)
  {
    s_out.gain(velocity * velocity / 16129.0f);
    if (floor(values[3].value) == 0)
    {
      sine.phase(0);
      bodyPitchEnv.noteOn();
      bodyAmpEnv.noteOn();
      sizzleEnv.noteOn();
    }
    else
    {
      //        wav.play(fileName.c_str());
      // wav.play(AudioSampleSnare1);
    }
  }

  void setupParams()
  {
    sine.frequency(130);
    sine.amplitude(0.97);
    bodyPitchEnv.setCoefficients(values[1].value, 0.001, values[0].value);
    bodyAmpEnv.setCoefficients(values[1].value, 0.001, values[0].value);

    snapNoise.amplitude(0.97);
    snapEnv.setCoefficients(values[1].value, 0.001, values[0].value);
    snapLPF.setLowpass(0, 2500, 0.70);

    sizzleNoise.amplitude(0.97);
    sizzleFilter.frequency(values[2].value);
    sizzleFilter.octaveControl(2.415);
    sizzleFilter.resonance(0.3);
    sizzleEnv.setCoefficients(values[1].value, 0.001, values[0].value);

    // fileName = "SNARE";
    // fileName += (int)floor(values[3].value);
    // fileName += ".WAV";
  }

private:
  const char *filePath = "SNARE";
  byte index = 0;

  AudioSynthWaveformSineModulated sine;
  Envelope bodyPitchEnv;
  Envelope bodyAmpEnv;
  AudioEffectMultiply bodyAM;
  AudioSynthNoiseWhite snapNoise;
  Envelope snapEnv;
  AudioEffectMultiply snapAM;
  AudioFilterBiquad snapLPF;
  AudioSynthNoiseWhite sizzleNoise;
  Envelope sizzleEnv;
  AudioFilterStateVariable sizzleFilter;
  AudioEffectMultiply sizzleAM;
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
  AudioConnection *patch10;
  AudioConnection *patch11;
  AudioConnection *patch12;
  AudioConnection *patch13;
  AudioConnection *patch14;
};

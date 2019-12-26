#include "arduino.h"
#include <Audio.h>
#include "Envelope.h"

class FM : public InstrumentI
{
public:
  FM()
  {
    Value frequency1Value(30.0, 3600.0, 100.0, "TOM 1", 100, true);
    values.push_back(frequency1Value);

    Value frequency2Value(30.0, 3600.0, 800.0, "TOM 2", 100, true);
    values.push_back(frequency2Value);

    Value decay1Value(30.0, 1000.0, 50.0, "DURACAO 1", 100, true);
    values.push_back(decay1Value);

    Value decay2Value(30.0, 1000.0, 50.0, "DURACAO 2", 100, true);
    values.push_back(decay2Value);

    Value feedbackValue(0, 20.0, 0.0, "FEEDBACK");
    values.push_back(feedbackValue);

    Value modulationValue(0, 20.0, 0.5, "MODULACAO");
    values.push_back(modulationValue);

    Value indexValue(0, 8, 0, "INDICE", 8
    values.push_back(indexValue);

    setupParams();
    outMixer.gain(0, 4.57088); // gain correction
    patch1 = new AudioConnection(osc1, 0, AM1, 0);
    patch2 = new AudioConnection(env1, 0, AM1, 1);
    patch3 = new AudioConnection(AM1, 0, fmMixer, 0);
    patch4 = new AudioConnection(fmMixer, osc2);
    patch5 = new AudioConnection(osc2, 0, AM2, 0);
    patch6 = new AudioConnection(env2, 0, AM2, 1);
    patch7 = new AudioConnection(AM2, 0, fmMixer, 1);
    patch8 = new AudioConnection(AM2, 0, outMixer, 0);
    patch9 = new AudioConnection(sampler, 0, outMixer, 1);
  }

  void noteOn(byte velocity)
  {
    s_out.gain(velocity * velocity / 16129.0f);
    if (floor(values[6].value) == 0)
    {
      setupParams();
      env1.noteOn();
      env2.noteOn();
      osc1.phase(0);
      osc2.phase(0);
    }
    else
    {
      sampler.play(fileName);
    }
  }

  void setupParams()
  {
    osc1.frequency(values[0].value);
    osc2.frequency(values[1].value);
    env1.setCoefficients(values[2].value, 0.001, 2);
    env2.setCoefficients(values[3].value, 0.001, 2);
    fmMixer.gain(1, values[4].value);
    fmMixer.gain(0, values[5].value);

    snprintf(fileName, 8, "FM%d.RAW", (int)values[6].value);
  }

  float frequency1 = 100;
  float frequency2 = 880;
  float decay1 = 50;
  float decay2 = 150;
  float feedbackAmount = 0.0;
  float modAmount = 0.0;

private:
  AudioSynthWaveformSine osc1;
  AudioSynthWaveformSineModulated osc2;
  AudioMixer4 fmMixer;
  Envelope env1;
  Envelope env2;
  AudioEffectMultiply AM1;
  AudioEffectMultiply AM2;
  AudioPlaySerialflashRaw sampler;
  byte index = 0;
  AudioConnection *patch1;
  AudioConnection *patch2;
  AudioConnection *patch3;
  AudioConnection *patch4;
  AudioConnection *patch5;
  AudioConnection *patch6;
  AudioConnection *patch7;
  AudioConnection *patch8;
  AudioConnection *patch9;
};

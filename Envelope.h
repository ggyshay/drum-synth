#ifndef ENVELOPE_H
#define ENVELOPE_H
#include <math.h>
#include <stdlib.h>
#define Q 26

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"

inline int32_t floatToFixed(double input)
{
  return (int32_t)(input * (1 << Q));
}

inline int32_t mult32_32_add32(int32_t a, int32_t b, int32_t c)
{
  int32_t t = ((int64_t)a * (int64_t)b) / (1 << Q);
  return t + c;
}

class Envelope : public AudioStream

{
public:
  Envelope() : AudioStream(0, NULL) {}
  void setCoefficients(double _length, double yf, double _attack, double midPoint = 0.3)
  {
    double m = (yf - midPoint) / (midPoint - 1);
    double alpha = 2000.0 / _length * log(m);
    double c = 1 - (midPoint - 1) / (m - 1);
    double h = 1.0 / AUDIO_SAMPLE_RATE;

    multTerm = floatToFixed((1.0 + h * alpha));
    constTerm = floatToFixed((-h * alpha * c));

    state = true;
    lastY = 0;

    attackSlope = (uint32_t)floatToFixed(1000.0 / (_attack * AUDIO_SAMPLE_RATE));
  }

  int32_t nextY()
  {
    if (state)
    {
      if (lastY < 5000)
        return lastY = 0;
      return lastY = mult32_32_add32(lastY, multTerm, constTerm);
    }
    lastY += attackSlope;
    state = (lastY >= 1 << Q);
    return lastY;
  }

  void update(void)
  {
    audio_block_t *block = allocate();
    if (block)
    {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i)
      {
        block->data[i] = (int16_t)(nextY() / (1 << (Q - 15)));
      }
      transmit(block);
      release(block);
    }
  }

  void noteOn()
  {
    lastY = 0;
    state = false;
  }

private:
  bool shouldDebug = false;
  int32_t lastY;
  uint32_t attackSlope;
  int32_t multTerm;
  int32_t constTerm;
  bool state; // 0 = attack, 1 = decay
};

class EnvelopeWithDelay : public AudioStream
{
public:
  EnvelopeWithDelay() : AudioStream(0, NULL) {}
  void setCoefficients(double length, double yf, double attack, float _delayTime)
  {
    state = 2;
    double h = 1.0 / AUDIO_SAMPLE_RATE;
    lambda = floatToFixed(1 + 1000.0 * h / length * log(yf));
    lastY = 0;
    attackSlope = (uint32_t)floatToFixed(1000 / (attack * AUDIO_SAMPLE_RATE));
    //      samplePeriod = ((1.0 * 0x10000000 )/ AUDIO_SAMPLE_RATE );
    delayTime = _delayTime;
  }

  int32_t nextY()
  {
    switch (state)
    {
    case 0:
      if (millis() - lastNote >= delayTime)
      {
        state = 1;
      }
      return 0;
    case 1:
      lastY += attackSlope;
      state = (lastY >= 1 << Q) ? 2 : 1;
      return lastY;
      break;
    case 2:
      int64_t temp = (int64_t)lastY * (int64_t)lambda;
      return lastY = temp >> Q;
      break;
    }
    return 0;
  }

  void update(void)
  {
    audio_block_t *block = allocate();
    if (block)
    {
      for (int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i)
      {
        block->data[i] = (int16_t)(nextY() >> (Q - 15));
      }
      transmit(block);
      release(block);
    }
  }

  void noteOn()
  {
    lastY = 0;
    state = 0;
    lastNote = millis();
  }

private:
  int32_t lambda;
  int32_t lastY;
  uint32_t attackSlope;
  float delayTime = 0;
  byte state; // 0 = delay, 1 = attack, 2 = decay
  long lastNote = 0;
};
#endif

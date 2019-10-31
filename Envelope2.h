//
//
//#ifndef ENVELOPE2_H
//#define ENVELOPE2_H
//#include <math.h>
//#include <stdlib.h>
//#define Q 28
//
//#include "Arduino.h"
//#include "AudioStream.h"
//#include "arm_math.h"
//
//extern "C"
//{
//    int mult64(int a, int b);
//}
//
//class Envelope : public AudioStream
//
//{
//public:
//    Envelope() : AudioStream(0, NULL) {}
//    void setCoefficients(double length, double yf, double attack, double shap = 0.3)
//    {
//        state = true;
//        double h = 1.0 / AUDIO_SAMPLE_RATE;
//        lambda = (1 + 1000.0 * h / length * log(yf)) * 0x10000000;
//        lastY = 0;
//        attackSlope = (uint32_t)(1000 / (attack * AUDIO_SAMPLE_RATE) * 0x10000000);
//    }
//
//    int32_t nextY()
//    {
//        if (state)
//        {
//            int64_t temp = (int64_t)lastY * (int64_t)lambda;
//            return lastY = temp >> Q;
//        }
//        lastY += attackSlope;
//        state = (lastY >= 0x8000000);
//        return lastY;
//    }
//
//    void update(void)
//    {
//        audio_block_t *block = allocate();
//        if (block)
//        {
//            for (int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i)
//            {
//                block->data[i] = (int16_t)(nextY() >> 13);
//            }
//            transmit(block);
//            release(block);
//        }
//    }
//
//    void noteOn()
//    {
//        lastY = attackSlope == 0 ? 0x8000000 : 0;
//        state = attackSlope == 0;
//    }
//
//private:
//    int32_t lambda;
//    int32_t lastY;
//    uint32_t attackSlope;
//    bool state; // 0 = attack, 1 = decay
//};
//
//class EnvelopeWithDelay : public AudioStream
//{
//public:
//    EnvelopeWithDelay() : AudioStream(0, NULL) {}
//    void setCoefficients(double length, double yf, double attack, float _delayTime)
//    {
//        state = 2;
//        double h = 1.0 / AUDIO_SAMPLE_RATE;
//        lambda = (1 + 1000.0 * h / length * log(yf)) * 0x10000000;
//        lastY = 0;
//        attackSlope = (uint32_t)(1000 / (attack * AUDIO_SAMPLE_RATE) * 0x10000000);
//        //      samplePeriod = ((1.0 * 0x10000000 )/ AUDIO_SAMPLE_RATE );
//        delayTime = _delayTime;
//    }
//
//    int32_t nextY()
//    {
//        switch (state)
//        {
//        case 0:
//            if (millis() - lastNote >= delayTime)
//            {
//                state = 1;
//            }
//            return 0;
//        case 1:
//            lastY += attackSlope;
//            state = (lastY >= 0x8000000) ? 2 : 1;
//            return lastY;
//            break;
//        case 2:
//            int64_t temp = (int64_t)lastY * (int64_t)lambda;
//            return lastY = temp >> Q;
//            break;
//        }
//        return 0;
//    }
//
//    void update(void)
//    {
//        audio_block_t *block = allocate();
//        if (block)
//        {
//            for (int i = 0; i < AUDIO_BLOCK_SAMPLES; ++i)
//            {
//                block->data[i] = (int16_t)(nextY() >> 13);
//            }
//            transmit(block);
//            release(block);
//        }
//    }
//
//    void noteOn()
//    {
//        lastY = 0;
//        state = 0;
//        lastNote = millis();
//    }
//
//private:
//    int32_t lambda;
//    int32_t lastY;
//    uint32_t attackSlope;
//    float delayTime = 0;
//    //    uint32_t samplePeriod; // this could be somewhere else
//    byte state; // 0 = delay, 1 = attack, 2 = decay
//    long lastNote = 0;
//};
//#endif

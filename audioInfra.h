#include <Audio.h>
#include <i2c_t3.h>
#include <SD_t3.h>
#include <functional>
#include <EEPROM.h>

#ifndef _AUDIO_INFRA_H_
#define _AUDIO_INFRA_H_
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

void print_i2c_status(void)
{
  switch (Wire.status())
  {
  case I2C_WAITING:
    Serial.print("I2C waiting, no errors\n");
    break;
  case I2C_ADDR_NAK:
    Serial.print("Slave addr not acknowledged\n");
    break;
  case I2C_DATA_NAK:
    Serial.print("Slave data not acknowledged\n");
    break;
  case I2C_ARB_LOST:
    Serial.print("Bus Error: Arbitration Lost\n");
    break;
  case I2C_TIMEOUT:
    Serial.print("I2C timeout\n");
    break;
  case I2C_BUF_OVF:
    Serial.print("I2C buffer overflow\n");
    break;
  default:
    Serial.print("I2C busy\n");
    break;
  }
}

class AudioInfra
{
public:
  void setup()
  {
    AudioMemory(50);
    //      sgtl5000_1.enable();
    //      sgtl5000_1.volume(1);

    audioInfraPatch1 = new AudioConnection(_input, 0, audioOutput, 0);
    audioInfraPatch2 = new AudioConnection(_input, 0, audioOutput, 1);

    //      SPI.setMOSI(SDCARD_MOSI_PIN);
    //      SPI.setSCK(SDCARD_SCK_PIN);
    //      while (!(SD.begin(SDCARD_CS_PIN))) {
    //        Serial.println("Unable to access the SD card");
    //        delay(500);
    //      }
    Serial.println("connected to sd card");
  }

  void initiateClock()
  {
    usingInternalClock = true;
    if (onTick != nullptr)
    {
      int sixteenth = (int)(1000.0 * 15000.0 / 140.0);
      myTimer.begin(onTick, sixteenth);
    }
  }

  void deactivateClock()
  {
    myTimer.end();
    usingInternalClock = false;
  }

  void setClock(int bpm)
  {
    int sixteenth = (int)(1000.0 * 15000.0 / bpm);
    myTimer.update(sixteenth);
  }

  //    AudioMixer4       output1;
  //    AudioMixer4       output2;
  AudioAmplifier _input;
  bool usingInternalClock = false;
  void (*onTick)(void) = nullptr;
  void (*saveAudioState)(void) = nullptr;
  void (*recoverAudioState)(void) = nullptr;

private:
  AudioOutputI2S audioOutput;
  AudioConnection *audioInfraPatch1;
  AudioConnection *audioInfraPatch2;
  AudioConnection *audioInfraPatch3;
  AudioConnection *audioInfraPatch4;
  AudioMixer4 internalMixer;
  AudioControlSGTL5000 sgtl5000_1;
  IntervalTimer myTimer;
};

AudioInfra *audioInfra = new AudioInfra();

#endif
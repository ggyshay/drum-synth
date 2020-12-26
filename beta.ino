#include "audioInfra.h"
#include "snare.h"
#include "clap.h"
#include "hat.h"
#include "fm.h"
#include "kick.h"
#include "displayDriver.h"
#include "Encoder.h"
#include "Audio.h"
#include "i2c_t3.h"
#include "Menu.h"
#include "EEPROM.h"
#include "SerialTransfer.h"
namespace std
{
  void __throw_bad_alloc()
  {
    Serial.println("Unable to allocate memory");
  }

  void __throw_length_error(char const *e)
  {
    Serial.print("Length Error :");
    Serial.println(e);
  }

  void __throw_bad_function_call()
  {
    Serial.println("Bad function call!");
  }
} // namespace std

#define A_PORT 26
#define B_PORT 25
#define C_PORT 24
#define s0 15
#define s1 16
#define s2 17
#define FLT_PORT A12

DisplayDriver disp;
Menu menu(&disp);
Encoder encoders[8];
InstrumentI *instruments[8];
SerialTransfer serialTransfer(&disp);
byte clockCounter = 0;
byte velocities[8] = {0, 0, 0, 0, 0, 0, 0, 0};

AudioConnection *patchCord1;
AudioConnection *patchCord2;
AudioConnection *patchCord3;
AudioConnection *patchCord4;
AudioConnection *patchCord5;
AudioConnection *patchCord6;
AudioConnection *patchCord7;
AudioConnection *patchCord8;
AudioConnection *patchCord9;
AudioConnection *patchCord10;
AudioConnection *patchCord11;

void setup()
{
  Serial.begin(4608000U);
  Serial.println("serial is up");
  audioInfra->setup();
  audioInfra->saveAudioState = &saveState;
  audioInfra->recoverAudioState = &recoverState;
  audioInfra->onTick = &sendMidiClock;

  Serial.println("audio infra setup done");
  instruments[0] = new Kick();
  instruments[1] = new Hat(100.0);
  instruments[2] = new Hat(500.0);
  instruments[3] = new Snare();
  instruments[4] = new Clap();
  instruments[5] = new FM();
  instruments[6] = new FM();
  instruments[7] = new FM();

  audioInfra->output1.gain(0, 0.74498524052);      // Kick gain correction
  audioInfra->output1.gain(1, 1.53108746168);      // CH gain correction
  audioInfra->output1.gain(2, 1.53108746168);      // OH gain correction
  audioInfra->output1.gain(3, 0.73875426299);      // Snare gain correction
  audioInfra->output2.gain(0, 2.40713251905);      // Clap gain correction
  audioInfra->output2.gain(1, 0.8881779763192479); // FM gain correction
  audioInfra->output2.gain(2, 0.8881779763192479); // FM gain correction
  audioInfra->output2.gain(3, 0.8881779763192479); // FM gain correction

  patchCord1 = new AudioConnection(instruments[0]->s_out, 0, audioInfra->output1, 0);
  patchCord2 = new AudioConnection(instruments[1]->s_out, 0, audioInfra->output1, 1);
  patchCord3 = new AudioConnection(instruments[2]->s_out, 0, audioInfra->output1, 2);
  patchCord4 = new AudioConnection(instruments[3]->s_out, 0, audioInfra->output1, 3);
  patchCord5 = new AudioConnection(instruments[4]->s_out, 0, audioInfra->output2, 0);
  patchCord6 = new AudioConnection(instruments[5]->s_out, 0, audioInfra->output2, 1);
  patchCord7 = new AudioConnection(instruments[6]->s_out, 0, audioInfra->output2, 2);
  patchCord8 = new AudioConnection(instruments[7]->s_out, 0, audioInfra->output2, 3);

  pinMode(A_PORT, INPUT);
  pinMode(B_PORT, INPUT);
  pinMode(C_PORT, INPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  Serial.println("objects allocated");

  for (int i = 0; i < 8; i++)
  {
    encoders[i].onClick = [i]() -> void {
      Value *v = instruments[i]->getNextValue();
      encoders[i].setParameterPointer(v);
      // Serial.print(v->toString());
      disp.putScreen(v->nameTag, v->value);
    };
    encoders[i].setParameterPointer(instruments[i]->getCurrentValue());
  }
  pinMode(3, OUTPUT); // set SDA + SCL pins as output
  pinMode(4, OUTPUT);
  disp.init();
  Serial.println("attempting to start Wire2");
  Wire2.begin(I2C_MASTER, 0x0, I2C_PINS_3_4, I2C_PULLUP_EXT, 1200000L);
  Wire2.onReqFromDone(handleSequencerResponse);

  usbMIDI.setHandleClock(handleClockIn);
  usbMIDI.setHandleStart(sendMidiStart);
  usbMIDI.setHandleStop(sendMidiStop);

  Serial.println("general setup finished");
  delay(100);
  disp.putScreen("ARA ARA ARA A", "RA ARA ARA AR");
}

void loop()
{
  serialTransfer.transfer();
  for (byte i = 0; i < 8; ++i)
  {
    if (velocities[i] > 0)
    {
      usbMIDI.sendNoteOn(36 + i, velocities[i], 0);
      instruments[i]->noteOn(velocities[i]);
      velocities[i] = 0;
    }
  }

  usbMIDI.read();

  for (byte i = 0; i < 8; ++i)
  {
    digitalWriteFast(s0, i & B001);
    digitalWriteFast(s1, i & B010);
    digitalWriteFast(s2, i & B100);
    delayMicroseconds(5);
    bool changed = encoders[i].setReading(digitalReadFast(A_PORT), digitalReadFast(B_PORT), digitalReadFast(C_PORT));
    if (changed)
    {
      Value *v = instruments[i]->getCurrentValue();
      disp.putScreen(v->nameTag, v->value);
      instruments[i]->setupParams();
    }
  }
  menu.update();
  audioInfra->setFilter(analogRead(FLT_PORT));
}

void handleSequencerResponse(void)
{
  Wire2.read(velocities, Wire2.available());
}

void sendMidiStart()
{
  if (!(audioInfra->usingInternalClock))
  {
    clockCounter = 0;
    Wire2.beginTransmission(0x66);
    Wire2.write(0xFA);
    Wire2.endTransmission();
  }
}

void sendMidiStop()
{
  if (!(audioInfra->usingInternalClock))
  {
    clockCounter = 0;
    Wire2.beginTransmission(0x66);
    Wire2.write(0xFC);
    Wire2.endTransmission();
  }
}

void sendMidiClock()
{
  Wire2.requestFrom(0x66, 8);
}

void handleClockIn()
{
  if (!(audioInfra->usingInternalClock))
  {
    if (clockCounter == 0)
    {
      sendMidiClock();
    }
    clockCounter = (clockCounter + 1) % 6;
  }
}

void saveState()
{
  byte offset = 0;
  for (byte i = 0; i < 8; i++)
  {
    byte size = instruments[i]->values.size();
    EEPROM.put(offset, size);
    offset += sizeof(byte);
    for (byte j = 0; j < size; ++j)
    {
      EEPROM.put(offset, instruments[i]->values[j].value);
      offset += sizeof(float);
    }
  }
}

void recoverState()
{
  byte offset = 0;
  byte size = 0;
  for (byte i = 0; i < 8; ++i)
  {
    EEPROM.get(offset, size);
    offset += sizeof(byte);
    for (byte j = 0; j < size; ++j)
    {
      EEPROM.get(offset, instruments[i]->values[j].value);
      offset += sizeof(float);
    }
  }
  for (byte i = 0; i < 8; i++)
  {
    instruments[i]->setupParams();
  }
}

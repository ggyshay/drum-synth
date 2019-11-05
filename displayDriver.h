#include "Arduino.h"
#include <avr/pgmspace.h>
#include <i2c_t3.h>

class DisplayDriver
{
public:
  void cls(char); //clear the display (optionally with specific byte value)
  void init();
  void putScreen(const char *, const char *);
  void putScreen(const char *, float);

private:
  static unsigned int lastTransmit;
  static void blackLine();
  static void writeLine(char *);
  void sendCmd(unsigned char cmd); //send a single command (see datasheet)
  static char nextStrings[2][14];
  static void triggerTransmission();
  static void transmitScreen();
};

#include <Audio.h>
#include <i2c_t3.h>
#include <SD_t3.h>
#include <functional>
#include <EEPROM.h>
#include <SerialFlash.h>

#ifndef _AUDIO_INFRA_H_
#define _AUDIO_INFRA_H_
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14
#define SFLASH_PIN 6

void setupWaveShaper(float *curve, int buffer_length, float amount)
{
  float deg = PI / 180.0f;
  float x;
  for (int i = 0; i < buffer_length; ++i)
  {
    x = (float)i * 2.0f / buffer_length - 1.0f;
    curve[i] = (3.0 + amount) * x * 20.0 * deg / (PI + amount * abs(x));
  }
}

void print_cpu_memory_status(void)
{
  Serial.print("CPU: ");
  Serial.print(AudioProcessorUsage());
  Serial.print(",");
  Serial.print(AudioProcessorUsageMax());
  Serial.print("    ");
  Serial.print("Memory: ");
  Serial.print(AudioMemoryUsage());
  Serial.print(",");
  Serial.print(AudioMemoryUsageMax());
  Serial.println();
}

void print_i2c_status(void)
{
  switch (Wire1.status())
  {
  case I2C_WAITING:
    // Serial.print("I2C waiting, no errors\n");
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
  case I2C_NOT_ACQ:
    Serial.print("I2C: Not acq\n");
    break;
  case I2C_DMA_ERR:
    Serial.print("DMA error\b");
    break;
  case I2C_SENDING:
    Serial.println("i2c sending");
    break;
  case I2C_SEND_ADDR:
    Serial.println("i2c send addr");
    break;
  case I2C_RECEIVING:
    Serial.println("i2c recieving");
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
    AudioMemory(60);
    sgtl5000_1.enable();
    sgtl5000_1.volume(0.8);

    audioInfraPatch1 = new AudioConnection(output1, 0, internalMixer, 0);
    audioInfraPatch2 = new AudioConnection(output2, 0, internalMixer, 1);
    audioInfraPatch3 = new AudioConnection(internalMixer, distortion);
    audioInfraPatch4 = new AudioConnection(distortion, 0, filter, 0);
    audioInfraPatch5 = new AudioConnection(filter, 2, audioOutput, 0);
    audioInfraPatch6 = new AudioConnection(filter, 2, audioOutput, 1);
    audioInfraPatch7 = new AudioConnection(filter, 2, usbOutput, 0);
    audioInfraPatch8 = new AudioConnection(filter, 2, usbOutput, 1);
    filter.resonance(1);

    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);
    // while (!(SD.begin(SDCARD_CS_PIN))) {
    //   Serial.println("Unable to access the SD card");
    //   delay(500);
    // }
    Serial.println("connected to sd card");
    SerialFlash.begin(SFLASH_PIN);
    delay(100);
    Serial.println("serial flash begun");
    setDistortion(1.0);
  }

  void setDistortion(float amount)
  {
    setupWaveShaper(waveshaperArray, 257, amount);
    distortion.shape(waveshaperArray, 257);
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
  void setFilter(int frequency)
  {
    if (frequency - lastFLTPosition > 2 || frequency - lastFLTPosition < -2)
    {
      lastFLTPosition = frequency;
      filter.frequency(20.0 * exp(0.00674585476 * frequency));
    }
  }
  float waveshaperArray[257];
  AudioMixer4 output1;
  AudioMixer4 output2;
  AudioAmplifier _input;
  bool usingInternalClock = false;
  void (*onTick)(void) = nullptr;
  void (*saveAudioState)(void) = nullptr;
  void (*recoverAudioState)(void) = nullptr;

private:
  AudioOutputI2S audioOutput;
  AudioOutputUSB usbOutput;
  AudioConnection *audioInfraPatch1;
  AudioConnection *audioInfraPatch2;
  AudioConnection *audioInfraPatch3;
  AudioConnection *audioInfraPatch4;
  AudioConnection *audioInfraPatch5;
  AudioConnection *audioInfraPatch6;
  AudioConnection *audioInfraPatch7;
  AudioConnection *audioInfraPatch8;
  AudioMixer4 internalMixer;
  AudioControlSGTL5000 sgtl5000_1;
  IntervalTimer myTimer;
  AudioEffectWaveshaper distortion;
  AudioFilterStateVariable filter;
  int lastFLTPosition = 0;
};

AudioInfra *audioInfra = new AudioInfra();

#endif

// #include <Audio.h>
// #include <i2c_t3.h>
// #include <SD.h>
// #include <SPI.h>
// #include <SerialFlash.h>

// // GUItool: begin automatically generated code
// AudioPlaySerialflashRaw          playMem;     //xy=477,215
// AudioOutputI2S           i2s1;           //xy=668,219
// AudioConnection          patchCord1(playMem, 0, i2s1, 0);
// AudioConnection          patchCord2(playMem, 1, i2s1, 1);
// AudioControlSGTL5000     sgtl5000_1;     //xy=64.5,20
// // GUItool: end automatically generated code

// #define SDCARD_CS_PIN    10
// #define SDCARD_MOSI_PIN  7
// #define SDCARD_SCK_PIN   14

// void print_i2c_status(void)
// {
//   switch (Wire.status())
//   {
//     case I2C_WAITING:  Serial.print("I2C waiting, no errors\n"); break;
//     case I2C_ADDR_NAK: Serial.print("Slave addr not acknowledged\n"); break;
//     case I2C_DATA_NAK: Serial.print("Slave data not acknowledged\n"); break;
//     case I2C_ARB_LOST: Serial.print("Bus Error: Arbitration Lost\n"); break;
//     case I2C_TIMEOUT:  Serial.print("I2C timeout\n"); break;
//     case I2C_BUF_OVF:  Serial.print("I2C buffer overflow\n"); break;
//     default:           Serial.print("I2C busy\n"); break;
//   }
// }

// void getFileExtension (const char *fileName, char *extension) {
//   byte i = 0;
//   for (; fileName[i] != 0 && fileName[i] != '.'; ++i);
//   if (fileName[i] == 0) return;
//   byte extOffset = ++i;
//   for (; fileName[i] != 0; ++i) {
//     extension[i - extOffset] = fileName[i];
//   }
//   extension[i - extOffset] = fileName[i];
// }

// void setup() {
//   Serial.begin(112500);
//   while (!Serial);
//   AudioMemory(50);
//   sgtl5000_1.enable();
//   sgtl5000_1.volume(0.4);

//   SPI.setMOSI(SDCARD_MOSI_PIN);
//   SPI.setSCK(SDCARD_SCK_PIN);
//   while (!(SD.begin(SDCARD_CS_PIN))) {
//     Serial.println("Unable to access the SD card");
//     delay(500);
//   }
//   Serial.println("connected to sd card");
//   SerialFlash.begin();
//   copyRawFiles();
// }

// void loop() {
//   Serial.println("clap");
//   playMem.play("CLAP1.RAW");
//   delay(500);
// }

// void copyRawFiles () {
//   SerialFlash.eraseAll();
//   while (!SerialFlash.ready()) {
//     Serial.println("erasing...");
//     delay(200);
//   }
//   File rootdir = SD.open("/");
//   rootdir.rewindDirectory();
//   while (1) {
//     // open a file from the SD card
//     Serial.println();
//     File f = rootdir.openNextFile();
//     if (!f) break;
//     const char *filename = f.name();
//     char extension[7];
//     getFileExtension(filename, extension);
//     if (strcmp(extension, "RAW") == 0) {
//       Serial.print(filename);
//       Serial.print("    ");
//       unsigned long length = f.size();
//       Serial.println(length);

//       // create the file on the Flash chip and copy data
//       if (SerialFlash.create(filename, length)) {
//         SerialFlashFile ff = SerialFlash.open(filename);
//         if (ff) {
//           Serial.print("  copying");
//           // copy data loop
//           unsigned long count = 0;
//           unsigned char dotcount = 9;
//           while (count < length) {
//             char buf[256];
//             unsigned int n;
//             n = f.read(buf, 256);
//             ff.write(buf, n);
//             count = count + n;
// //            Serial.print(".");
// //            if (++dotcount > 100) {
// ////              Serial.println();
// //              dotcount = 0;
// //            }
//           }
//           ff.close();
// //          if (dotcount > 0) Serial.println();
//         } else {
//           Serial.println("  error opening freshly created file!");
//         }
//       } else {
//         Serial.println("  unable to create file");
//       }
//       f.close();
//     }
//   }
//   rootdir.close();
//   delay(10);
//   Serial.println("Finished All Files");
// }

// ===============================================================================================================================
//#include <SerialFlash.h>
//#include <SD_t3.h>
//#include <SPI.h>
//#define SDCARD_CS_PIN    10
//#define SDCARD_MOSI_PIN  7
//#define SDCARD_SCK_PIN   14
//
//const int SDchipSelect = 4;    // Audio Shield has SD card CS on pin 10
//const int FlashChipSelect = 6; // digital pin for flash chip CS pin
////const int FlashChipSelect = 21; // Arduino 101 built-in SPI Flash
//
//void setup() {
//  //uncomment these if using Teensy audio shield
//  //SPI.setSCK(14);  // Audio shield has SCK on pin 14
//  //SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
//
//  //uncomment these if you have other SPI chips connected
//  //to keep them disabled while using only SerialFlash
//  //pinMode(4, INPUT_PULLUP);
//  //pinMode(10, INPUT_PULLUP);
//
//  Serial.begin(9600);
//
//  // wait up to 10 seconds for Arduino Serial Monitor
//  unsigned long startMillis = millis();
//  while (!Serial && (millis() - startMillis < 10000)) ;
//  delay(100);
//  Serial.println("Copy all files from SD Card to SPI Flash");
//  SPI.setMOSI(SDCARD_MOSI_PIN);
//  SPI.setSCK(SDCARD_SCK_PIN);
//  while (!(SD.begin(SDCARD_CS_PIN))) {
//    Serial.println("Unable to access the SD card");
//    delay(500);
//  }
//  Serial.println("connected to sd card");
//
//  if (!SerialFlash.begin(FlashChipSelect)) {
//    error("Unable to access SPI Flash chip");
//  }
//
//  File rootdir = SD.open("/");
//  while (1) {
//    // open a file from the SD card
//    Serial.println();
//    File f = rootdir.openNextFile();
//    if (!f) break;
//    const char *filename = f.name();
//    Serial.print(filename);
//    Serial.print("    ");
//    unsigned long length = f.size();
//    Serial.println(length);
//
//    // check if this file is already on the Flash chip
//    if (SerialFlash.exists(filename)) {
//      Serial.println("  already exists on the Flash chip");
//      SerialFlashFile ff = SerialFlash.open(filename);
//      if (ff && ff.size() == f.size()) {
//        Serial.println("  size is the same, comparing data...");
//        if (compareFiles(f, ff) == true) {
//          Serial.println("  files are identical :)");
//          f.close();
//          ff.close();
//          continue;  // advance to next file
//        } else {
//          Serial.println("  files are different");
//        }
//      } else {
//        Serial.print("  size is different, ");
//        Serial.print(ff.size());
//        Serial.println(" bytes");
//      }
//      // delete the copy on the Flash chip, if different
//      Serial.println("  delete file from Flash chip");
//      SerialFlash.remove(filename);
//    }
//
//    // create the file on the Flash chip and copy data
//    if (SerialFlash.create(filename, length)) {
//      SerialFlashFile ff = SerialFlash.open(filename);
//      if (ff) {
//        Serial.print("  copying");
//        // copy data loop
//        unsigned long count = 0;
//        unsigned char dotcount = 9;
//        while (count < length) {
//          char buf[256];
//          unsigned int n;
//          n = f.read(buf, 256);
//          ff.write(buf, n);
//          count = count + n;
//          Serial.print(".");
//          if (++dotcount > 100) {
//             Serial.println();
//             dotcount = 0;
//          }
//        }
//        ff.close();
//        if (dotcount > 0) Serial.println();
//      } else {
//        Serial.println("  error opening freshly created file!");
//      }
//    } else {
//      Serial.println("  unable to create file");
//    }
//    f.close();
//  }
//  rootdir.close();
//  delay(10);
//  Serial.println("Finished All Files");
//}
//
//
//bool compareFiles(File &file, SerialFlashFile &ffile) {
//  file.seek(0);
//  ffile.seek(0);
//  unsigned long count = file.size();
//  while (count > 0) {
//    char buf1[128], buf2[128];
//    unsigned long n = count;
//    if (n > 128) n = 128;
//    file.read(buf1, n);
//    ffile.read(buf2, n);
//    if (memcmp(buf1, buf2, n) != 0) return false; // differ
//    count = count - n;
//  }
//  return true;  // all data identical
//}
//
//
//void loop() {
//}
//
//void error(const char *message) {
//  while (1) {
//    Serial.println(message);
//    delay(2500);
//  }
//}

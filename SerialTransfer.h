#include "SerialFlash.h"

class SerialTransfer
{
public:
    void transfer()
    {
        if (!transfering)
            return;
        if (Serial.available() >= 32)
        {
            for (byte i = 0; i < 32; ++i)
            {
                buffer[i] = Serial.read();
            }
            handleBuffer();
        }
    }

private:
    int nameSize = 0;
    int dataSize = 0;
    bool transfering = true;
    char buffer[32];
    char nameBuf[20];
    int transfered = 0;
    SerialFlashFile file;

    void handleBuffer()
    {
        if (transfered == 0)
        {
            extractMetadata();
        }
        else
        {
            while (!SerialFlash.ready())
            {
            }
            Serial.write(SerialFlash.ready() ? 0xF8 : 0xF9);
            file.write(buffer, 32, 0);
        }

        Serial.write(0x55);
        transfered++;
        if (((transfered - 1) << 5) >= dataSize)
        {
            transfering = false;
            Serial.write(0xFF);
            endTransfer();
        }
    }

    void extractMetadata()
    {
        nameSize = buffer[0];
        dataSize = (buffer[nameSize + 1] << 16) + (buffer[nameSize + 2] << 8) + buffer[nameSize + 3];

        memcpy(nameBuf, buffer + 1, nameSize);
        nameBuf[nameSize] = 0;
        Serial.write(nameSize);
        Serial.print(dataSize);

        file = SerialFlash.open(nameBuf);
        if (file)
        {
            file.erase();
            while (!SerialFlash.ready())
                ;
        }
        SerialFlash.createErasable(nameBuf, dataSize);
        while (!SerialFlash.ready())
            ;
        file = SerialFlash.open(nameBuf);
        while (!SerialFlash.ready())
            ;
    }

    void endTransfer()
    {
        nameSize = 0;
        dataSize = 0;
        transfering = true;
        int transfered = 0;
    }
};
SerialTransfer serialTransfer;
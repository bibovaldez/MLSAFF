#ifndef RFID_READER_H
#define RFID_READER_H

#include <MFRC522.h>

class RFIDReader
{
public:
    RFIDReader(uint8_t rstPin, uint8_t ssPin) : rfid(ssPin, rstPin) {}

    void initRFID()
    {
        rfid.PCD_Init();
    }

    void checkForCard(std::function<void(const String &)> onTagRead)
    {
        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
        {
            String tag = "";
            for (byte i = 0; i < 4; i++)
            {
                tag += String(rfid.uid.uidByte[i], HEX);
            }
            onTagRead(tag);
            rfid.PICC_HaltA();
            rfid.PCD_StopCrypto1();
        }
    }

private:
    MFRC522 rfid;
};

#endif

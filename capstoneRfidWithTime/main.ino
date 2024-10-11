#include <RtcDS1302.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Crypto.h>
#include <ChaChaPoly.h>
#include <SPI.h>
#include <MFRC522.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
// separate files for each class tapnu nalinis jay code mo

#include "WiFiHandler.h" //ag connectToWiFi kin and sendEncryptedData
#include "TimeManager.h" //  beginNTPClient, updateRTC, printCurrentTime, isWithinOperatingHours, enterDeepSleepUntilNextOperation
#include "RFIDReader.h" // initRFID, checkForCard
#include "CryptoManager.h" //encryptMessage
#include "Certificate.h" //root_ca

WiFiHandler wifiHandler;
TimeManager timeManager;
RFIDReader rfidReader(D3, D4);
CryptoManager cryptoManager;

// Key and nonce from your database
uint8_t key[32] = {0xd2, 0x8a, 0x6f, 0x54, 0xb7, 0xc3, 0xe1, 0x09, 0x9a, 0x3f, 0x72, 0x81, 0x4d, 0xae, 0xb8, 0x63, 0x95, 0xe7, 0x1c, 0x2d, 0x41, 0xbc, 0x18, 0xfa, 0x07, 0xcd, 0x50, 0x7b, 0x32, 0x9f, 0x68, 0xf5};
uint8_t nonce[12] = {0x7f, 0x26, 0x8d, 0x91, 0x34, 0xeb, 0xc2, 0x57, 0xa3, 0x4b, 0x1e, 0x69};

void setup()
{
    Serial.begin(115200);

    // Initialize WiFi and NTP
    wifiHandler.connectToWiFi("mlsaff", "meljay04");
    timeManager.beginNTPClient(28800); // UTC +8 for Manila

    // Initialize SPI and RFID reader
    SPI.begin();
    rfidReader.initRFID();

    // Synchronize time
    timeManager.updateRTC();
}

void loop()
{
    timeManager.printCurrentTime();

    if (timeManager.isWithinOperatingHours())
    {
        rfidReader.checkForCard([&](const String &tag)
                                {
            String encryptedPayload = cryptoManager.encryptMessage(tag.c_str(), key, nonce);
            wifiHandler.sendEncryptedData("mlsaff.com", encryptedPayload); });
    }
    else
    {
        timeManager.enterDeepSleepUntilNextOperation();
    }

    delay(500); // Small delay to prevent spamming the server
}

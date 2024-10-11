#ifndef SERVER_COMMUNICATION_H
#define SERVER_COMMUNICATION_H

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include "WiFiManager.h"
#include "FeedingSystem.h"

class ServerCommunication {
public:
    ServerCommunication(WiFiManager& wifiManager);
    void begin();
    void sendWeightUpdate(int weight);
    void getAndProcessServerData(FeedingSystem& feedingSystem);
    void sendLog(const String& logMessage);

private:
    WiFiManager& _wifiManager;
    String requestDataFromServer(const String& endpoint);
    void sendEncryptedData(const String& encryptedPayload, const String& endpoint);
    void parseResponse(const String& response, FeedingSystem& feedingSystem);
};

#endif // SERVER_COMMUNICATION_H
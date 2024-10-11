#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "Certificate.h"

class WiFiHandler
{
public:
    void connectToWiFi(const char *ssid, const char *password)
    {
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Connected to WiFi");
    }

    void sendEncryptedData(const char *host, const String &encryptedPayload)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            WiFiClientSecure client;
            X509List cert(root_ca);
            client.setTrustAnchors(&cert);

            const int httpsPort = 443;
            if (!client.connect(host, httpsPort))
            {
                Serial.println("Connection failed!");
                return;
            }

            // Make an HTTPS POST request
            client.print(String("POST /api/verify-tag HTTP/1.1\r\n") +
                         "Host: " + host + "\r\n" +
                         "Content-Type: application/json\r\n" +
                         "Content-Length: " + encryptedPayload.length() + "\r\n" +
                         "Connection: close\r\n\r\n" +
                         encryptedPayload);

            // Read the response
            Serial.println("Response:");
            while (client.connected() || client.available())
            {
                if (client.available())
                {
                    String line = client.readStringUntil('\n');
                    Serial.println(line);
                }
            }

            client.stop();
        }
        else
        {
            Serial.println("Error: WiFi not connected");
        }
    }
};

#endif

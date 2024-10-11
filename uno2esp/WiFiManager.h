#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "Certificate.h"

class WiFiManager {
public:
    WiFiManager() : _ssid(nullptr), _password(nullptr), _lastReconnectAttempt(0) {}

    void begin(const char* ssid, const char* password) {
        _ssid = ssid;
        _password = password;
        connect();
    }

    void maintainConnection() {
        if (WiFi.status() != WL_CONNECTED) {
            unsigned long now = millis();
            if (now - _lastReconnectAttempt > 5000) { // Attempt to reconnect every 5 seconds
                _lastReconnectAttempt = now;
                connect();
            }
        }
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

    WiFiClientSecure& getSecureClient() {
        return _secureClient;
    }

    void sendEncryptedData(const char* host, const String& encryptedPayload) {
        if (isConnected()) {
            X509List cert(root_ca);
            _secureClient.setTrustAnchors(&cert);

            const int httpsPort = 443;
            if (!_secureClient.connect(host, httpsPort)) {
                Serial.println("Connection failed!");
                return;
            }

            // Make an HTTPS POST request
            _secureClient.print(String("POST /api/verify-tag HTTP/1.1\r\n") +
                                "Host: " + host + "\r\n" +
                                "Content-Type: application/json\r\n" +
                                "Content-Length: " + encryptedPayload.length() + "\r\n" +
                                "Connection: close\r\n\r\n" +
                                encryptedPayload);

            // Read the response
            Serial.println("Response:");
            while (_secureClient.connected() || _secureClient.available()) {
                if (_secureClient.available()) {
                    String line = _secureClient.readStringUntil('\n');
                    Serial.println(line);
                }
            }

            _secureClient.stop();
        } else {
            Serial.println("Error: WiFi not connected");
        }
    }

private:
    const char* _ssid;
    const char* _password;
    WiFiClientSecure _secureClient;
    unsigned long _lastReconnectAttempt;

    void connect() {
        WiFi.begin(_ssid, _password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Connected to WiFi");
    }
};

#endif // WIFI_MANAGER_H
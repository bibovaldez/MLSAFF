#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Crypto.h>
#include <ChaChaPoly.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <RTClib.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include <sys/time.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include "HX711.h"

// rx/tx
#define rx D3
#define tx D4
#define WAKEUP_PIN D0               // wake up pin
SoftwareSerial ArduinoUno(rx, tx);  // RX, TX
//time manila
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.windows.com", 28800, 60000); // UTC +8 for Asia/Manila

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D6;
const int LOADCELL_SCK_PIN = D7;
HX711 scale;

//RTC_DS3231 module 
RTC_DS3231 rtc;

DateTime now;

// const char* ssid = "Dan_VIP";        // Replace with your network SSID
// const char* password = "Ccsict-143";    // Replace with your network password

const char* ssid = "mlsaff";
const char* password = "meljay04";

// Rain sensor pin and timing
#define RAIN_SENSOR_PIN A0
unsigned long rainStartTime = 0;
bool raining = false;

//data holder
int weigth;
unsigned long previousMillis = 0;
const long interval = 10000;  // 5 seconds in milliseconds
const long interval2 = 2000;  // 2 seconds in milliseconds
String mh, mm, nh, nm, ms, ns, actstat, aw, mw, nw, lockstat, logs, jsonPayload;
String mhour, mhourtag, mminutes, mminutestag, nhour, nhourtag, nminutes, nminutestag, mstat, mstattag, nstat, nstattag, acts, actstag, aweight, aweighttag, mweight, mweighttag, nweight, nweighttag, locks, lockstattag;

const char* root_ca =
"-----BEGIN CERTIFICATE-----\n"
"MIIDoTCCA0igAwIBAgIRAO6Z9P1P3584De+0GaKWIIYwCgYIKoZIzj0EAwIwOzEL\n"
"MAkGA1UEBhMCVVMxHjAcBgNVBAoTFUdvb2dsZSBUcnVzdCBTZXJ2aWNlczEMMAoG\n"
"A1UEAxMDV0UxMB4XDTI0MDkxODEwMDAzM1oXDTI0MTIxNzEwMDAzMlowFTETMBEG\n"
"A1UEAxMKbWxzYWZmLmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABLDPiVtX\n"
"miRTng8Cbxad7xIliNcgSMHCX71hpwXXrKE2uiLMmTCVE3MKMv/OusB/6ogIejFq\n"
"RfpPGmc6MHnCB1+jggJRMIICTTAOBgNVHQ8BAf8EBAMCB4AwEwYDVR0lBAwwCgYI\n"
"KwYBBQUHAwEwDAYDVR0TAQH/BAIwADAdBgNVHQ4EFgQUSl1kxnbOegcFEpWm+c3c\n"
"33dccygwHwYDVR0jBBgwFoAUkHeSNWfE/6jMqeZ72YB5e8yT+TgwXgYIKwYBBQUH\n"
"AQEEUjBQMCcGCCsGAQUFBzABhhtodHRwOi8vby5wa2kuZ29vZy9zL3dlMS83cGsw\n"
"JQYIKwYBBQUHMAKGGWh0dHA6Ly9pLnBraS5nb29nL3dlMS5jcnQwIwYDVR0RBBww\n"
"GoIKbWxzYWZmLmNvbYIMKi5tbHNhZmYuY29tMBMGA1UdIAQMMAowCAYGZ4EMAQIB\n"
"MDYGA1UdHwQvMC0wK6ApoCeGJWh0dHA6Ly9jLnBraS5nb29nL3dlMS9yZ2Vva1JB\n"
"X24tZy5jcmwwggEEBgorBgEEAdZ5AgQCBIH1BIHyAPAAdgB2/4g/Crb7lVHCYcz1\n"
"h7o0tKTNuyncaEIKn+ZnTFo6dAAAAZIEyoTHAAAEAwBHMEUCIQDfWQ3wkU4kSMT3\n"
"PQLoyJYBdVLCRyxi8tobnsfoD0E/WgIgf2eN8JBKnmfs7CJQT0HvAgAS9MFZuRWB\n"
"7xyok4e/JRMAdgBIsONr2qZHNA/lagL6nTDrHFIBy1bdLIHZu7+rOdiEcwAAAZIE\n"
"yoS1AAAEAwBHMEUCIEYnDwNJHjyNza8eY4yAQ/dzVpCE64cehLdzzWunEcGRAiEA\n"
"7vKTK4YC9D8zqvyxH3g+JQbpsmy4ZA++bkQ2I3fkrW4wCgYIKoZIzj0EAwIDRwAw\n"
"RAIgfXAMt44ZN8Iw/hqKHLTZedbrnzUkNHVhl1Kq+z/LekoCIBhqkpluvQCSCuY+\n"
"BJCCTT76mMS7eDixnYsfPeUqNtqg\n"
  "-----END CERTIFICATE-----";



// Structure to hold sensor information
struct module {
  uint8_t key[32];
  uint8_t nonce[12];
};

// Sensor instances
module rainSensor = {
  { 0x64, 0xf0, 0x18, 0x17, 0x78, 0x5a, 0x8d, 0x8f, 0x81, 0xb5, 0xfd, 0x75, 0x8a, 0x26, 0xc5, 0xca, 0x39, 0xa4, 0x5e, 0x4a, 0x0e, 0xa7, 0x78, 0x06, 0x35, 0x87, 0x34, 0x9d, 0xb5, 0xe8, 0xcf },
  { 0xaa, 0x40, 0xc9, 0x71, 0x42, 0x15, 0x99, 0x21, 0x19, 0xe7, 0xe0, 0x76 }
};

module loadCellSensor = {
  { 0x66, 0xac, 0xdd, 0x89, 0xa8, 0x43, 0x4d, 0xf7, 0xc5, 0x2b, 0x05, 0x11, 0x9e, 0xbb, 0x0c, 0xd3, 0x4d, 0x36, 0x2c, 0xe6, 0x25, 0xac, 0xdb, 0xb4, 0xfb, 0x2a, 0xc8, 0xf6, 0xa0, 0xb9, 0xa3, 0x7b },
  { 0xf7, 0x52, 0x9d, 0x67, 0x55, 0x98, 0xb0, 0xab, 0x88, 0x5b, 0xd1, 0x1a }
};

module Realtimeclock = {
  { 0x53, 0x15, 0x74, 0xbf, 0x54, 0x65, 0x7d, 0xf6, 0xba, 0xa7, 0x4d, 0x2d, 0x60, 0xa6, 0xce, 0xf7, 0x5f, 0xfa, 0xbe, 0xa8, 0xd0, 0xd7, 0xa1, 0x58, 0xe6, 0x39, 0x11, 0xde, 0xab, 0xc1, 0x60, 0xf6 },
  { 0x25, 0x9a, 0xc0, 0xaf, 0x5c, 0x30, 0x23, 0x47, 0x8f, 0x64, 0xbe, 0x78 }
};

module stat = {
  { 0xb4, 0x68, 0x78, 0xd9, 0x43, 0x2d, 0xb4, 0x75, 0xcb, 0x6b, 0x15, 0xd2, 0xab, 0xb6, 0xc9, 0x11, 0xbf, 0x22, 0xde, 0x86, 0x22, 0x41, 0x09, 0x96, 0x65, 0xac, 0x08, 0x2b, 0x4c, 0xc0, 0x50, 0x04 },
  { 0x56, 0x8a, 0x5c, 0xd2, 0x81, 0x8c, 0xe9, 0x19, 0x58, 0xd1, 0x3f, 0x93 }
};
module actuator = {
  { 0xd2, 0x8a, 0x6f, 0x54, 0xb7, 0xc3, 0xe1, 0x09, 0x9a, 0x3f, 0x72, 0x81, 0x4d, 0xae, 0xb8, 0x63, 0x95, 0xe7, 0x1c, 0x2d, 0x41, 0xbc, 0x18, 0xfa, 0x07, 0xcd, 0x50, 0x7b, 0x32, 0x9f, 0x68, 0xf5 },
  { 0x7f, 0x26, 0x8d, 0x91, 0x34, 0xeb, 0xc2, 0x57, 0xa3, 0x4b, 0x1e, 0x69 }
};

// Server endpoint for sending data
String serverName = "/api/command-data";  // Replace with your actual endpoint
String cmd = "/api/cmd";
String activeFed = "/api/activeFed";
String AmFed = "/api/AmFed";
String PmFed = "/api/PmFed";
String updateAweight = "/api/upW";
String updateWeight = "/api/upWeight";
String sensor_reading = "/api/sensor";
String logsend = "/api/logs";

const char* host = "mlsaff.com";
const int httpsPort = 443;
const unsigned char Active_buzzer = D8;

void setup() {
  Serial.begin(9600);
  ArduinoUno.begin(9600);
  pinMode (Active_buzzer,OUTPUT) ;
  pinMode(RAIN_SENSOR_PIN, INPUT);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  pinMode(WAKEUP_PIN, OUTPUT);
  digitalWrite(WAKEUP_PIN, HIGH);
  // put your setup code here, to run once:
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());  // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));  // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
  // by the SCALE parameter (not set yet)

  scale.set_scale(95.916666666666666666666666666667); //calibration reading/20
  //scale.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());  // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));  // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight, divided
  // by the SCALE parameter set with set_scale

  Serial.println("Readings:");

  // Initialize the RTC module
  // Initialize the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // Set RTC to the compile time as fallback
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Set system time using RTC
  setSystemTimeFromRTC();
  // Check the time and deep sleep if outside of the 6 AM to 6 PM window
     DateTime now = rtc.now();
  if (now.hour() < 6 || now.hour() >= 20) {
    ArduinoUno.println("d");
    deepSleepUntil6AM(now);
  }


}

void loop() {
  // Wake up Arduino by sending a LOW pulse
  digitalWrite(WAKEUP_PIN, LOW);
  delay(100);  // Short delay to ensure the Arduino gets the signal
  digitalWrite(WAKEUP_PIN, HIGH);
  weigth = getWeight();
  Serial.println(String(weigth).c_str());
  if(weigth<100 || weigth>1000){
  tone(Active_buzzer,6000,255) ; //Turn on active buzzer

  }else
  {
    noTone(Active_buzzer);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Save the last time you performed the action
    String encryptedPayload = encryptMessage(String(weigth).c_str(), loadCellSensor);
    sendEncryptedData(encryptedPayload, updateWeight);
  }
  checkRainSensor();
  //wifi not connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Trying to reconnect...");
    WiFi.begin(ssid, password);
  }else{
  Serial.println("Connected to WiFi");
  }
  // Check the time and deep sleep if outside of the 6 AM to 6 PM window
  DateTime now = rtc.now();
  if (now.hour() < 6 || now.hour() >= 20) {
    ArduinoUno.println("d");

    deepSleepUntil6AM(now);
  }

  // Check for data from Arduino

  //feeding condition
  if (now.hour() == mh.toInt() && now.minute() == mm.toInt() == 0 && ms == "1") {
    weigth = getWeight();
    int fedWeight = weigth - mw.toInt();
    if (weigth > mw.toInt()) {
      logs = "Am fed validation match! Sending 'on' to Arduino.";
      Serial.println(logs);
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);
      //send on for feeding
      ArduinoUno.println("f");
      Serial.println('f');
      while (weigth >= fedWeight) {
        weigth = getWeight();
        Serial.println("Am fed in progreess");
      }
      ArduinoUno.println("s");  //stop
      //UPDATE DATA
      String encryptedPayload = encryptMessage(mw.c_str(), loadCellSensor);
      sendEncryptedData(encryptedPayload, sensor_reading);
      encryptedPayload = encryptMessage("0", stat);
      sendEncryptedData(encryptedPayload, AmFed);
      ms = "0";
      logs = "Stop feeding";
      Serial.println(logs);
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);
    } else {
      Serial.println("Not enough feeds fo feeding");
      logs = "Not enough feeds for AM feeding. failed to feed";
      Serial.println(logs);
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);
    }
  } else if (now.hour() == nh.toInt() && now.minute() == nm.toInt() && ns == "1") {
    weigth = getWeight();
    int fedWeight = weigth - nw.toInt();
    if (weigth > nw.toInt()) {
      logs = "Pm fed validation match! Sending 'on' to Arduino.";
      Serial.println(logs);
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);
      //send on for feeding
      ArduinoUno.println("f");
      Serial.println('f');
      while (weigth >= fedWeight) {
        weigth = getWeight();
        Serial.println("Pm fed in progreess");
      }
      ArduinoUno.println("s");  //stop
      //UPDATE DATA
      String encryptedPayload = encryptMessage(nw.c_str(), loadCellSensor);
      sendEncryptedData(encryptedPayload, sensor_reading);

      encryptedPayload = encryptMessage("0", stat);
      sendEncryptedData(encryptedPayload, PmFed);
      ns = "0";
      logs = "Stop feeding";
      Serial.println(logs);
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);
    } else {
      Serial.println("Not enough feeds fo feeding");
      logs = "Not enough feeds for PM feeding. failed to feed";
      Serial.println(logs);
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);
    }
  } else if (actstat == "1") {
    weigth = getWeight();
    int fedWeight = weigth - aw.toInt();
    if (weigth > aw.toInt()) {
      logs = "Active fed validation match! Sending 'on' to Arduino.";
      Serial.println(logs);
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);

      ArduinoUno.println("f");  //FEED
      Serial.println('f');
      while (weigth >= fedWeight) {
        weigth = getWeight();
        Serial.println("Active fed in progreess");
      }
      ArduinoUno.println("s");  //STOP
      Serial.println('s');
      //UPDATE
      String encryptedPayload = encryptMessage(aw.c_str(), loadCellSensor);
      sendEncryptedData(encryptedPayload, sensor_reading);

      encryptedPayload = encryptMessage("0", stat);
      sendEncryptedData(encryptedPayload, activeFed);
      encryptedPayload = encryptMessage("0", loadCellSensor);
      sendEncryptedData(encryptedPayload, updateAweight);
      actstat = "0";
      logs = "Stop feeding";
      Serial.println(logs);
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);
    } else {
      Serial.println("Not enough feeds fo feeding");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    getdata();

    if (lockstat == "1") {
      Serial.println("open storage! Sending 'on' to Arduino.");
      ArduinoUno.println("a");
      lockstat = "1";
      logs = "open storage! Sending 'on' to Arduino.";
      jsonPayload = "{\"log\": \"" + logs + "\"}";
      sendEncryptedData(jsonPayload, logsend);
      String encryptedPayload = encryptMessage("0", actuator);
      sendEncryptedData(encryptedPayload, cmd);
    } else {
      ArduinoUno.println("b");
    }
  }
}

void getdata() {
  // Request encrypted data from server
  String response = requestDataFromServer();
  if (response.length() > 0) {
    // Parse JSON response
    parseResponse(response);

    // Decrypt the data
    mh = decryptMessage(mhour, mhourtag, Realtimeclock);
    mm = decryptMessage(mminutes, mminutestag, Realtimeclock);
    nh = decryptMessage(nhour, nhourtag, Realtimeclock);
    nm = decryptMessage(nminutes, nminutestag, Realtimeclock);
    ms = decryptMessage(mstat, mstattag, stat);
    ns = decryptMessage(nstat, nstattag, stat);
    actstat = decryptMessage(acts, actstag, stat);
    aw = decryptMessage(aweight, aweighttag, loadCellSensor);
    mw = decryptMessage(mweight, mweighttag, loadCellSensor);
    nw = decryptMessage(nweight, nweighttag, loadCellSensor);
    lockstat = decryptMessage(locks, lockstattag, actuator);

    Serial.println(mh + "/" + mm + "/" + nh + "/" + nm + "/" + ms + "/" + ns + "/" + actstat + "/" + aw + "/" + mw + "/" + nw + "/" + lockstat);
    // Get current time from RTC
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  }
  delay(2000);
}
// Function to request encrypted data from the server
String requestDataFromServer() {
 WiFiClientSecure client;

  // Create X509List object and set the root CA
  X509List cert(root_ca);
  client.setTrustAnchors(&cert);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed!");
    return "";
  }

  // Make an HTTPS GET request
  client.print(String("GET " + serverName + " HTTP/1.1\r\n") + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  // Read the response and find the start of the JSON body
  String payload = "";
  bool jsonStarted = false;

  while (client.connected() || client.available()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');

      // If the line is just "\r", it means headers have ended
      if (line == "\r") {
        jsonStarted = true;
        continue;  // Skip the current line to start reading JSON data
      }

      // Once headers are skipped, append the JSON body
      if (jsonStarted) {
        payload += line;  // Append each line of JSON to the payload
      }
    }
  }

  client.stop();
  return payload; 
}

void parseResponse(const String& response) {
  // Find the position where the JSON data starts
  int jsonStartIndex = response.indexOf('{');
  if (jsonStartIndex == -1) {
    Serial.println("Error: JSON data not found.");
    return;
  }

  // Extract the JSON part
  String jsonResponse = response.substring(jsonStartIndex);

  // Create a DynamicJsonDocument with an appropriate size
  DynamicJsonDocument doc(2048);  // Adjust size as needed

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, jsonResponse);

  // Check for deserialization errors
  if (error) {
    Serial.print("DeserializeJson() failed: ");
    Serial.println(error.c_str());
    Serial.print("JSON Data: ");
    Serial.println(jsonResponse);  // Print the JSON for debugging
    return;
  }

  // Access the data using the keys
  mhour = doc["mh"].as<String>();
  mhourtag = doc["mhtag"].as<String>();
  mminutes = doc["mm"].as<String>();
  mminutestag = doc["mmtag"].as<String>();
  nhour = doc["nh"].as<String>();
  nhourtag = doc["nhtag"].as<String>();
  nminutes = doc["nm"].as<String>();
  nminutestag = doc["nmtag"].as<String>();
  mstat = doc["ms"].as<String>();
  mstattag = doc["mstag"].as<String>();
  nstat = doc["ns"].as<String>();
  nstattag = doc["nstag"].as<String>();
  acts = doc["as"].as<String>();
  actstag = doc["astag"].as<String>();
  aweight = doc["aw"].as<String>();
  aweighttag = doc["awtag"].as<String>();
  mweight = doc["mw"].as<String>();
  mweighttag = doc["mwtag"].as<String>();
  nweight = doc["nw"].as<String>();
  nweighttag = doc["nwtag"].as<String>();
  locks = doc["lock"].as<String>();
  lockstattag = doc["locktag"].as<String>();
}

int getWeight() {

  float weight = scale.get_units(10);        // Get the average of 10 weight readings
  int weightInt = static_cast<int>(weight);  // Convert to integer if needed

  // Print both for clarity
  Serial.print("Weight (float): ");
  Serial.println(weight);
  Serial.print("Weight (int): ");
  Serial.println(weightInt);

  return weightInt;
}

//decrypt Message
String decryptMessage(String ciphertextHex, String tagHex, module data) {
  ChaChaPoly chacha;
  uint8_t ciphertext[128];
  uint8_t tag[16];
  uint8_t decryptedText[128];

  // Convert hex strings to byte arrays
  hexStringToBytes(ciphertextHex, ciphertext);
  hexStringToBytes(tagHex, tag);

  // Initialize decryption with the sensor's key and nonce
  chacha.clear();
  chacha.setKey(data.key, sizeof(data.key));
  chacha.setIV(data.nonce, sizeof(data.nonce));

  // Decrypt the message
  size_t ciphertextLength = ciphertextHex.length() / 2;
  chacha.decrypt(decryptedText, ciphertext, ciphertextLength);

  // Compute and verify the tag
  uint8_t computedTag[16];
  chacha.computeTag(computedTag, sizeof(computedTag));
  if (memcmp(tag, computedTag, sizeof(tag)) != 0) {
    Serial.println("Tag verification failed!");
    logs = "Tag verification failed!";
    Serial.println(logs);
    jsonPayload = "{\"log\": \"" + logs + "\"}";
    sendEncryptedData(jsonPayload, logsend);
    return "";  // Return an empty string if the tag verification fails
  }

  // Convert decrypted text to string
  String decryptedString = "";
  for (size_t i = 0; i < ciphertextLength; i++) {
    decryptedString += (char)decryptedText[i];
  }

  return decryptedString;
}


// Function to convert hex string to byte array
void hexStringToBytes(String hexString, uint8_t* bytes) {
  for (size_t i = 0; i < hexString.length() / 2; i++) {
    bytes[i] = (uint8_t)strtol(hexString.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
  }
}

// Check rain sensor status and send "stop" command if raining for 10 minutes
void checkRainSensor() {
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  Serial.println(rainValue);

  delay(1000);
  if (rainValue < 500) {  // Threshold to detect rain
    if (!raining) {
      raining = true;
      rainStartTime = millis();
    } else if (millis() - rainStartTime >= 10 * 60 * 1000) {  // 10 minutes
      String encryptedPayload = encryptMessage("0", stat);
      sendEncryptedData(encryptedPayload, AmFed);
      encryptedPayload = encryptMessage("0", stat);
      sendEncryptedData(encryptedPayload, PmFed);
      ns = "0";
      ms = "0";
    }
  } else {
    raining = false;  // Reset if no rain is detected
  }
}

// Enter deep sleep until 6 AM
void deepSleepUntil6AM(DateTime now) {
  String encryptedPayload = encryptMessage("1", stat);
  sendEncryptedData(encryptedPayload, AmFed);
  encryptedPayload = encryptMessage("1", stat);
  sendEncryptedData(encryptedPayload, PmFed);
  ns = "1";
  ms = "1";
  int secondsUntil6AM = ((24 + 6 - now.hour()) % 24) * 3600 - now.minute() * 60 - now.second();
  ESP.deepSleep(secondsUntil6AM * 1000000);
}

// Function to encrypt the message
String encryptMessage(const char* plaintext, module data) {
  ChaChaPoly chacha;
  uint8_t ciphertext[128];
  uint8_t tag[16];

  // Encrypt the message and generate the authentication tag
  chacha.clear();
  chacha.setKey(data.key, sizeof(data.key));
  chacha.setIV(data.nonce, sizeof(data.nonce));
  size_t plaintextLength = strlen(plaintext);
  chacha.encrypt(ciphertext, (uint8_t*)plaintext, plaintextLength);
  chacha.computeTag(tag, sizeof(tag));

  // Convert ciphertext to hex string
  String encryptedData = "";
  for (size_t i = 0; i < plaintextLength; i++) {
    if (ciphertext[i] < 0x10) {
      encryptedData += "0";  // Ensure each byte is represented by two hex digits
    }
    encryptedData += String(ciphertext[i], HEX);
  }
  // Convert tag to hex string
  String tagString = "";
  for (size_t i = 0; i < sizeof(tag); i++) {
    if (tag[i] < 0x10) {
      tagString += "0";  // Ensure each byte is represented by two hex digits
    }
    tagString += String(tag[i], HEX);
  }

  // Create JSON payload
  String payload = "{";
  payload += "\"ciphertext\":\"" + encryptedData + "\",";
  payload += "\"tag\":\"" + tagString + "\"";
  payload += "}";
  return payload;
}
void sendEncryptedData(String encryptedPayload, String serverSend) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    X509List cert(root_ca);
    client.setTrustAnchors(&cert);

    if (!client.connect(host, httpsPort)) {
      Serial.println("Connection failed!");
      return;
    }
    // Make an HTTPS POST request
    String request = String("POST " + serverSend + " HTTP/1.1\r\n") + "Host: " + host + "\r\n" + "Content-Type: application/json\r\n" + "Content-Length: " + encryptedPayload.length() + "\r\n" + "Connection: close\r\n\r\n" + encryptedPayload;
    //Serial.println(request);
    client.print(request);

    // Read the response
    Serial.println("Response:");
    // while (client.connected() || client.available()) {
    //   if (client.available()) {
    //     String line = client.readStringUntil('\n');
    //     Serial.println(line);
    //   }
    // }

    client.stop();
  } else {
    Serial.println("Not connected to WiFi.");
  }
}


void setSystemTimeFromRTC() {
  timeClient.begin();

    // Ensure NTP time is synchronized
    Serial.println("Fetching NTP time...");
    int retries = 10;  // Maximum retries for NTP update
    while (!timeClient.update() && retries > 0) {
        Serial.println("Failed to get NTP time, retrying...");
        timeClient.forceUpdate();  // Force update for NTP
        delay(1000);  // Retry after 1 second
        retries--;
    }

    if (retries == 0) {
        Serial.println("Failed to get NTP time after multiple retries");
    } else {
        Serial.println("NTP time updated successfully");
        Serial.println("Current time: " + timeClient.getFormattedTime());
    }

    // Get the current time from NTP and update RTC
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();  // Fetch the current time from NTP server

        // Convert NTP time to DateTime and update the RTC
        time_t epochTime = timeClient.getEpochTime();
        DateTime now = DateTime(epochTime);
        
        rtc.adjust(now);  // Update RTC with NTP time

        Serial.print("RTC Time Updated: ");
        printDateTime(now);
    } else {
        // If not connected to Wi-Fi, fallback to compiled time
        DateTime compiled = DateTime(F(__DATE__), F(__TIME__));
        Serial.print("Compiled Time: ");
        printDateTime(compiled);
        
        // Set the RTC time to the compiled time
        rtc.adjust(compiled);
    }

    // Verify if the time was set correctly
    DateTime now = rtc.now();
    Serial.print("RTC time after setting: ");
    printDateTime(now);

    // Set system time from RTC
    struct tm timeinfo;
    timeinfo.tm_year = now.year() - 1900;
    timeinfo.tm_mon = now.month() - 1;
    timeinfo.tm_mday = now.day();
    timeinfo.tm_hour = now.hour();
    timeinfo.tm_min = now.minute();
    timeinfo.tm_sec = now.second();

    time_t t = mktime(&timeinfo);
    timeval epoch = {t, 0};
    settimeofday(&epoch, nullptr);  // Set the system time from RTC

    Serial.print("System time: ");
    Serial.println(asctime(&timeinfo));
}
// Function to print DateTime object
void printDateTime(const DateTime& dt) {
    char datestring[20];
    snprintf(datestring, sizeof(datestring),
            "%04d/%02d/%02d %02d:%02d:%02d",
            dt.year(), dt.month(), dt.day(),
            dt.hour(), dt.minute(), dt.second());
    Serial.println(datestring);
}

//RTC_DS3231 module 
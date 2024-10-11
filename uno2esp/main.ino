#include <Arduino.h>
#include "Config.h"
#include "WiFiManager.h"
#include "RTCManager.h"
#include "WeightSensor.h"
#include "RainSensor.h"
#include "FeedingSystem.h"
#include "ServerCommunication.h"

WiFiManager wifiManager;
RTCManager rtcManager;
WeightSensor weightSensor(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
RainSensor rainSensor(RAIN_SENSOR_PIN);
FeedingSystem feedingSystem;
ServerCommunication serverComm;

void setup() {
  Serial.begin(9600);
  
  wifiManager.begin(WIFI_SSID, WIFI_PASSWORD);
  rtcManager.begin();
  weightSensor.begin();
  rainSensor.begin();
  feedingSystem.begin();
  serverComm.begin();

  pinMode(WAKEUP_PIN, OUTPUT);
  digitalWrite(WAKEUP_PIN, HIGH);

  rtcManager.setSystemTimeFromRTC();

  // Check if it's time to sleep
  if (rtcManager.isOutsideActiveHours()) {
    feedingSystem.prepareSleep();
    rtcManager.sleepUntil6AM();
  }
}

void loop() {
  wifiManager.maintainConnection();

  int weight = weightSensor.getWeight();
  feedingSystem.updateWeight(weight);

  if (feedingSystem.shouldTriggerAlarm(weight)) {
    tone(ACTIVE_BUZZER, 6000, 255);
  } else {
    noTone(ACTIVE_BUZZER);
  }

  rainSensor.check();
  if (rainSensor.isRainingLongEnough()) {
    feedingSystem.stopFeeding();
  }

  if (wifiManager.isConnected()) {
    serverComm.sendWeightUpdate(weight);
    serverComm.getAndProcessServerData(feedingSystem);
    feedingSystem.performScheduledFeeding(rtcManager.now());
  }

  // Check for sleep condition
  if (rtcManager.isOutsideActiveHours()) {
    feedingSystem.prepareSleep();
    rtcManager.sleepUntil6AM();
  }

  delay(MAIN_LOOP_DELAY);
}
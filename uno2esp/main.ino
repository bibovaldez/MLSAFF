#include <SoftwareSerial.h>
#include <Servo.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>

class FishFeeder {
private:
  static const uint8_t LOCK_PIN = 9;
  static const uint8_t FEEDER_PIN = 8;
  static const uint8_t SERVO_PIN = 10;
  static const uint8_t INTERRUPT_PIN = 7;
  static const uint8_t RX_PIN = 3;
  static const uint8_t TX_PIN = 2;

  Servo myServo;
  SoftwareSerial espSerial;

public:
  FishFeeder() : espSerial(RX_PIN, TX_PIN) {}

  void setup() {
    Serial.begin(9600);
    espSerial.begin(9600);
    
    pinMode(LOCK_PIN, OUTPUT);
    pinMode(FEEDER_PIN, OUTPUT);
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    
    myServo.attach(SERVO_PIN);
    
    digitalWrite(LOCK_PIN, LOW);
    digitalWrite(FEEDER_PIN, HIGH);
    myServo.write(0);
    
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), [](){}, LOW);
  }

  void loop() {
    if (espSerial.available()) {
      String data = espSerial.readStringUntil('\n');
      data.trim();
      Serial.println("Received from ESP8266: " + data);
      
      processCommand(data);
    }
  }

private:
  void processCommand(const String& cmd) {
    if (cmd == "b") {
      Serial.println("b");
    } else if (cmd == "a") {
      openCloseLock();
    } else if (cmd == "f") {
      feed();
    } else if (cmd == "s") {
      stopFeeding();
    } else if (cmd == "d") {
      enterSleep();
    }
  }

  void openCloseLock() {
    digitalWrite(LOCK_PIN, HIGH);
    Serial.println("open storage");
    delay(7000);
    digitalWrite(LOCK_PIN, LOW);
    Serial.println("closed storage");
  }

  void feed() {
    digitalWrite(FEEDER_PIN, LOW);
    myServo.write(35);
    myServo.write(0);
    myServo.write(35);
    Serial.println("feeding");
  }

  void stopFeeding() {
    myServo.write(0);
    delay(10000);
    digitalWrite(FEEDER_PIN, HIGH);
    Serial.println("stop");
  }

  void enterSleep() {
    Serial.println("Entering sleep mode...");
    delay(100);  // Allow time for serial to finish transmitting
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), [](){}, LOW);
    
    sleep_mode();
    
    // Execution resumes here after wake-up
    sleep_disable();
    detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN));
    Serial.println("Woke up!");
  }
};

FishFeeder fishFeeder;

void setup() {
  fishFeeder.setup();
}

void loop() {
  fishFeeder.loop();
}
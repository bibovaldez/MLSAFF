#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

class Utilities {
public:
    static void setSystemTime(const DateTime& dt);
    static String createJsonPayload(const String& key, const String& value);
    static void printDebug(const String& message);
    static void blinkLED(int pin, int times, int duration);
};

#endif // UTILITIES_H
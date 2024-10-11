#ifndef RAIN_SENSOR_H
#define RAIN_SENSOR_H

#include <Arduino.h>

class RainSensor {
public:
    RainSensor(int pin);
    void begin();
    void check();
    bool isRaining();
    bool isRainingLongEnough();

private:
    int _pin;
    bool _raining;
    unsigned long _rainStartTime;
    static const int RAIN_THRESHOLD = 500;
    static const unsigned long RAIN_DURATION_THRESHOLD = 600000; // 10 minutes in milliseconds
};

#endif // RAIN_SENSOR_H
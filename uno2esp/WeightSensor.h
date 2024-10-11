#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "HX711.h"

class WeightSensor {
public:
    WeightSensor(int dout_pin, int sck_pin);
    void begin();
    void calibrate();
    int getWeight();

private:
    HX711 _scale;
    int _dout_pin;
    int _sck_pin;
    float _calibration_factor;
};

#endif // WEIGHT_SENSOR_H
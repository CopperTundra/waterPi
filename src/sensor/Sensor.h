#ifndef SENSOR_H
#define SENSOR_H

#define INVALID_RES 1000000
#include <cstdint>
#pragma once

enum SensorType {
    SENSOR_DHT22 = 0,
    SENSORTYPENUMBER
};

class Sensor
{

public:
    Sensor();
    Sensor(SensorType type, uint8_t pinNumber);
    float GetValue();
    virtual bool fetchHumidity() = 0;
    virtual bool init() = 0;
    virtual float getHumidity() = 0;
protected:
    SensorType type;
    uint8_t pinNumber;
private:
    float value;
};

#endif
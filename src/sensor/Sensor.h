#ifndef SENSOR_H
#define SENSOR_H

#define INVALID_RES 1000000
#include <cstdint>
#include <nlohmann/json.hpp>
#pragma once

enum SensorType {
    SENSOR_DHT22 = 0,
    SENSORTYPENUMBER
};

NLOHMANN_JSON_SERIALIZE_ENUM(SensorType, {
    {SENSOR_DHT22, "DHT22"}
})

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
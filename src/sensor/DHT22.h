#ifndef DHT22_H
#define DHT22_H

#include "Sensor.h"
#include <cstdint>

class DHT22 : public Sensor
{
public:
    DHT22();
    DHT22(SensorType type, uint8_t pinNumber);
    virtual bool fetchHumidity();
    virtual bool init();
    virtual float getHumidity();

private:
    int32_t temp, rh;
};

#endif
#include "DHT22.h"
#include "sensor/Sensor.h"
#include <maxdetect.h>

DHT22::DHT22()
{

}

DHT22::DHT22(SensorType type, uint8_t pinNumber)
: Sensor(type,pinNumber)
{

}

bool DHT22::init()
{
    /* No init is needed for DHT 22 sensor */
    return true;
}

bool DHT22::fetchHumidity()
{
    if(readRHT03(pinNumber, &temp, &rh))
    {
        return true;
    }
    else {
        temp = rh = INVALID_RES;
        return false;
    }
}

float DHT22::getHumidity()
{
    return (rh / 10.0);
}
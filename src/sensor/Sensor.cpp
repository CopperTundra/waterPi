#include "Sensor.h"

Sensor::Sensor()
{

}
Sensor::Sensor(SensorType type, uint8_t pinNumber)
: type(type), pinNumber(pinNumber)
{

}

float Sensor::GetValue()
{
    return value;
}
#include "Plant.h"
#include "Valve.h"
#include "sensor/Sensor.h"

Plant::Plant()
{

}
Plant::Plant(std::string name, Sensor* sensor, Valve* valve)
: name(name), humSensor(sensor), valve(valve)
{

}

bool Plant::fetchHumidity()
{
    return (humSensor->fetchHumidity());
}

float Plant::getHumidity()
{
    humidity = humSensor->getHumidity();
    return humidity;
}

bool Plant::checkAndWater()
{
    // TODO
    return true;
}
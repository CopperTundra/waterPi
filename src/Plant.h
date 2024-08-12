#ifndef PLANT_H
#define PLANT_H

#include "Valve.h"
#include "sensor/Sensor.h"
#include <string>
#pragma once

class Plant
{
public:
    Plant();
    Plant(std::string name, Sensor* sensor, Valve* valve);
    bool checkAndWater();
    bool fetchHumidity();
    float getHumidity();

private:
    std::string name;
    Sensor* humSensor;
    Valve* valve;
    float humidity;

};

#endif
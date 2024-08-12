/*
* GNU GENERAL PUBLIC LICENSE
* Version 3, 29 June 2007
* 
* Copyright (C) Olivier Schüwer
* 
* WaterPi is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* WaterPi is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with WaterPi.  If not, see <https://www.gnu.org/licenses/>.
*/

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
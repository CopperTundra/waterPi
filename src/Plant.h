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
#include "sensor/DHT22.h"
#include <string>
#include <uuid/uuid.h>
#pragma once

class Plant
{
public:
    Plant(std::string name, DHT22* sensor, Valve* valve);
    Plant(std::string name, std::string uid, DHT22* sensor, Valve* valve);
    Plant(std::string name, DHT22* sensor, Valve* valve, uint16_t wateringTime, float minHumidity);
    Plant(std::string name, std::string uid, DHT22* sensor, Valve* valve, uint16_t wateringTime, float minHumidity);
    bool getHumidity(float* humidity);
    std::string getName() { return _name; }
    std::string getUid() { return _uid; }
    float getMinHumidity() { return _minHumidity; }
    void setWateringTime(uint16_t time) { _wateringTime = time; }
    void waterPlant();
    void waterPlant(uint16_t time);
    DHT22* _humSensor;

private:
    std::string _name;
    std::string _uid;
    Valve* _valve;
    float _humidity;
    uint16_t _wateringTime = 3; // in seconds
    float _minHumidity;

    void _generateUid();
};

#endif
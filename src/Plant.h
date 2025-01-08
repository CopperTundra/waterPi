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
    Plant(std::string name, DHT22* sensor, Valve* valve, uint16_t wateringTime, float humidityThreshold);
    Plant(std::string name, std::string uid, std::string room, DHT22* sensor, Valve* valve, uint16_t wateringTime, float humidityThreshold);
    bool getHumidity(float* humidity);
    std::string getName() { return _name; }
    std::string getUid() { return _uid; }
    float getHumidityThreshold() { return _humidityThreshold; }
    void setWateringTime(uint16_t time) { _wateringTime = time; }
    void setHumidityThreshold(float humidityThreshold) { _humidityThreshold  = humidityThreshold; }
    void setSensorPin(uint8_t pin) { _humSensor->setPin(pin); }
    void setValvePin(uint8_t pin) { _valve->setPin(pin); }
    void setRoom(std::string room) { _room = room; }
    void setName(std::string name) { _name = name; }

    void waterPlant();
    void waterPlant(uint16_t time);
    DHT22* _humSensor;

private:
    std::string _name;
    std::string _uid;
    std::string _room;
    Valve* _valve;
    float _humidity;
    uint16_t _wateringTime = 3; // in seconds
    float _humidityThreshold;

    void _generateUid();
};

#endif
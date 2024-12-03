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

#include "Plant.h"

Plant::Plant(std::string name, DHT22* sensor, Valve* valve)
: _name(name), _humSensor(sensor), _valve(valve)
{
    _generateUid();
}

Plant::Plant(std::string name, std::string uid, DHT22* sensor, Valve* valve)
: _name(name), _uid(uid), _humSensor(sensor), _valve(valve)
{

}

Plant::Plant(std::string name, DHT22* sensor, Valve* valve, uint16_t wateringTime, float minHumidity)
: _name(name), _humSensor(sensor), _valve(valve), _wateringTime(wateringTime), _minHumidity(minHumidity)
{
    _generateUid();
}

Plant::Plant(std::string name, std::string uid, DHT22* sensor, Valve* valve, uint16_t wateringTime, float minHumidity)
: _name(name), _uid(uid), _humSensor(sensor), _valve(valve), _wateringTime(wateringTime), _minHumidity(minHumidity)
{

}

bool Plant::getHumidity(float* humidity)
{
    return _humSensor->GetHumidity(humidity);
}

void Plant::waterPlant()
{
    _valve->open(_wateringTime);
}

void Plant::waterPlant(uint16_t time)
{
    _valve->open(time);
}

void Plant::_generateUid()
{
    uuid_t uuid;
    uuid_generate(uuid);
    char uuidStr[37];
    uuid_unparse(uuid, uuidStr);
    _uid = uuidStr;
}
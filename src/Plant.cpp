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
#include "Valve.h"
#include "sensor/DHT22.h"

Plant::Plant(std::string name, DHT22* sensor, Valve* valve)
: _name(name), _humSensor(sensor), _valve(valve)
{

}

Plant::Plant(std::string name, DHT22* sensor, Valve* valve, uint16_t wateringTime)
: _name(name), _humSensor(sensor), _valve(valve), _wateringTime(wateringTime)
{

}

bool Plant::fetchHumidity()
{
    return (_humSensor->fetchHumidity());
}

float Plant::getHumidity()
{
    _humidity = _humSensor->getHumidity();
    return _humidity;
}

void Plant::waterPlant()
{
    _valve->open(_wateringTime);
}
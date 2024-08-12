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

#ifndef SENSOR_H
#define SENSOR_H

#define INVALID_RES 1000000
#include <cstdint>
#include <nlohmann/json.hpp>
#pragma once

enum SensorType {
    SENSOR_DHT22 = 0,
    SENSORTYPENUMBER
};

NLOHMANN_JSON_SERIALIZE_ENUM(SensorType, {
    {SENSOR_DHT22, "DHT22"}
})

class Sensor
{

public:
    Sensor();
    Sensor(SensorType type, uint8_t pinNumber);
    float GetValue();
    virtual bool fetchHumidity() = 0;
    virtual bool init() = 0;
    virtual float getHumidity() = 0;
protected:
    SensorType type;
    uint8_t pinNumber;
private:
    float value;
};

#endif
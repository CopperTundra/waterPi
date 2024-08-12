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

#include "DHT22.h"
#include "sensor/Sensor.h"
#include <cstdlib>
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
    srand (static_cast <unsigned> (time(0)));
    return true;
}

bool DHT22::fetchHumidity()
{
    // DHT22 sensor is not connected to the Raspberry Pi
    rh = rand()%(1000-0 + 1) + 0;
    temp = rand()%(1000-0 + 1) + 0;;
    return true;

    // if(readRHT03(pinNumber, &temp, &rh))
    // {
    //     return true;
    // }
    // else {
    //     temp = rh = INVALID_RES;
    //     return false;
    // }
}

float DHT22::getHumidity()
{
    return (rh / 10.0);
}
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
#include <cstdio>
#include <iostream>
#include <maxdetect.h>

DHT22::DHT22(uint8_t pinNumber)
: pinNumber(pinNumber)

{
    std::cout << "DHT created, pinNumber: " << +pinNumber << std::endl;
    printf("DEBUG: &pinNumber %p, &rh: %p, &temp: %p\n",&pinNumber, &rh, &temp);
}

bool DHT22::init()
{
    /* No init is needed for DHT22 sensor */
    return true;
}

bool DHT22::fetchHumidity()
{
    if(readRHT03(pinNumber, &temp, &rh)) {
        std::cout << "pinNumber: " << +pinNumber << " temp: " << (float)temp/10 << " rh: " << (float)rh/10 << std::endl;
        return true;
    }
    else {
        temp = rh = INVALID_RES;
        return false;
    }
}

float DHT22::getHumidity()
{
    return (rh / 10.0);
}
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

#ifndef DHT22_H
#define DHT22_H
#include <sys/time.h>
#include <cstdint>

#pragma once

#define INVALID_RES 1000000

class DHT22
{
public:
    DHT22(uint8_t pin);
    bool GetHumidity(float* humidity);
    void setPin(uint8_t pin) { pinNumber = pin; }

private:
    int32_t _temp, _rh;
    struct timeval _then;
    int pinNumber;
    bool readDHTvalue(const int pin);
};

#endif
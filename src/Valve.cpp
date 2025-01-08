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

#include "Valve.h"

#ifndef UNIT_TEST
#include <wiringPi.h>
#endif

Valve::Valve()
{

}
Valve::Valve(ValveType type, unsigned char pin)
: _type(type), _pin(pin)
{
#ifndef UNIT_TEST
    pinMode(_pin, OUTPUT);
#endif
}
Valve::~Valve()
{

}

void Valve::open(uint16_t seconds)
{
#ifndef UNIT_TEST
    digitalWrite(_pin, HIGH);
    delay(seconds * 1000);
    digitalWrite(_pin, LOW);
#endif
}
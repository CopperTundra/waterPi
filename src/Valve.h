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

#ifndef VALVE_H
#define VALVE_H

#include <cstdint>
#include <nlohmann/json.hpp>
#pragma once

enum ValveType {
    solenoid = 0,
    VALVETYPENUMBER
};

NLOHMANN_JSON_SERIALIZE_ENUM(ValveType, {
    {solenoid, "solenoid"}
})

class Valve
{
public:
    Valve();
    Valve(ValveType type, uint8_t pin);
    ~Valve();
    void open(uint16_t seconds);
    void setPin(uint8_t pin) { _pin = pin; }

private:
    ValveType _type;
    uint8_t _pin;
};

#endif
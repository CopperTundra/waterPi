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

private:
    ValveType type;
    uint8_t pin;
};

#endif
#ifndef VALVE_H
#define VALVE_H

#include <cstdint>
#pragma once

enum ValveType {
    solenoid = 0,
    VALVETYPENUMBER
};

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
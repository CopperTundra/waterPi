#include "Valve.h"
#include <cstdint>

Valve::Valve()
{

}
Valve::Valve(ValveType type, uint8_t pin)
: type(type), pin(pin)
{

}
Valve::~Valve()
{

}
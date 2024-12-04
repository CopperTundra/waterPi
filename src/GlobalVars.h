#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <atomic>
#include <vector>
#include "Plant.h"

extern std::atomic<bool> terminateFlag;
extern std::vector<Plant*> plants;

#endif

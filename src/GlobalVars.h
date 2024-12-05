#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <atomic>
#include <mutex>
#include <vector>
#include "Plant.h"

extern std::atomic<bool> terminateFlag;
extern std::vector<Plant*> plants;
extern std::mutex mutex_plants;

#endif

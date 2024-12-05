#include "GlobalVars.h"

std::atomic<bool> terminateFlag{false};

std::vector<Plant *> plants;
std::mutex mutex_plants;

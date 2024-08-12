#ifndef CONFIG_H
#define CONFIG_H

#include "Plant.h"
#include <cstdint>
#include <string>
#include <vector>
#pragma once

class Config
{
public:
    Config(std::string path);
    ~Config();
    bool parseConfig();
    bool fetchPlantData();

private:
    uint8_t configVersion;
    std::string configPath;
    std::vector<Plant*> plants;
};

#endif
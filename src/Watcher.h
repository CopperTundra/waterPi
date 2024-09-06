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

#ifndef WATCHER_H
#define WATCHER_H

#include "Plant.h"
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#pragma once

struct PlantData {
    Plant* plant;
    float humidity;
    float minHumidity;
};
class Watcher
{
    void watch();
    void checkAndWater();

    std::thread _watchThread;
    bool _alive = false;
    std::condition_variable _cv;
    std::mutex _mutex;

    std::vector<PlantData> _plantData;

public:
    Watcher();
    ~Watcher();
    void addPlant(Plant* plant);
    void setMinHumidity(float min, std::string name);
    void spawnThread();
    void stop();
};

#endif
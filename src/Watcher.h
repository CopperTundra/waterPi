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

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#pragma once


class Watcher
{
    void watch();
    void checkAndWater();

    std::thread _watchThread;
    bool _alive = false;
    std::condition_variable _cv;
    std::mutex _mutex;

    std::atomic<bool> _timeToWater{false};
public:
    Watcher();
    ~Watcher();
    void spawnThread();
    void stop();
    void resetTimer() { _timeToWater = false; }
    bool isTimeToWater() { return _timeToWater; }
};

#endif
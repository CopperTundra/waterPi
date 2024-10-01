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

#include "Watcher.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

Watcher::Watcher()
{

}

Watcher::~Watcher()
{
    _alive = false;
    if (_watchThread.joinable()) {
        _watchThread.join();
    }
}

void Watcher::watch()
{
    std::cout << "Plant monitor thread started!\r\n";
    _alive = true;
    while (_alive) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_cv.wait_for(lock, std::chrono::minutes(1), [this] { return !_alive; })) {
            break;
        }
        std::cout << "Plant monitor thread woke up!\r\n";
        _timeToWater = true;
    }
}

void Watcher::stop()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _alive = false;
    }
    _cv.notify_all();
}

void Watcher::spawnThread() 
{
    if (_watchThread.joinable()) {
        _watchThread.join();
    }
    _watchThread = std::thread(&Watcher::watch,this);
}

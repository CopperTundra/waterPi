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

#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include "Plant.h"
#include <condition_variable>
#include <thread>
#include <string>
#include <vector>

#pragma once

namespace ApiEndpoint {
const std::string POST_PLANT_VALUES = "/values";
const std::string GET_PLANT_INFO = "/plants";
const std::string POST_PLANT_INFO = "/plants";
const std::string DELETE_PLANT_INFO = "/plants";
const std::string GET_LIMIT = "/limit";
const std::string GET_MANUAL_WATERING = "/watering";
};

class WebClient
{
public:
  WebClient(std::string configPath);
  WebClient(std::string configPath, const char *url);
  ~WebClient();
  void spawnThread();
  void stop();
  void addPlant(Plant* plant) {_plants.push_back(plant);}

private:
  const char *_url;
  std::string _configPath;
  std::vector<Plant*> _plants;

  std::thread _webclientThread;
  bool _alive = false;

  std::condition_variable _cv;
  std::mutex _mutex;

  void webClientThread();
  bool getPlantInfo();
  bool postPlantInfo();
  bool postValues();
};

#endif
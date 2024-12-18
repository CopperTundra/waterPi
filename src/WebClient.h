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

#include <condition_variable>
#include <cstdint>
#include <thread>
#include <string>

#pragma once

namespace ApiEndpoint {
const std::string GET_PLANT_INFO = "/plants"; // to the server
const std::string POST_PLANT_VALUES = "/plants"; // + /[UID] ; to the server
const std::string POST_WEBHOOK_EVENT = "/webhooks"; // to the server
const std::string POST_PLANT_INFO = "/plants"; // from the server
const std::string PATCH_PLANT_INFO = "/plants"; // + /[UID] ; from the server
const std::string DELETE_PLANT_INFO = "/plants"; // + /[UID] ; from the server
const std::string POST_PLANT_TRIGGER_WATER = "/plants"; // + /[UID]/water ; from the server
const std::string POST_STOP_WATER = "/plants"; // + /[UID]/stop-water ; from the server
};

namespace WebhookEvent {
  const std::string NEW_PLANT = "new_plant";
  const std::string WATERING_REQUEST = "watering_request";
  const std::string DELETE_PLANT = "delete_plant";
  const std::string PATCH_PLANT = "patch_plant";
  const std::string STOP_WATERING_REQUEST = "stop_watering_request";
}

class WebClient
{

  class GET_PLANT_INFO {
    const std::string endpoint = ApiEndpoint::GET_PLANT_INFO;
    std::string uid;
    std::string name;
    std::string room;
    std::string humidity_threshold;
    std::string watering_time_seconds;
    std::string current_humidity;
    std::string sensor_pin_number;
    std::string valve_pin_number;
  };

  class POST_PLANT_VALUES {
    const std::string endpoint;
    std::string current_humidity;
    POST_PLANT_VALUES(std::string uid) 
      : endpoint(ApiEndpoint::POST_PLANT_VALUES + "/" + uid) {}
  };

  class POST_WEBHOOK_EVENT {
    const std::string endpoint = ApiEndpoint::POST_WEBHOOK_EVENT;
    std::string event;
    std::string url;
  };

  class POST_PLANT_INFO {
    const std::string endpoint = ApiEndpoint::POST_PLANT_INFO;
    std::string uid;
    std::string name;
    std::string room;
    std::string humidity_threshold;
    std::string watering_time_seconds;
    std::string current_humidity;
    std::string sensor_pin_number;
    std::string valve_pin_number;
  };

  class PATCH_PLANT_INFO {
    const std::string endpoint;
    std::string name;
    std::string room;
    std::string humidity_threshold;
    std::string watering_time_seconds;
    std::string current_humidity;
    std::string sensor_pin_number;
    std::string valve_pin_number;
    PATCH_PLANT_INFO(std::string uid)
        : endpoint(ApiEndpoint::PATCH_PLANT_INFO + "/" + uid) {}
  };

  class DELETE_PLANT_INFO {
    const std::string endpoint;
    DELETE_PLANT_INFO(std::string uid)
        : endpoint(ApiEndpoint::DELETE_PLANT_INFO + "/" + uid) {}
  };

  class POST_PLANT_TRIGGER_WATER {
    const std::string endpoint;
    std::string watering_time_seconds;
    POST_PLANT_TRIGGER_WATER(std::string uid)
        : endpoint(ApiEndpoint::POST_PLANT_TRIGGER_WATER + "/" + uid + "/water") {}
  };

  class POST_STOP_WATER {
    const std::string endpoint;
    POST_STOP_WATER(std::string uid)
        : endpoint(ApiEndpoint::POST_STOP_WATER + "/" + uid + "/stop-water") {}
  };

public:
  WebClient(std::string configPath);
  WebClient(std::string configPath, const char *url);
  WebClient(std::string configPath, const char *url, uint16_t portDest, uint16_t portSrc);
  ~WebClient();
  void spawnThread();
  void stop();

private:
  std::string _url; // url including destination port
  const uint16_t _portDest;
  const uint16_t _portSrc;
  std::string _configPath;

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
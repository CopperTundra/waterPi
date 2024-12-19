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
#include <sys/types.h>
#include <thread>
#include <string>
#include <array>
#include <../cpp-httplib/httplib.h>

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
  const uint8_t NUM_EVENTS = 5;

  const std::array<std::string,NUM_EVENTS> all_events = {
    NEW_PLANT,
    WATERING_REQUEST,
    DELETE_PLANT,
    PATCH_PLANT,
    STOP_WATERING_REQUEST
  };
}

class WebClient
{

  class GET_PLANT_INFO {
  public:
    const std::string endpoint = ApiEndpoint::GET_PLANT_INFO;
    const std::string uid = "uid";
    const std::string name = "name";
    const std::string room = "room";
    const std::string humidity_threshold = "humidity_threshold";
    const std::string watering_time_seconds = "watering_time_seconds";
    const std::string current_humidity = "current_humidity";
    const std::string sensor_pin_number = "sensor_pin_number";
    const std::string valve_pin_number = "valve_pin_number";
  };

  class POST_PLANT_VALUES {
  public:
    const std::string endpoint;
    const std::string current_humidity = "current_humidity";
    POST_PLANT_VALUES(std::string uid) 
      : endpoint(ApiEndpoint::POST_PLANT_VALUES + "/" + uid) {}
  };

  class POST_WEBHOOK_EVENT {
  public:
    const std::string endpoint = ApiEndpoint::POST_WEBHOOK_EVENT;
    struct event {
        const std::string event_name = "event";
        std::string event_field;
        event(std::string event_field) : event_field(event_field) {}
        void setEventField(std::string new_event_field) {
            event_field = new_event_field;
        }
    };

    const std::string url = "url";
    
    event event_instance;

    POST_WEBHOOK_EVENT(std::string event_field) : event_instance(event_field) {}

    void setEvent(std::string new_event_field) {
        event_instance.setEventField(new_event_field);
    }
  };

  class POST_PLANT_INFO {
  public:
    const std::string endpoint = ApiEndpoint::POST_PLANT_INFO;
    const std::string uid = "uid";
    const std::string name = "name";
    const std::string room = "room";
    const std::string humidity_threshold = "humidity_threshold";
    const std::string watering_time_seconds = "watering_time_seconds";
    const std::string current_humidity = "current_humidity";
    const std::string sensor_pin_number = "sensor_pin_number";
    const std::string valve_pin_number = "valve_pin_number";
  };

  class PATCH_PLANT_INFO {
  public:
    const std::string endpoint;
    const std::string name = "name";
    const std::string room = "room";
    const std::string humidity_threshold = "humidity_threshold";
    const std::string watering_time_seconds = "watering_time_seconds";
    const std::string current_humidity = "current_humidity";
    const std::string sensor_pin_number = "sensor_pin_number";
    const std::string valve_pin_number = "valve_pin_number";
    PATCH_PLANT_INFO(std::string uid)
        : endpoint(ApiEndpoint::PATCH_PLANT_INFO + "/" + uid) {}
  };

  class DELETE_PLANT_INFO {
  public:
    const std::string endpoint;
    DELETE_PLANT_INFO(std::string uid)
        : endpoint(ApiEndpoint::DELETE_PLANT_INFO + "/" + uid) {}
  };

  class POST_PLANT_TRIGGER_WATER {
  public:
    const std::string endpoint;
    const std::string watering_time_seconds = "watering_time_seconds";
    POST_PLANT_TRIGGER_WATER(std::string uid)
        : endpoint(ApiEndpoint::POST_PLANT_TRIGGER_WATER + "/" + uid + "/water") {}
  };

  class POST_STOP_WATER {
  public:
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
  void listenForServerNotifications();

  // Functions to handle data to the server
  bool getPlantInfo(); 
  bool postValues();
  bool postWebhookEvents();

  // Functions to handle data from the server
  void postPlantInfo(const httplib::Request &req, httplib::Response &res);
};

#endif
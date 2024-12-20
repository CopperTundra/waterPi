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

#include "WebClient.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <httplib.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include "Plant.h"
#include "Valve.h"
#include "sensor/DHT22.h"
#include <nlohmann/json.hpp>
#include "GlobalVars.h"

using json = nlohmann::json;

WebClient::WebClient(std::string configPath)
    : _portDest(1323), _portSrc(1620),_configPath(configPath) 
{
    _url = "http://localhost:" + std::to_string(_portDest);
}

WebClient::WebClient(std::string configPath, const char *url)
    : _portDest(1323), _portSrc(1620), _configPath(configPath) 
{
    _url = url + std::to_string(':') + std::to_string(_portDest);
}

WebClient::WebClient(std::string configPath, const char *url, uint16_t portDest, uint16_t portSrc)
    : _portDest(portDest), _portSrc(portSrc), _configPath(configPath) 
{
    _url = url + std::to_string(':') + std::to_string(_portDest);
}

WebClient::~WebClient()
{
    _alive = false;
    if (_webclientThread.joinable()) {
        _webclientThread.join();
    }
}

void WebClient::spawnThread()
{
    if (_webclientThread.joinable()) {
        _webclientThread.join();
    }
    _webclientThread = std::thread(&WebClient::webClientThread, this);
}

void WebClient::webClientThread()
{
    std::cout << "Web client thread started!\r\n";
    _alive = true;
    std::cout << "Getting plant data from server...\r\n";
    getPlantInfo();
    postWebhookEvents();

    std::thread sendDataThread([this]() {
        while (_alive) {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_cv.wait_for(lock, std::chrono::seconds(30),
                            [this] { return !_alive; })) {
                // woken up by stop()
                break;
            }
            else {
                // woken up by timer
                std::cout << "Woken up by timer.\r\n";
                postValues();
            std::cout << "Web client thread woke up!\r\n";
            }
        }
    });

    while (_alive) {
        listenForServerNotifications();
    }

    // Clean up
    if (sendDataThread.joinable()) {
        sendDataThread.join();
    }
}

void WebClient::listenForServerNotifications()
{
    httplib::Server svr;
    std::cout << "Listening on " << _url << ":" << _portSrc << "\r\n";

    auto post_plant_info = new POST_PLANT_INFO();
    svr.Get(post_plant_info->endpoint,
            std::bind(&WebClient::postPlantInfo, this, std::placeholders::_1,
                      std::placeholders::_2, post_plant_info));

    auto patch_plant_info = new PATCH_PLANT_INFO("");
    std::string exactUrl = patch_plant_info->endpoint + "/:uid";
    svr.Get(exactUrl,
            std::bind(&WebClient::patchPlantInfo, this, std::placeholders::_1,
                      std::placeholders::_2, patch_plant_info));

    auto delete_plant_info = new DELETE_PLANT_INFO("");
    exactUrl = delete_plant_info->endpoint + "/:uid";
    svr.Get(exactUrl,
            std::bind(&WebClient::deletePlantInfo, this, std::placeholders::_1,
                      std::placeholders::_2, delete_plant_info));

    auto post_plant_trigger_water = new POST_PLANT_TRIGGER_WATER("");
    exactUrl = post_plant_trigger_water->endpoint + "/:uid/water";
    svr.Get(exactUrl, std::bind(&WebClient::postPlantTriggerWater, this,
                                std::placeholders::_1, std::placeholders::_2,
                                post_plant_trigger_water));

    auto post_stop_water = new POST_STOP_WATER("");
    exactUrl = post_stop_water->endpoint + "/:uid/stop-water";
    svr.Get(exactUrl,
            std::bind(&WebClient::postStopWater, this, std::placeholders::_1,
                      std::placeholders::_2, post_stop_water));

    svr.listen(_url, _portSrc);
}

void WebClient::stop() {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _alive = false;
  }
  _cv.notify_all();
}

bool WebClient::getPlantInfo()
{
    httplib::Client cli(_url);
    httplib::Headers headers = {
        { "Content-Type", "application/json" }
    };
    std::cout << "Get request: " << _url << ApiEndpoint::GET_PLANT_INFO.c_str() << "\r\n";
    auto ApiCall = new GET_PLANT_INFO();
    auto res = cli.Get(ApiCall->endpoint.c_str(), headers);
    if (res && res->status == 200) {
        // TODO: improve the Plant, Valve and Sensor classes to handle the new data
        json j = json::parse(res->body);
        std::unique_lock<std::mutex> lock(mutex_plants);
        for (auto plant : j) {
            std::string uid = plant[ApiCall->uid];
            std::string name = plant[ApiCall->name];
            std::string room = plant[ApiCall->room];
            float humidity_threshold = (float) plant[ApiCall->humidity_threshold];
            uint16_t watering_time_seconds = (uint16_t) plant[ApiCall->watering_time_seconds];
            uint8_t sensor_pin_number = (uint8_t) plant[ApiCall->sensor_pin_number];
            uint8_t valve_pin_number = (uint8_t) plant[ApiCall->valve_pin_number];
            for(auto p : plants) {
                if (p->getUid() == uid) {
                    // p->setHumidityThreshold(humidity_threshold);
                    p->setWateringTime(watering_time_seconds);
                    // p->setSensorPin(sensor_pin_number);
                    // p->setValvePin(valve_pin_number);
                    std::cout << "DEBUG: Plant " << name << ", UID " << uid << " updated.\r\n";
                    continue;
                }
            }
            DHT22* sensor = new DHT22(sensor_pin_number);
            Valve* valve = new Valve(ValveType::solenoid, valve_pin_number);
            Plant *p = new Plant(name, uid, sensor, valve);
            plants.push_back(p);
            std::cout << "DEBUG: Plant " << plant["name"] << ", UID " << plant["uid"] << " added.\r\n";
        }
        std::cout << "DEBUG: Plant info retrieved successfully!" << std::endl;
        return true;
    } else {
        std::cout << "DEBUG: Failed to retrieve plant info\r\n";
        if (res) {
            std::cout << "DEBUG: Error " << res->status << "\r\n";
        }
        return false;
    }
    return false;
}

bool WebClient::postWebhookEvents()
{
    httplib::Client cli(_url);
    httplib::Headers headers = {
        { "Content-Type", "application/json" }
    };
    bool val = true;
    for (uint8_t i = 0; i < WebhookEvent::NUM_EVENTS; i++)
    {
        auto ApiCall = new POST_WEBHOOK_EVENT(WebhookEvent::all_events[i]);
        auto body = ApiCall->event_instance.event_name + "=" + ApiCall->event_instance.event_field;
        auto res = cli.Post(ApiCall->endpoint.c_str(), headers, body,
                            "application/json");
        if (res && res->status == 200) {
            std::cout << "DEBUG: Webhook event posted successfully!" << std::endl;
        } else {
            std::cout << "DEBUG: Failed to post webhook event \r\n";
            if (res) {
            std::cout << "DEBUG: Error " << res->status << "\r\n";
            }
            val = false;
        }
    }
    return val;
}

bool WebClient::postValues()
{
    std::unique_lock<std::mutex> lock(mutex_plants);
    if (plants.empty()) {
        std::cout << "No plants to send data for.\r\n";
        return false;
    }
    httplib::Client cli(_url);
    httplib::Headers headers = {
        { "Content-Type", "application/json" }
    };

    for(auto plant : plants) {
        float humidity;
        if (plant->getHumidity(&humidity)) {
            auto ApiCall = new POST_PLANT_VALUES(plant->getUid());
            json j;
            j[ApiCall->current_humidity] = humidity;
            std::string body = j.dump();
            auto res = cli.Post(ApiCall->endpoint.c_str(), headers, body, "application/json");
            if (res && res->status == 200) {
              std::cout << "DEBUG: Data for plant " << plant->getUid() <<" posted successfully! \r\n";
            } else {
              std::cout << "DEBUG: Failed to post data for plant " << plant->getUid() << " \r\n";
              if (res) {
                std::cout << "DEBUG: Error " << res->status << "\r\n";
              }
              return false;
            }
        }
    }
    return false;
}

void WebClient::postPlantInfo(const httplib::Request &req, httplib::Response &res, POST_PLANT_INFO *post_plant_info)
{
    json data = json::parse(req.body);
    // auto post_plant_info = new POST_PLANT_INFO();
    std::string uid = data[post_plant_info->uid];
    for (auto p : plants) {
        if (p->getUid() == uid) {
            std::cout << "DEBUG: Plant " << p->getName() << ", UID " << p->getUid() << " already exists.\r\n";
            res.status = 400; // Bad request
            res.set_content("Plant already exists", "text/plain");
            return;
        }
    }
    std::string name = data[post_plant_info->name];
    std::string room = data[post_plant_info->room];
    float humidity_threshold = (float) data[post_plant_info->humidity_threshold];
    uint16_t watering_time_seconds = (uint16_t) data[post_plant_info->watering_time_seconds];
    uint8_t sensor_pin_number = (uint8_t) data[post_plant_info->sensor_pin_number];
    uint8_t valve_pin_number = (uint8_t) data[post_plant_info->valve_pin_number];
    DHT22 *sensor = new DHT22(sensor_pin_number);
    Valve *valve = new Valve(ValveType::solenoid, valve_pin_number);
    Plant *p = new Plant(name, uid, sensor, valve);
    plants.push_back(p);
    std::cout << "DEBUG: Plant " << name << ", UID " << uid << " added.\r\n";
    res.status = 200; // OK
    res.set_content("Plant added", "text/plain");
    return;
}

void WebClient::patchPlantInfo(const httplib::Request &req, httplib::Response &res, PATCH_PLANT_INFO *patch_plant_info)
{
    json data = json::parse(req.body);
    std::string uid = req.params.find("uid")->second;
    for (auto p : plants) {
        if (p->getUid() == uid) {
            std::string name = data[patch_plant_info->name];
            std::string room = data[patch_plant_info->room];
            float humidity_threshold = (float) data[patch_plant_info->humidity_threshold];
            uint16_t watering_time_seconds = (uint16_t) data[patch_plant_info->watering_time_seconds];
            uint8_t sensor_pin_number = (uint8_t) data[patch_plant_info->sensor_pin_number];
            uint8_t valve_pin_number = (uint8_t) data[patch_plant_info->valve_pin_number];
            // TODO: improve the Plant, Valve and Sensor classes to handle the PATCH data
            // p->setName(name);
            // p->setRoom(room);
            // p->setHumidityThreshold(humidity_threshold);
            p->setWateringTime(watering_time_seconds);
            // p->setSensorPin(sensor_pin_number);
            // p->setValvePin(valve_pin_number);
            res.status = 200; // OK
            res.set_content("Plant updated", "text/plain");
            return;
        }
    }
    res.status = 400; // Bad request
    res.set_content("Plant not found", "text/plain");
}

void WebClient::deletePlantInfo(const httplib::Request &req, httplib::Response &res, DELETE_PLANT_INFO *delete_plant_info)
{
    std::string uid = req.params.find("uid")->second;
    for (auto it = plants.begin(); it != plants.end(); ++it) {
        if ((*it)->getUid() == uid) {
            plants.erase(it);
            res.status = 200; // OK
            res.set_content("Plant deleted", "text/plain");
            return;
        }
    }
    res.status = 400; // Bad request
    res.set_content("Plant not found", "text/plain");
}

void WebClient::postPlantTriggerWater(const httplib::Request &req, httplib::Response &res, POST_PLANT_TRIGGER_WATER *post_plant_trigger_water)
{
    std::string uid = req.params.find("uid")->second;
    std::string watering_time_seconds = req.params.find(post_plant_trigger_water->watering_time_seconds)->second;
    for (auto p : plants) {
        if (p->getUid() == uid) {
            if (watering_time_seconds.empty()) 
            {
                p->waterPlant();
            }
            else 
            {
                p->waterPlant(std::stoi(watering_time_seconds));
            }
            res.status = 200; // OK
            res.set_content("Watering started", "text/plain");
            return;
        }
    }
}

void WebClient::postStopWater(const httplib::Request &req, httplib::Response &res, POST_STOP_WATER *post_stop_water)
{
    std::string uid = req.params.find("uid")->second;
    for (auto p : plants) {
        if (p->getUid() == uid) {
            // TODO: improve the Plant, Valve and Sensor classes to handle the STOP WATER command
            // p->stopWater();
            res.status = 200; // OK
            res.set_content("Watering stopped", "text/plain");
            return;
        }
    }
}

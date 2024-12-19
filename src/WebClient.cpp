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
#include <cstring>
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
    auto ApiCall = new POST_PLANT_INFO();
    svr.Get(ApiCall->endpoint, std::bind(&WebClient::postPlantInfo, this, std::placeholders::_1, std::placeholders::_2));
    // ApiCall = new PATCH_PLANT_INFO();
    // ApiCall = new DELETE_PLANT_INFO();
    // ApiCall = new POST_PLANT_TRIGGER_WATER();
    // ApiCall = new POST_STOP_WATER();

    svr.listen(_url, _portSrc);
    // Implement the logic to listen on a port for new data
    // Example:
    // Server svr;
    // svr.Get("/newdata", [&](const Request & /*req*/, Response &res) {
    //     // Handle new data
    //     res.set_content("Data received", "text/plain");
    // });
    // svr.listen("localhost", PORT);
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

void WebClient::postPlantInfo(const httplib::Request &req, httplib::Response &res)
{
    // TODO: Implement the logic to retrieve plant info from the server (POST call from the server)
}

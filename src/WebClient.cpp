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
#include <unistd.h>
#include <../cpp-httplib/httplib.h>
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
    std::cout << "Sending plant data to server...\r\n";
    getPlantInfo();
    postWebhookEvents();
    // TODO: redesign to only listen on one port for a server notification and send data to another port
    // Two conditions for the same condition variable would not work
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
        // TODO: parse the body using the GET_PLANT_INFO class
        std::cout << "DEBUG: Plant info retrieved successfully!" << std::endl;
        json j = json::parse(res->body);
        std::unique_lock<std::mutex> lock(mutex_plants);
        for (auto plant : j) {
            DHT22* sensor = new DHT22(plant["pin"]);
            Valve* valve = new Valve(ValveType::solenoid, plant["valvePin"]);
            Plant *p = new Plant(plant["name"], plant["uid"], sensor, valve);
            plants.push_back(p);
            std::cout << "DEBUG: Plant " << plant["name"] << ", UID " << plant["uid"] << " added.\r\n";
        }
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
        { "Content-Type", "application/json" },
        { "Cookie", "Cookie=XDEBUG_SESSION" }
    };
    std::string body;
    for(auto plant : plants) {
        float humidity;
        if (plant->getHumidity(&humidity)) {
            json j;
            j["uid"] = plant->getUid();
            j["humidity"] = humidity;
            body += j.dump();
        }
    }
    auto res = cli.Post(ApiEndpoint::POST_PLANT_VALUES.c_str(), headers, body,
                        "application/json");
    if (res && res->status == 200) {
        std::cout << "DEBUG: Data posted successfully!" << std::endl;
        return true;
    } else {
        std::cout << "DEBUG: Failed to post data \r\n";
        if (res) {
            std::cout << "DEBUG: Error " << res->status << "\r\n";
        }
        return false;
    }
    return false;
}

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
#include "GlobalVars.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

WebClient::WebClient(std::string configPath)
: _configPath(configPath)
{
    _url = "http://localhost:8080";
}

WebClient::WebClient(std::string configPath, const char *url)
    : _url(url), _configPath(configPath) {}

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
    postPlantInfo();
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

bool WebClient::postPlantInfo()
{
    httplib::Client cli(_url);
    httplib::Headers headers = {
        { "Content-Type", "application/json" }
    };
    std::ifstream ifs(_configPath);
    auto config = json::parse(ifs);
    std::string body = config.dump();
    auto res = cli.Post(ApiEndpoint::POST_PLANT_INFO.c_str(), headers, body,
                        "application/json");
    if (res && res->status == 200) {
        std::cout << "DEBUG: Plant info posted successfully!" << std::endl;
        return true;
    } else {
        std::cout << "DEBUG: Failed to post plant info." << std::endl;
        return false;
    }
    return false;
}

bool WebClient::postValues()
{
    if (_plants.empty()) {
        std::cout << "No plants to send data for.\r\n";
        return false;
    }
    httplib::Client cli(_url);
    httplib::Headers headers = {
        { "Content-Type", "application/json" }
    };
    std::string body; //TODO: Implement data to send
    for(auto plant : _plants) {
        float humidity;
        if (plant->getHumidity(&humidity)) {
            json j;
            j["plantName"] = plant->getName();
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
        std::cout << "DEBUG: Failed to post data." << std::endl;
        return false;
    }
    return false;
}

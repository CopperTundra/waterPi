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

#include "Config.h"
#include "Plant.h"
#include "Valve.h"
#include "sensor/DHT22.h"
#include <cstdint>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>
#include <vector>

Config::Config(std::string path) :
_configPath(path)
{

}

Config::~Config()
{

}

bool Config::parseConfig()
{
    std::cout << "Parsing configuration...\r\n";
    try {
        std::ifstream ifs(_configPath);
        auto data = nlohmann::json::parse(ifs);
        _configVersion = data["version"].get<std::string>();
        std::cout << "Config version is " << _configVersion << "\r\n";
        for (auto& plant : data["plants"]) {
            if (plant["waterValve"].get<ValveType>() >= VALVETYPENUMBER) {
                std::cout << "The provided valve type is not defined! Provided: " << plant["waterValve"] << std::endl;
                return false; 
            }
            Valve *val = new Valve(plant["waterValve"].get<ValveType>(),plant["valve_gpioPin"].get<uint8_t>());
            DHT22 *sen = new DHT22(plant["sensor_gpioPin"].get<uint8_t>());
        
            uint16_t wateringTime = plant["wateringTime"].get<uint16_t>();
            Plant *pl;
            if (wateringTime > 0) {
                pl = new Plant(plant["plantName"].get<std::string>(),sen,val,wateringTime);
            }
            else {
                std::cout << "No watering time provided for plant \"" << plant["plantName"].get<std::string>() << "\". Default watering time is 3 seconds.\r\n";
                pl = new Plant(plant["plantName"].get<std::string>(),sen,val);
            }
            _plants.push_back(pl);
            std::cout << "Successfully added plant \"" << plant["plantName"].get<std::string>() << "\" to waterPi\r\n";
        }
    } catch (const std::exception&e) {
        std::cout << "Config.json file is incorrect!\r\n";
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}
void Config::fetchPlantData()
{
    float hum = 0;
    for (auto & plant : _plants) {
        if (plant->getHumidity(&hum)) {
            std::cout << "[Debug] plant " << plant->getName() << " humidity is " << hum << "% \r\n";
        }
    }
}

void Config::addWebServer(WebserverSocket* webserver)
{
    _webserver = webserver;
    communicateWithWebServer();
}

void Config::communicateWithWebServer()
{
    std::cout << "Communicating with web server...\r\n";
    /*TODO: protocol to be defined */
}
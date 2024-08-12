#include "Config.h"
#include "Plant.h"
#include "Valve.h"
#include "sensor/DHT22.h"
#include "sensor/Sensor.h"
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
            Valve *val = new Valve(plant["waterValve"].get<ValveType>(),plant["valve_gpioPin"].get<int>());
            Sensor *sen;
            if (plant["humiditySensor"].get<std::string>() == "DHT22") {
                sen = new DHT22(plant["humiditySensor"].get<SensorType>(),plant["sensor_gpioPin"].get<int>());
            }
            else {
                std::cout << "The provided humidity sensor type is not defined! Provided: " << plant["humiditySensor"].get<std::string>() << std::endl; 
                return false;
            }
            Plant *pl = new Plant(plant["plantName"].get<std::string>(),sen,val);
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
bool Config::fetchPlantData()
{
    for (auto & plant : _plants) {
        if (plant->fetchHumidity()) {
            auto hum = plant->getHumidity();
            std::cout << "[Debug] Humidity is " << hum << "% \r\n";
        }
        else {
            return false;
        }
    }
    return true;
}

void Config::addWebServer(WebserverSocket* webserver)
{
    _webserver = webserver;
    communicateWithWebServer();
}

void Config::communicateWithWebServer()
{
    std::cout << "Communicating with web server...\r\n";
}
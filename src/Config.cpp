#include "Config.h"
#include "Plant.h"
#include "Valve.h"
#include "sensor/DHT22.h"
#include <cstdio>
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>

Config::Config(std::string path) :
configPath(path)
{

}

Config::~Config()
{

}

bool Config::parseConfig()
{
    auto data = nlohmann::json::parse(configPath);
    configVersion = data["version"];
    std::cout << "Parsing configuration..." << std::endl;
    try {
        for (auto& plant : data["plants"]) {
        
        Valve *val = new Valve(data["waterValve"],data["valve_gpioPin"]);
        Sensor *sen;
        if (data["humiditySensor"] == "DHT22") {
            sen = new DHT22(data["humiditySensor"],data["sensor_gpioPin"]);
        }
        else {
            std::cout << "The provided humidity sensor type is not defined! Provided: " << data["humiditySensor"] << std::endl; 
            return false;
        }
        Plant *pl = new Plant(data["plantName"],sen,val);
        plants.push_back(pl);
        std::cout << "Successfully added plant \"" << data["plantName"] << "\" to waterPi" << std::endl; 
        }
    } catch (const std::exception&e) {
        std::cout << "Config.json file is incorrect!" << std::endl;
        std::cout << e.what() << std::endl;
        return false;
    }
    return true;
}
bool Config::fetchPlantData()
{
    for (auto & plant : plants) {
        if (plant->fetchHumidity()) {
            plant->getHumidity();
        }
        else {
            return false;
        }
    }
    return true;
}
#ifndef CONFIG_H
#define CONFIG_H

#include "Plant.h"
#include "WebserverSocket.h"
#include <string>
#include <vector>
#pragma once

class Config
{
public:
    Config(std::string path);
    ~Config();
    bool parseConfig();
    bool fetchPlantData();
    void addWebServer(WebserverSocket* webserver);
    void communicateWithWebServer();

private:
    std::string _configVersion;
    std::string _configPath;
    std::vector<Plant*> _plants;

    WebserverSocket* _webserver;
};

#endif
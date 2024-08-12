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

#ifndef WEBSERVERSOCKET_H
#define WEBSERVERSOCKET_H

#include <sys/un.h>
#include <vector>
#include <string>
#pragma once

struct PlantData
{
    std::string name;
    float humidity;
    bool watered;
};

class WebserverSocket
{
public:
    WebserverSocket(const char* path = "/tmp/webserver.sock");
    ~WebserverSocket();
    bool Connect();
    bool IsConnected();
    bool SendPlantData(std::vector<PlantData> PlantValues);
private:
    const char* _path;
    struct sockaddr_un _address;
    bool _connected = false;
    int _socket_fd;
};

#endif
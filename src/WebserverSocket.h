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
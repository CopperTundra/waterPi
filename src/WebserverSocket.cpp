#include "WebserverSocket.h"
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

WebserverSocket::WebserverSocket(const char* path)
: _path(path)
{
    memset(&_address, 0, sizeof(_address));
    _address.sun_family = AF_UNIX;
    strncpy(_address.sun_path, path, sizeof(_address.sun_path) - 1);
}

WebserverSocket::~WebserverSocket()
{
    // disconnect from socket
    if (_connected) {
        close(_socket_fd);
    }
}

bool WebserverSocket::Connect()
{
    _socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_socket_fd == -1) {
        return false;
    }
    if (connect(_socket_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1) {
        return false;
    }
    _connected = true;
    return true;
}

bool WebserverSocket::IsConnected()
{
    return _connected;
}

bool WebserverSocket::SendPlantData(std::vector<PlantData> PlantValues)
{
    if (!_connected) {
        return false;
    }
    for (auto& plant : PlantValues) {
        std::string data = plant.name + "," + std::to_string(plant.humidity) + "," + std::to_string(plant.watered) + "\n";
        if (send(_socket_fd, data.c_str(), data.length(), 0) == -1) {
            return false;
        }
    }
    return true;
}

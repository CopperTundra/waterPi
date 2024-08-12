#include "Watcher.h"
#include <iostream>

Watcher::Watcher()
{

}

Watcher::~Watcher()
{
    if (watchThread.joinable()) {
        watchThread.join();
    }
}

void Watcher::watch()
{
    std::cout << "Plant monitor thread started!" << std::endl;
}

void Watcher::spawnThread() 
{
    if (watchThread.joinable()) {
        watchThread.join();
    }
    watchThread = std::thread(&Watcher::watch,this);
}
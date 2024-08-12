#ifndef WATCHER_H
#define WATCHER_H

#include <thread>
#pragma once

class Watcher
{
public:
    Watcher();
    ~Watcher();

    void spawnThread();

private:
    std::thread watchThread;
    void watch();
};

#endif
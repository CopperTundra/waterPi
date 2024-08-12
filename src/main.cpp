#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ostream>
#include <string>
#include <wiringPi.h>
#include <iostream>
#include <boost/program_options.hpp>
#include "Config.h"
#include "Watcher.h"
#include "version.h"

bool terminateFlag = false;

struct config_t
{
    std::string jsonFile;
};

void signalHandler(int code)
{
    std::cout << "Signal (" << code << ") handled!" <<  std::endl;
    terminateFlag = true;
}

config_t parseArgs(int argc, char** argv)
{
    try {
        boost::program_options::options_description desc("Allowed options");
        std::string configPath;
        config_t config;
        desc.add_options()
            ("configPath", boost::program_options::value<std::string>(&configPath), "path to the JSON configuration file")
            ("help,h", "print help")
            ("version,v","print version")
        ;
        boost::program_options::variables_map var;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc),var);
        if (var.count("help")) {
            std::cout << desc << std::endl;
            exit(0);
        }
        else if (var.count("version")) {
            std::cout << "WaterPi, version: " << VERSION << std::endl;
            exit(0);
        }
        if (var.count("configPath")) {
            std::ifstream f(configPath);
            if (f.good()) {
                config.jsonFile = configPath;
            }
            else {
                std::cout << "Error, the JSON path specified does not exist." << std::endl;
                exit(-1);
            }
        }
        else {
            std::cout << "Error, no JSON path specified!" << std::endl;
            std::cout << desc << std::endl;
            exit(-1);
        }
        return config;
    } catch (const std::exception &e) {
        std::cout << "Can't parse commandline arguments!" << std::endl;
        std::cout << e.what() << std::endl;
        exit(-1);
    }
}

int main(int argc, char** argv)
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGKILL, signalHandler);
    auto params = parseArgs(argc, argv);
    wiringPiSetupGpio();
    Config *config = new Config(params.jsonFile);
    if (!config->parseConfig()) {
        std::cout << "Error, the provided config.json file is invalid." << std::endl;
        exit(-1);
    }


    Watcher* watcher = new Watcher();
    watcher->spawnThread();

    do {
        try {
            config->fetchPlantData();
            /* push to the webserver the data and get the new limits (if available) */
            /* trigger watering if needed - every 30min, using a thread */
        } catch (const std::exception& e) {
            std::cout << "Unhandled exception: " << e.what() << std::endl;
            terminateFlag = true;
        }

    } while (!terminateFlag);

    if (watcher) {
        delete watcher;
    }
}
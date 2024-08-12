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
#include "WebserverSocket.h"
#include "version.h"

bool terminateFlag = false;

namespace po = boost::program_options;

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
        po::options_description desc("Allowed options");
        config_t config;
        desc.add_options()
            ("configPath,c", po::value<std::string>(), "path to the JSON configuration file")
            ("help,h", "print help")
            ("version,v","print version")
        ;
        po::variables_map var;
        po::store(po::parse_command_line(argc, argv, desc),var);
        if (var.count("help")) {
            std::cout << desc << std::endl;
            exit(0);
        }
        else if (var.count("version")) {
            std::cout << "WaterPi, version: " << VERSION << std::endl;
            exit(0);
        }
        if (var.count("configPath")) {
            std::ifstream f(var["configPath"].as<std::string>());
            if (f) {
                config.jsonFile = var["configPath"].as<std::string>();
            }
            else {
                std::cout << "Error, the JSON path specified does not exist. Provided:" << var["configPath"].as<std::string>() << "\r\n";
                exit(-1);
            }
        }
        else {
            std::cout << "Error, no JSON path specified!\r\n";
            std::cout << desc << std::endl;
            exit(-1);
        }
        return config;
    } catch (const std::exception &e) {
        std::cout << "Can't parse commandline arguments!\r\n";
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
        std::cout << "Error, the provided config.json file is invalid.\r\n";
        exit(-1);
    }

    WebserverSocket *webserver = new WebserverSocket("/tmp/webserver.sock");
    if (webserver->Connect()) {
        std::cout << "Connected to the webserver!\r\n";
    }
    else {
        std::cout << "Error, can't connect to the webserver!\r\n";
        std::cout << "Is the webserver running?\r\n";
        exit(-1);
    }
    config->addWebServer(webserver);

    Watcher* watcher = new Watcher();
    watcher->spawnThread();

    do {
        try {
            config->fetchPlantData();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            /* Give to the local webserver the humidity of each plant and get the new limits parameters */
            config->communicateWithWebServer();
            /* trigger watering if needed - every 30min, using a thread */
        } catch (const std::exception& e) {
            std::cout << "Unhandled exception: " << e.what() << std::endl;
            terminateFlag = true;
        }

    } while (!terminateFlag);

    if (watcher) {
        delete watcher;
        std::cout << "Watcher thread successfully stopped!\r\n";
    }
    if (webserver) {
        delete webserver;
        std::cout << "Webserver connection successfully closed!\r\n";    
    }
}
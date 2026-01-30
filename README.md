# Water Pi

The aim of this project is to automatically irrigate and monitor the humidity of your beloved plants, using a Raspberry Pi.
There are two components within this project: an app that measures your plants' data and cares for them, and a web interface that allows you to check your plants' data and water them manually if needed.

## Table of Contents

- [Water Pi](#water-pi)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
    - [WaterPi Application (C++)](#waterpi-application-c)
    - [Web Interface](#web-interface)
  - [Hardware Requirements](#hardware-requirements)
  - [Software Requirements](#software-requirements)
    - [For the C++ Application:](#for-the-c-application)
    - [For the Web Server:](#for-the-web-server)
  - [Installation](#installation)
    - [C++ Application (Raspberry Pi)](#c-application-raspberry-pi)
    - [Web Server](#web-server)
  - [Configuration](#configuration)
    - [Configuration Parameters:](#configuration-parameters)
  - [Usage](#usage)
  - [API Documentation](#api-documentation)
    - [GET `/plants`](#get-plants)
    - [POST `/plants/[uid]`](#post-plantsuid)
    - [POST `/webhooks`](#post-webhooks)
    - [POST `/plants/[uid]/water`](#post-plantsuidwater)
    - [POST `/plants/[uid]/stop-water`](#post-plantsuidstop-water)
    - [PATCH `/plants/[uid]`](#patch-plantsuid)
    - [DELETE `/plants/[uid]`](#delete-plantsuid)
  - [Building from Source](#building-from-source)
    - [Native Build](#native-build)
    - [Cross-Compilation](#cross-compilation)
    - [Running Tests](#running-tests)
  - [Architecture](#architecture)
    - [WaterPi Application (C++)](#waterpi-application-c-1)
    - [Web Server (Go)](#web-server-go)
    - [Communication](#communication)
  - [License](#license)

## Features

### WaterPi Application (C++)
- Automatic plant watering based on humidity thresholds
- DHT22 humidity sensor integration
- Solenoid valve control for precise water delivery
- Configurable watering schedules (checks every 30 minutes)
- JSON-based plant configuration
- RESTful API communication with web server
- Multi-threaded architecture for sensor monitoring and web communication
- Signal handling for graceful shutdown

### Web Interface
- Real-time display of plant humidity levels
- Manual watering override
- Plant configuration management
- Connection status monitoring
- Watering history tracking
- API endpoints for plant management

## Hardware Requirements

- **Raspberry Pi** (tested on RPi3)
- **DHT22 Humidity Sensors** (one per plant)
- **Solenoid Valves** (one per plant)
- **Power Supply** for valves
- **GPIO Wiring** for sensors and valves
- **Water supply system** (reservoir, tubing, etc.)

## Software Requirements

### For the C++ Application:
- Raspberry Pi OS (or compatible Linux distribution)
- WiringPi library
- Boost library
- CMake 3.5.0 or higher
- C++11 compatible compiler

### For the Web Server:
- Docker and Docker Compose
- Go (if building from source)
- PostgreSQL database

## Installation

### C++ Application (Raspberry Pi)

1. **Install dependencies:**
   ```bash
   sudo apt-get update
   sudo apt-get install cmake build-essential libboost-program-options-dev wiringpi
   ```

2. **Clone the repository:**
   ```bash
   git clone https://github.com/CopperTundra/waterPi.git --recurse-submodules
   cd waterPi
   ```

3. **Build the project:**
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

4. **Deploy to Raspberry Pi:**
   ```bash
   # Copy the executable to your Raspberry Pi
   scp waterPi RPi3:~/bin
   ```

### Web Server

1. **Navigate to the web server directory:**
   ```bash
   cd waterPi-web
   ```

2. **Set up environment:**
   ```bash
   cp .env.dist .env
   # Edit .env with your configuration
   ```

3. **Build and run with Docker:**
   ```bash
   docker compose pull && docker compose build
   docker compose up
   ```

4. **Initialize the database:**
   ```bash
   cat dump_21-09-2024_18_12_18.sql | docker exec -i waterpi-web-db-1 psql -U water_pi
   ```

## Configuration

Create a `config.json` file to configure your plants:

```json
{
    "version": "1.0",
    "plants": [
        {
            "plantName": "Geranium",
            "sensor_gpioPin": 4,
            "waterValve": "solenoid",
            "valve_gpioPin": 12,
            "wateringTime": 5,
            "minHumidity": 30
        },
        {
            "plantName": "Tulipes",
            "sensor_gpioPin": 10,
            "waterValve": "solenoid",
            "valve_gpioPin": 13,
            "wateringTime": 7,
            "minHumidity": 40
        }
    ]
}
```

### Configuration Parameters:
- **plantName**: Name of your plant
- **sensor_gpioPin**: GPIO pin number for the DHT22 sensor
- **waterValve**: Type of valve (currently "solenoid")
- **valve_gpioPin**: GPIO pin number for the water valve
- **wateringTime**: Duration of watering in seconds
- **minHumidity**: Minimum humidity threshold (percentage)

> [!NOTE] 
> This configuration file will be later created by the web server.

## Usage

1. **Start the web server** (if not already running):
   ```bash
   cd waterPi-web
   docker compose up -d
   ```

2. **Run the WaterPi application** on your Raspberry Pi:
   ```bash
   ./waterPi -c /path/to/config.json
   ```

3. **Command line options:**
   ```bash
   ./waterPi --help              # Display help information
   ./waterPi --version           # Display version information
   ./waterPi -c config.json      # Run with specified configuration file
   ```

4. **Access the web interface:**
   - Open your browser and navigate to `http://localhost:1323` (or your configured port)
   - Monitor plant humidity levels
   - Trigger manual watering if needed

## API Documentation

The application communicates with the web server using RESTful API endpoints:

### GET `/plants`
Retrieve information for all plants (called at start and every 30 minutes)

### POST `/plants/[uid]`
Update plant humidity values (every 30 minutes and 2 minutes after watering)

### POST `/webhooks`
Register webhook events (watering_request, new_plant, delete_plant, etc.)

### POST `/plants/[uid]/water`
Manually trigger plant watering

### POST `/plants/[uid]/stop-water`
Emergency stop for manual watering

### PATCH `/plants/[uid]`
Update plant configuration

### DELETE `/plants/[uid]`
Remove a plant from the system

See [featureList.md](featureList.md) for detailed API documentation.

## Building from Source

### Native Build

```bash
mkdir build && cd build
cmake ..
make
```

### Cross-Compilation

For Raspberry Pi 3 (ARMv8):
```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/aarch64-rpi3-linux-gnu.cmake ..
make
```

For Raspberry Pi Zero/1 (ARMv6) (not tested):
```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/armv6-rpi-linux-gnueabihf.cmake ..
make
```

### Running Tests

```bash
cd tests
make test
# Or run the test executable directly
./tests/waterPi_tests
```

## Architecture

The project consists of two main components:

### WaterPi Application (C++)
- **Config**: Manages plant configuration from JSON
- **Plant**: Represents individual plants with sensors and valves
- **DHT22**: Handles humidity sensor readings
- **Valve**: Controls water delivery through solenoid valves
- **Watcher**: Monitors timing for periodic watering checks
- **WebClient**: Manages HTTP communication with the web server
- **Main Loop**: Coordinates sensor readings, watering decisions, and web communication

### Web Server (Go)
- RESTful API for plant management
- PostgreSQL database for data persistence
- Real-time monitoring dashboard
- Webhook support for event notifications

### Communication
- JSON-based data exchange
- HTTP/HTTPS for client-server communication
- Thread-safe plant data access using mutexes

## License
The project is licensed under the GPL License. See the [COPYING](./COPYING) file for more details.

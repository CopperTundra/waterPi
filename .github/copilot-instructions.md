# Copilot Instructions for WaterPi

## Overview
WaterPi is a project designed to automate the irrigation and monitoring of plants using a Raspberry Pi. It consists of two main components: an application that measures plant data and a web interface for monitoring and manual watering.

## Architecture
- **Components**: The project is divided into a backend application (C++) and a web interface (likely using Go based on the presence of `server.go`).
- **Data Flow**: The application collects data from sensors (e.g., DHT22 for humidity) and manages plant states through the `Plant` class. The web interface communicates with the backend to display this data.
- **Service Boundaries**: The backend handles data collection and processing, while the web interface focuses on user interaction.

## Developer Workflows
- **Building the Project**: Use CMake to configure and build the project. The main entry point is defined in `src/main.cpp`.
- **Testing**: Ensure to run tests located in the `tests/` directory. The project uses Google Test for unit testing.
- **Debugging**: Utilize standard debugging tools compatible with C++ and the Raspberry Pi environment.

## Project Conventions
- **File Structure**: Source files are located in the `src/` directory, with headers in the same directory. Configuration files are in `config.json`.
- **Naming Conventions**: Classes are named using CamelCase (e.g., `Plant`, `Valve`), while variables use snake_case.

## Integration Points
- **External Dependencies**: The project relies on libraries such as `cpp-httplib` for HTTP communication and Google Test for testing.
- **Cross-Component Communication**: The backend communicates with the web interface via HTTP requests, likely using RESTful principles.

## Key Files
- **Main Application**: `src/main.cpp` - Entry point for the application.
- **Configuration**: `src/Config.cpp` and `src/Config.h` - Manage application settings.
- **Plant Management**: `src/Plant.cpp` and `src/Plant.h` - Define plant behavior and data handling.
- **Sensor Handling**: `src/sensor/DHT22.cpp` and `src/sensor/DHT22.h` - Interface with the DHT22 sensor for humidity data.

## Conclusion
These instructions should help AI agents understand the structure and workflows of the WaterPi project, enabling them to assist effectively in development tasks.
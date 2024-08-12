cmake_minimum_required(VERSION 3.5.0)
project(waterPi VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Boost REQUIRED COMPONENTS program_options)

set(SOURCES
    src/sensor/Sensor.cpp
    src/sensor/DHT22.cpp
    src/Config.cpp
    src/Plant.cpp
    src/Valve.cpp
    src/Watcher.cpp
    src/main.cpp
)

add_executable(waterPi 
    ${SOURCES}
)
include_directories(src)

find_library(WIRINGPI_LIBRARIES wiringPi)
find_library(WIRINGPIDEV_LIB wiringPiDev)
target_link_libraries(waterPi PRIVATE ${WIRINGPI_LIBRARIES})
target_link_libraries(waterPi PRIVATE ${WIRINGPIDEV_LIB})
# target_link_libraries(waterPi PRIVATE thread)
target_link_libraries(waterPi PRIVATE Boost::program_options)
target_link_libraries(waterPi PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(waterPi PRIVATE "/usr/local/include")
target_link_directories(waterPi PRIVATE "/usr/local/lib")

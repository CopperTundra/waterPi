# GNU GENERAL PUBLIC LICENSE
# Version 3, 29 June 2007

# Copyright (C) Olivier Schüwer

# WaterPi is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# WaterPi is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with WaterPi.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.5.0)
project(WaterPi)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/latest/download/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Boost REQUIRED COMPONENTS program_options)

set(SOURCES
    src/sensor/DHT22.cpp
    src/Config.cpp
    src/Plant.cpp
    src/Valve.cpp
    src/Watcher.cpp
    src/WebClient.cpp
    src/GlobalVars.cpp
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
target_link_libraries(waterPi PRIVATE Boost::program_options)
target_link_libraries(waterPi PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(waterPi PRIVATE curl)
target_link_libraries(waterPi PRIVATE uuid)
add_subdirectory(cpp-httplib)
target_link_libraries(waterPi PRIVATE httplib)
target_include_directories(waterPi PRIVATE "/usr/local/include")
target_link_directories(waterPi PRIVATE "/usr/local/lib")

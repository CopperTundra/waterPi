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

cmake_minimum_required(VERSION 3.16)
project(waterpi_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
enable_testing()

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/latest/download/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(GTest REQUIRED)

# Add test executable
set(TEST_SOURCES
    WebClientTest.cpp
    MockHttpClient.h
    ../src/WebClient.cpp
    ../src/Config.cpp
    ../src/Plant.cpp
    ../src/Valve.cpp
    ../src/Watcher.cpp
    ../src/GlobalVars.cpp
    ../src/sensor/DHT22.cpp
)

add_executable(waterPi_tests
    ${TEST_SOURCES}
)

target_compile_definitions(waterPi_tests PRIVATE UNIT_TEST)
target_include_directories(waterPi_tests PRIVATE "/usr/local/include")
target_link_directories(waterPi_tests PRIVATE "/usr/local/lib")
target_link_libraries(waterPi_tests PRIVATE Boost::program_options)
target_link_libraries(waterPi_tests PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(waterPi_tests PRIVATE curl)
target_link_libraries(waterPi_tests PRIVATE uuid)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
)
FetchContent_MakeAvailable(httplib)
target_link_libraries(waterPi_tests PRIVATE httplib)
target_link_libraries(waterPi_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(waterPi_tests)
target_compile_definitions(waterPi_tests PRIVATE CMAKE_EXPORT_COMPILE_COMMANDS=1)

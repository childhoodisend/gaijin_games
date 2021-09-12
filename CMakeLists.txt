cmake_minimum_required(VERSION 3.1)
project(gaijin_games)

find_package (bson-1.0 REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfloat-conversion -pedantic -pthread")

include_directories(".")

add_subdirectory(clsocket)
add_subdirectory(message)
add_subdirectory(server)
add_subdirectory(client)

add_executable(gaijin_games main.cpp)

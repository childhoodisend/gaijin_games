cmake_minimum_required(VERSION 3.1)
project(gaijin_games)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfloat-conversion -pedantic -pthread")

add_subdirectory(server)
add_subdirectory(client)

add_executable(gaijin_games main.cpp)

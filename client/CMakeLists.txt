cmake_minimum_required(VERSION 3.1)
project(client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfloat-conversion -pedantic -pthread")

add_executable(client client.cpp client.h)
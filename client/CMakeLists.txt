cmake_minimum_required(VERSION 3.1)
project(client)

find_package (bson-1.0 REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfloat-conversion -Wshadow -Werror -pedantic -pthread -O3")

if(UNIX)
    add_definitions(-D_LINUX)
endif()

if(APPLE)
    remove_definitions(-D_LINUX)
    add_definitions(-D_DARWIN)
endif()

if(WIN32)
    add_definitions(-D_WIN32)
endif()


add_executable(client client.cpp scenarios/scenarios.h)
target_link_libraries(client PRIVATE clsocket PUBLIC message mongo::bson_shared)
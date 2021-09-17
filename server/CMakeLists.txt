cmake_minimum_required(VERSION 3.1)
project(server)
find_package (bson-1.0 REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfloat-conversion -Wshadow -Werror -pedantic -pthread -O3")

add_executable(server
        server.cpp server.h
        data_writer.cpp data_writer.h)

target_link_libraries(server PRIVATE mongo::bson_shared PUBLIC clsocket message)

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
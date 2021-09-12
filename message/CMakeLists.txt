cmake_minimum_required(VERSION 3.1)
project(message)

find_package (bson-1.0 REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Werror -pedantic -O3")

add_library(message STATIC
        message.cpp message.h)

target_link_libraries(message PRIVATE mongo::bson_shared)

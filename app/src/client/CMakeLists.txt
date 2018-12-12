cmake_minimum_required(VERSION 3.12)
project(client)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_CLIENT main.cc client.cc client.h ../connection.cc ../connection.h
    ../user.h)

include_directories(../)

add_executable(client ${SOURCE_CLIENT})

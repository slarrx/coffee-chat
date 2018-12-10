cmake_minimum_required(VERSION 3.12)
project(server)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_SERVER main.cc server.cc server.h ../connection.cc ../connection.h
    ../handler.cc ../handler.h ../user.cc ../user.h)

include_directories(../)

add_executable(server ${SOURCE_SERVER})

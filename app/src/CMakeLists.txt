cmake_minimum_required(VERSION 3.12)
project(coffee_chat)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS -pthread)

add_subdirectory(server)
add_subdirectory(client)

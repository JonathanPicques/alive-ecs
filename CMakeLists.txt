cmake_minimum_required(VERSION 3.9)
project(alive_ecs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -fno-rtti)

add_executable(alive_ecs main.cpp Entity.cpp Entity.hpp)
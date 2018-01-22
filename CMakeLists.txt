cmake_minimum_required(VERSION 3.9)
project(alive_ecs)

set(CMAKE_CXX_STANDARD 14)

add_executable(alive_ecs main.cpp src/core/entity.cpp include/core/entity.hpp src/core/component.cpp include/core/component.hpp src/core/entitymanager.cpp include/core/entitymanager.hpp)
target_include_directories(alive_ecs PUBLIC include)
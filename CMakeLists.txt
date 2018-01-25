cmake_minimum_required(VERSION 3.9)
project(alive_ecs)

set(CMAKE_CXX_STANDARD 14)

# ECS library
add_library(alive_ecs
        src/core/entity.cpp
        src/core/component.cpp
        src/core/entitymanager.cpp)

target_include_directories(alive_ecs
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# ECS tests
enable_testing()
add_executable(tests
        tests/entities_test.cpp
        tests/components_test.cpp)
add_subdirectory(tests/googletest-release-1.8.0)
target_link_libraries(tests alive_ecs gtest_main)
target_include_directories(tests PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(tests PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/1.8.0/googletest/include>)
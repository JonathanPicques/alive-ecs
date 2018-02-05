cmake_minimum_required(VERSION 3.9)
project(alive_ecs)

set(CMAKE_CXX_STANDARD 14)

# ECS library
add_library(alive_ecs
        src/core/entitymanager.cpp
        include/core/entitymanager.hpp
        src/core/system.cpp
        include/core/system.hpp
        src/core/entity.cpp
        include/core/entity.hpp
        src/core/component.cpp
        include/core/component.hpp tests/test_systems/systems.cpp tests/test_systems/systems.hpp)
target_include_directories(alive_ecs
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)
target_compile_options(alive_ecs PRIVATE "-Wall")
target_compile_options(alive_ecs PRIVATE "-ansi")
target_compile_options(alive_ecs PRIVATE "-pedantic")
target_compile_definitions(alive_ecs PUBLIC _DEBUG=1)

# ECS tests
enable_testing()
add_executable(alive_tests
        tests/test_systems/systems.cpp
        tests/test_systems/systems.hpp
        tests/test_components/components.cpp
        tests/test_components/components.hpp
        tests/test_systems.cpp
        tests/test_entities.cpp
        tests/test_lifecycle.cpp
        tests/test_entitymanager.cpp)
add_subdirectory(tests/googletest)
target_link_libraries(alive_tests alive_ecs gtest_main)
target_include_directories(alive_tests PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(alive_tests PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/1.8.0/googletest/include>)
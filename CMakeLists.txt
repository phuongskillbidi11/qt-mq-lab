cmake_minimum_required(VERSION 3.21)

project(qtmqlab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

if(WIN32)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

add_subdirectory(external/base)
add_subdirectory(external/AMQP-CPP)

find_package(Qt5 5.15 REQUIRED COMPONENTS Core Network Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_feature_module(mq UI LINK amqpcpp)

add_executable(qtmqlab WIN32 app/main.cpp)
target_link_libraries(qtmqlab PRIVATE feature_mq)

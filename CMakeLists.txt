cmake_minimum_required(VERSION 3.16)
project(lit LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "release")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build ${CMAKE_MODULE_PATH})

add_library(
    init 
    lit-init/init.hpp
    lit-init/init.cpp)
target_include_directories(init PUBLIC lit-init)

add_library(
    ihelp 
    lit-help/help.cpp)
target_include_directories(ihelp PUBLIC lit-help)

add_library(
    utils 
    lit-utils/utils.hpp
    lit-utils/utils.cpp
    lit-utils/utils2.cpp)
target_include_directories(utils PUBLIC lit-utils)

add_library(
    log 
    lit-log/log.hpp
    lit-log/log.cpp)
target_include_directories(log PUBLIC lit-log)

add_library(
    status 
    lit-status/status.hpp
    lit-status/status.cpp)
target_include_directories(status PUBLIC lit-status)

add_executable(lit main.cpp)
target_link_libraries(lit PRIVATE utils init log status ihelp)


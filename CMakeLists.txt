cmake_minimum_required (VERSION 3.8)

project ("Taskable")

message ("${CMAKE_CONFIGURATION_TYPES}")

set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
message(STATUS "CMAKE_MODULE_PATH:${CMAKE_MODULE_PATH}")

message ("${CMAKE_CONFIGURATION_TYPES}")

add_subdirectory("src")

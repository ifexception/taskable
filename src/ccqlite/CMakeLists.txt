cmake_minimum_required (VERSION 3.8)
project (ccqlite
    VERSION 0.0.1
    DESCRIPTION "A simple sqlite wrapper in modern C++")

find_package (sqlite3 REQUIRED)
message (STATUS "sqlite3 found: ${sqlite3_FOUND}")

set (HEADER "include/database.hh"
    "include/permission.hh"
    "include/database_exception.hh")

set (SRC "src/database.cc"
    "src/permission.cc"
    "src/database_exception.cc")

set(CMAKE_DEBUG_POSTFIX d)

add_library (${PROJECT_NAME} SHARED
    ${SRC} ${HEADER})

message (STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions (${PROJECT_NAME} PUBLIC CCQLITE_EXPORTS)

target_include_directories (${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

set_target_properties (${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)

target_compile_features (${PROJECT_NAME} PRIVATE
    cxx_std_17)

target_link_libraries (${PROJECT_NAME} sqlite3)

install (TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

install (FILES ${HEADER}
    DESTINATION include)

install (EXPORT ${PROJECT_NAME}
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/${PROJECT_NAME})

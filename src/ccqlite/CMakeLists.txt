cmake_minimum_required (VERSION 3.8)
project (ccqlite
    VERSION 0.1.0
    DESCRIPTION "A simple sqlite wrapper in modern C++")

find_package (sqlite3 REQUIRED)
find_package (spdlog CONFIG REQUIRED)

message (STATUS "sqlite3 found: ${sqlite3_FOUND}")
message (STATUS "spdlog found: ${spdlog_FOUND}")

set (HEADER "include/constants.hh"
    "include/database.hh"
    "include/permission.hh"
    "include/database_exception.hh"
    "include/backup.hh"
    "include/column_type.hh"
    "include/column.hh"
    "include/copy_options.hh"
    "include/statement.hh"
    "include/command.hh"
    "include/query.hh")

set (SRC "src/constants.cc"
    "src/database.cc"
    "src/permission.cc"
    "src/database_exception.cc"
    "src/backup.cc"
    "src/column_type.cc"
    "src/column.cc"
    "src/copy_options.cc"
    "src/statement.cc"
    "src/command.cc"
    "src/query.cc")

set(CMAKE_DEBUG_POSTFIX d)

add_library (${PROJECT_NAME} SHARED
    ${SRC} ${HEADER})

message (STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions (${PROJECT_NAME} PUBLIC
    CCQLITE_EXPORTS)

target_compile_options (${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /permissive- /TP /MD>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>)

target_include_directories (${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

set_target_properties (${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)

target_compile_features (${PROJECT_NAME} PRIVATE
    cxx_std_17)

target_link_libraries (${PROJECT_NAME} PUBLIC
    sqlite3
    spdlog::spdlog)

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

cmake_minimum_required (VERSION 3.8)
project(wxTimesheetsTracker)

set(default_build_type ${CMAKE_CONFIGURATION_TYPES})
if(NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}")
endif()

#if(MSVC)
#    include(${CMAKE_SOURCE_DIR}/cmake/FindwxWidgetsVcpkg.cmake)
#else(MSVC)
#    find_package(wxWidgets REQUIRED COMPONENTS core base)
#    include(${wxWidgets_USE_FILE})
#endif()

message(STATUS "CMAKE_CONFIGURATION_TYPES:${CMAKE_CONFIGURATION_TYPES}")

find_package(ccqlite REQUIRED)
message (STATUS "ccqlite_FOUND: ${ccqlite_FOUND}")

add_executable (${PROJECT_NAME} "src/app.cc")

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_17)

target_link_libraries (${PROJECT_NAME} ccqlite)

#target_compile_definitions(${PROJECT_NAME} PUBLIC
#                           _CRT_SECURE_NO_WARNINGS
#                           _UNICODE
#                           UNICODE
#                           WXUSINGDLL
#                           wxUSE_GUI=1
#                           __WXMSW__
#                           $<$<CONFIG:Debug>:_DEBUG>
#                          )
#
#target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

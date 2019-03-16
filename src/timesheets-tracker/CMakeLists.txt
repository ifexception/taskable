cmake_minimum_required (VERSION 3.8)
project(wxTimesheetsTracker)

if(MSVC)
    include (${CMAKE_MODULE_PATH}/FindwxWidgetsVcpkg.cmake)
else(MSVC)
    find_package (wxWidgets REQUIRED COMPONENTS core base)
    include (${wxWidgets_USE_FILE})
endif()

message(STATUS "CMAKE_CONFIGURATION_TYPES:${CMAKE_CONFIGURATION_TYPES}")

find_package (ccqlite REQUIRED)
message (STATUS "ccqlite_FOUND: ${ccqlite_FOUND}")

message (STATUS "wxWidgets_FOUND: ${wxWidgets_FOUND}")

set (SRC
    "src/app.cc"
    "src/frame/main_frame.cc"
    "src/dialogs/about_dialog.cc"
    "src/dialogs/new_task_dialog.cc")

add_executable (${PROJECT_NAME} WIN32 ${SRC})

target_compile_options (${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /permissive- /TP>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>)

target_compile_features (${PROJECT_NAME} PRIVATE
    cxx_std_17)

target_compile_definitions (${PROJECT_NAME} PUBLIC
    _CRT_SECURE_NO_WARNINGS
    _UNICODE
    UNICODE
    WXUSINGDLL
    wxUSE_GUI=1
    __WXMSW__
    $<$<CONFIG:Debug>:_DEBUG>)

target_link_libraries(${PROJECT_NAME} ccqlite ${wxWidgets_LIBRARIES})

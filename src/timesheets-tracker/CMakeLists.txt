cmake_minimum_required (VERSION 3.8)
project (wxTimesheetsTracker)

if (MSVC)
    include (${CMAKE_MODULE_PATH}/FindwxWidgetsVcpkg.cmake)
else (MSVC)
    find_package (wxWidgets REQUIRED COMPONENTS base core)
    include (${wxWidgets_USE_FILE})
endif ()
include (${CMAKE_MODULE_PATH}/FindRapidXml.cmake)

message (STATUS "CMAKE_CONFIGURATION_TYPES:${CMAKE_CONFIGURATION_TYPES}")

find_package (rapidxml REQUIRED)
find_package (sqlite3 REQUIRED)
find_package (spdlog CONFIG REQUIRED)

message (STATUS "sqlite3 found: ${sqlite3_FOUND}")
message (STATUS "spdlog found: ${spdlog_FOUND}")
message (STATUS "rapidxml_FOUND: ${rapidxml_FOUND}")
message (STATUS "wxWidgets_FOUND: ${wxWidgets_FOUND}")

set (SRC
    "src/common/ids.cc"
    "src/common/common.cc"
    "src/common/util.cc"
    "src/common/config.cc"
    "src/common/constants.cc"
    "src/db/database.cc"
    "src/db/permission.cc"
    "src/db/database_exception.cc"
    "src/db/backup.cc"
    "src/db/column_type.cc"
    "src/db/column.cc"
    "src/db/copy_options.cc"
    "src/db/command.cc"
    "src/db/statement.cc"
    "src/db/query.cc"
    "src/services/models/employer.cc"
    "src/services/models/client.cc"
    "src/services/db_connection.cc"
    "src/services/employer_service.cc"
    "src/services/client_service.cc"
    "src/app.cc"
    "src/frame/main_frame.cc"
    "src/dialogs/about_dialog.cc"
    "src/dialogs/task_details_dialog.cc"
    "src/dialogs/employer_dialog.cc"
    "src/dialogs/client_dialog.cc"
    "src/dialogs/project_dialog.cc")

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
    wxUSE_TIMEPICKCTRL=1
    __WXMSW__
    $<$<CONFIG:Debug>:_DEBUG>)

target_include_directories(${PROJECT_NAME} PUBLIC
    rapidxml)

target_link_libraries(${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
    sqlite3
    spdlog::spdlog)

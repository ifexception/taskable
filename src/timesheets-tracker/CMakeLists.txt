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
    "src/common/ids.cpp"
    "src/common/common.cpp"
    "src/common/util.cpp"
    "src/common/config.cpp"
    "src/common/constants.cpp"
    "src/db/database.cpp"
    "src/db/permission.cpp"
    "src/db/database_exception.cpp"
    "src/db/backup.cpp"
    "src/db/column_type.cpp"
    "src/db/column.cpp"
    "src/db/copy_options.cpp"
    "src/db/command.cpp"
    "src/db/statement.cpp"
    "src/db/query.cpp"
    "src/services/models/employer.cpp"
    "src/services/models/client.cpp"
    "src/services/models/project.cpp"
    "src/services/models/category.cpp"
    "src/services/models/detailed_task.cpp"
    "src/services/db_connection.cpp"
    "src/services/db_service.cpp"
    "src/app.cpp"
    "src/frame/main_frame.cpp"
    "src/dialogs/about_dialog.cpp"
    "src/dialogs/task_details_dialog.cpp"
    "src/dialogs/employer_dialog.cpp"
    "src/dialogs/client_dialog.cpp"
    "src/dialogs/project_dialog.cpp"
    "src/dialogs/category_dialog.cpp")

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

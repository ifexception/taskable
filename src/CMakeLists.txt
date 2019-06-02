cmake_minimum_required (VERSION 3.8)
project (wxTasksTracker)

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
    "common/ids.cpp"
    "common/common.cpp"
    "common/util.cpp"
    "common/config.cpp"
    "common/constants.cpp"
    "db/database.cpp"
    "db/permission.cpp"
    "db/database_exception.cpp"
    "db/backup.cpp"
    "db/column_type.cpp"
    "db/column.cpp"
    "db/copy_options.cpp"
    "db/command.cpp"
    "db/statement.cpp"
    "db/query.cpp"
    "services/models/employer.cpp"
    "services/models/client.cpp"
    "services/models/project.cpp"
    "services/models/category.cpp"
    "services/models/detailed_task.cpp"
    "services/models/task_detail.cpp"
    "services/db_connection.cpp"
    "services/db_service.cpp"
    "app.cpp"
    "frame/mainframe.cpp"
    "frame/taskbaricon.cpp"
    "dialogs/aboutdialog.cpp"
    "dialogs/taskdetailsdialog.cpp"
    "dialogs/employerdialog.cpp"
    "dialogs/clientdialog.cpp"
    "dialogs/projectdialog.cpp"
    "dialogs/categorydialog.cpp"
    "dialogs/editlistdialog.cpp"
    "wizards/setupwizard.cpp")

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

target_include_directories (${PROJECT_NAME} PUBLIC
    rapidxml)

target_link_libraries (${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
    sqlite3
    spdlog::spdlog)

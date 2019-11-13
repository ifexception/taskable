cmake_minimum_required (VERSION 3.8)
project ("Taskable")

if (MSVC)
    include (${CMAKE_MODULE_PATH}/FindwxWidgetsVcpkg.cmake)
else (MSVC)
    find_package (wxWidgets REQUIRED COMPONENTS base core)
    include (${wxWidgets_USE_FILE})
endif ()

message (STATUS "CMAKE_CONFIGURATION_TYPES:${CMAKE_CONFIGURATION_TYPES}")

find_package(ZLIB REQUIRED)
find_package (sqlite3 CONFIG REQUIRED)
find_package (spdlog CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

message (STATUS "ZLIB found: ${ZLIB_FOUND}")
message (STATUS "sqlite3 found: ${sqlite3_FOUND}")
message (STATUS "spdlog found: ${spdlog_FOUND}")
message (STATUS "wxWidgets_FOUND: ${wxWidgets_FOUND}")
message (STATUS "cpr_FOUND: ${cpr_FOUND}")
message (STATUS "nlohmann_json_FOUND: ${nlohmann_json_FOUND}")

set (SRC
    "common/ids.cpp"
    "common/common.cpp"
    "common/util.cpp"
    "config/configuration.cpp"
    "common/constants.cpp"
    "services/models/employer.cpp"
    "services/models/client.cpp"
    "services/models/project.cpp"
    "services/models/category.cpp"
    "services/models/task_item.cpp"
    "services/db_connection.cpp"
    "services/db_service.cpp"
    "services/taskstateservice.cpp"
    "services/taskstorageservice.cpp"
    "app.cpp"
    "resources.rc"
    "app.manifest"
    "frame/mainframe.cpp"
    "frame/taskbaricon.cpp"
    "dialogs/taskitemdlg.cpp"
    "dialogs/clientdlg.cpp"
    "dialogs/editlistdlg.cpp"
    "dialogs/settingsdialog.cpp"
    "dialogs/stopwatchtaskdlg.cpp"
    "dialogs/checkforupdatedlg.cpp"
    "wizards/setupwizard.cpp"

    "dialogs/employerdlg.cpp"
    "models/employermodel.cpp"

    "dialogs/categorydlg.cpp"
    "dialogs/categoriesdlg.cpp"
    "models/categorymodel.cpp"

    "dialogs/projectdlg.cpp"
    "models/projectmodel.cpp"
    )

add_executable (${PROJECT_NAME} WIN32 ${SRC})

target_compile_options (${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /permissive- /TP /EHsc>
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

target_link_libraries (${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
    ZLIB::ZLIB
    sqlite3
    spdlog::spdlog
    cpr
    nlohmann_json nlohmann_json::nlohmann_json)

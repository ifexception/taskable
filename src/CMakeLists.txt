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
find_package(unofficial-sqlite3 CONFIG REQUIRED)
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
    "common/datetraverser.cpp"
    "common/constants.cpp"
    "config/configuration.cpp"

    "database/connection.cpp"
    "database/connectionfactory.cpp"
    "database/connectionpool.cpp"
    "database/sqliteconnection.cpp"
    "database/sqliteconnectionfactory.cpp"
    "database/connectionprovider.cpp"

    "services/outlookintegrator.cpp"
    "services/taskstateservice.cpp"
    "services/taskstorageservice.cpp"
    "services/databasebackup.cpp"
    "services/databasebackupdeleter.cpp"
    "services/setupdatabase.cpp"

    "application.cpp"
    "resources.rc"
    "application.manifest"

    "frame/mainframe.cpp"
    "frame/taskbaricon.cpp"
    "frame/feedbackpopup.cpp"

    "dataview/weeklymodel.cpp"
    "dialogs/weeklytaskviewdlg.cpp"

    "dialogs/editlistdlg.cpp"
    "dialogs/stopwatchtaskdlg.cpp"
    "dialogs/checkforupdatedlg.cpp"

    "dialogs/preferencesgeneralpage.cpp"
    "dialogs/preferencesdatabasepage.cpp"
    "dialogs/preferencesstopwatchpage.cpp"
    "dialogs/preferencestaskitempage.cpp"
    "dialogs/preferencesdlg.cpp"

    "wizards/setupwizard.cpp"
    "wizards/entitycompositor.cpp"
    "wizards/databaserestorewizard.cpp"

    "dialogs/employerdlg.cpp"
    "models/employermodel.cpp"
    "data/employerdata.cpp"

    "dialogs/clientdlg.cpp"
    "models/clientmodel.cpp"
    "data/clientdata.cpp"

    "data/ratetypedata.cpp"
    "models/ratetypemodel.cpp"

    "data/currencydata.cpp"
    "models/currencymodel.cpp"

    "dialogs/projectdlg.cpp"
    "models/projectmodel.cpp"
    "data/projectdata.cpp"

    "dialogs/categorydlg.cpp"
    "dialogs/categoriesdlg.cpp"
    "models/categorymodel.cpp"
    "data/categorydata.cpp"

    "models/taskmodel.cpp"
    "models/taskitemtypemodel.cpp"
    "models/taskitemmodel.cpp"
    "dialogs/taskitemdlg.cpp"
    "data/taskdata.cpp"
    "data/taskitemtypedata.cpp"
    "data/taskitemdata.cpp"

    "data/meetingdata.cpp"
    "models/meetingmodel.cpp"
    "dialogs/meetingsviewdlg.cpp"
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
    $<$<CONFIG:Debug>:TASKABLE_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Debug>:WXDEBUG>)

target_link_libraries (${PROJECT_NAME}
    ${wxWidgets_LIBRARIES}
    ZLIB::ZLIB
    unofficial::sqlite3::sqlite3
    spdlog::spdlog
    cpr
    nlohmann_json nlohmann_json::nlohmann_json)

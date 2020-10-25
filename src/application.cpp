// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "application.h"

#include <algorithm>

#include <wx/file.h>
#include <wx/stdpaths.h>
#include <wx/msw/registry.h>

#include "common/common.h"
#include "common/constants.h"
#include "config/configurationprovider.h"
#include "database/sqliteconnectionfactory.h"
#include "database/sqliteconnection.h"
#include "database/connectionprovider.h"
#include "frame/mainframe.h"
#include "services/setupdatabase.h"
#include "services/databasebackup.h"
#include "services/databasestructureupdater.h"
#include "wizards/setupwizard.h"
#include "wizards/databaserestorewizard.h"

namespace app
{
Application::Application()
    : pInstanceChecker(std::make_unique<wxSingleInstanceChecker>())
{
}

bool Application::OnInit()
{
#ifndef TASKABLE_DEBUG
    bool isInstanceAlreadyRunning = pInstanceChecker->IsAnotherRunning();
    if (isInstanceAlreadyRunning) {
        wxMessageBox(wxT("Another instance of the application is already running."),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_WARNING);
        return false;
    }
#endif // TASKABLE_DEBUG

    if (!InitializeLogging()) {
        return false;
    }

    if (!ConfigurationFileExists()) {
        return false;
    }

    if (IsSetup()) {
        if (!StartupInitialization()) {
            return false;
        }
    } else {
        if (!FirstStartupInitialization()) {
            return false;
        }
    }

    auto frame = new frm::MainFrame(pLogger);
    frame->CreateFrame();
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}

bool Application::FirstStartupInitialization()
{
    if (!CreateDatabaseFile()) {
        return false;
    }

    InitializeDatabaseConnectionProvider();

    if (!InitializeDatabaseTables()) {
        return false;
    }

    if (!RunSetupWizard()) {
        db::ConnectionProvider::Get().PurgeConnectionPool();
        DeleteDatabaseFile();
        return false;
    }

    if (!ConfigureRegistry()) {
        return false;
    }

    return true;
}

bool Application::StartupInitialization()
{
    if (!DatabaseFileExists()) {
        return false;
    } else {
        InitializeDatabaseConnectionProvider();
    }

    if (CheckForDatabaseUpgrade()) {
        svc::DatabaseStructureUpdater dbStructureUpdater(pLogger);

        if (!dbStructureUpdater.ExecuteScripts()) {
            wxString errorMessage =
                wxString::Format(wxT("%s encountered an error while executing a database update operation.\n"
                                     "The operation was aborted."),
                    common::GetProgramName());
            wxMessageBox(errorMessage, common::GetProgramName(), wxICON_ERROR | wxOK_DEFAULT);
            return false;
        }

        if (!CompleteDatabaseUpgrade()) {
            return false;
        }
    }

    return true;
}

bool Application::InitializeLogging()
{
    const std::string LoggerName = "Taskable_Daily";
    const char* LogsFilename = "Taskable.log.txt";

    if (!CreateLogsDirectory()) {
        return false;
    }

    auto logDirectory =
        wxString::Format(wxT("%s\\logs\\%s"), wxStandardPaths::Get().GetUserDataDir(), LogsFilename).ToStdString();

    try {
        auto msvcSink = std::make_shared<spdlog::sinks::msvc_sink_st>();

        auto msvcLogger = std::make_shared<spdlog::logger>("msvc", msvcSink);
        msvcLogger->set_level(spdlog::level::debug);
        spdlog::register_logger(msvcLogger);

        auto dialySink = std::make_shared<spdlog::sinks::daily_file_sink_st>(logDirectory, 23, 59);
        dialySink->set_level(spdlog::level::err);

        auto combinedLoggers = std::make_shared<spdlog::sinks::dist_sink_st>();
        combinedLoggers->add_sink(msvcSink);
        combinedLoggers->add_sink(dialySink);
        pLogger = std::make_shared<spdlog::logger>(LoggerName, combinedLoggers);
    } catch (const spdlog::spdlog_ex& e) {
        wxMessageBox(wxString::Format(wxT("Error initializing logger: %s"), e.what()),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_EXCLAMATION);
        return false;
    }

    pLogger->flush_on(spdlog::level::err);
    pLogger->enable_backtrace(32);

    return true;
}

bool Application::CreateLogsDirectory()
{
    wxString logs = wxString::Format(wxT("%s\\logs"), wxStandardPaths::Get().GetUserDataDir());
    bool logDirectoryExists = wxDirExists(logs);
    if (!logDirectoryExists) {
        bool success = wxMkDir(logs);
        return !success;
    }

    return logDirectoryExists;
}

bool Application::InitializeDatabaseConnectionProvider()
{
    static int ConnectionPoolSize = 14;

    auto sqliteConnectionFactory = std::make_shared<db::SqliteConnectionFactory>(
        common::GetDatabaseFilePath(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath()).ToStdString());
    auto connectionPool =
        std::make_unique<db::ConnectionPool<db::SqliteConnection>>(sqliteConnectionFactory, ConnectionPoolSize);
    db::ConnectionProvider::Get().InitializeConnectionPool(std::move(connectionPool));

    return true;
}

bool Application::IsSetup()
{
#ifdef TASKABLE_DEBUG
    wxRegKey key(wxRegKey::HKCU, "Software\\Taskabled");
#else
    wxRegKey key(wxRegKey::HKCU, "Software\\Taskable");
#endif // TASKABLE_DEBUG

    if (key.HasValue(wxT("IsSetup"))) {
        long value = 0;
        key.QueryValue(wxT("IsSetup"), &value);

        return !!value;
    }
    return false;
}

bool Application::RunSetupWizard()
{
    auto wizard = new wizard::SetupWizard(nullptr, pLogger);
    wizard->CenterOnScreen();
    bool result = wizard->Run();

    return result;
}

bool Application::ConfigureRegistry()
{
#ifdef TASKABLE_DEBUG
    wxRegKey key(wxRegKey::HKCU, "Software\\Taskabled");
#else
    wxRegKey key(wxRegKey::HKCU, "Software\\Taskable");
#endif // TASKABLE_DEBUG

    bool result = key.Exists();
    if (!result) {
        pLogger->critical("Unable to locate registry: \"Taskable\"");
        return result;
    }

    result = key.SetValue("IsSetup", true);
    if (!result) {
        pLogger->critical("Unable to set registry value: \"IsSetup\"");
    }
    return result;
}

bool Application::ConfigurationFileExists()
{
    bool configFileExists = wxFileExists(common::GetConfigFilePath());
    if (!configFileExists) {
        wxMessageBox(wxT("Error: Program cannot locate configuration file!"),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_ERROR);
        pLogger->error("Unable to locate configuration file at specified location.");
        return false;
    }

    cfg::ConfigurationProvider::Get().Initialize();

    if (cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath().empty()) {
        cfg::ConfigurationProvider::Get().Configuration->SetDatabasePath(wxStandardPaths::Get().GetAppDocumentsDir());
        cfg::ConfigurationProvider::Get().Configuration->Save();
    }

    return configFileExists;
}

bool Application::CreateBackupsDirectory()
{
    wxString backupsDirectory;
    if (IsSetup() && !cfg::ConfigurationProvider::Get().Configuration->GetBackupPath().empty()) {
        backupsDirectory = cfg::ConfigurationProvider::Get().Configuration->GetBackupPath();
    } else {
        backupsDirectory = wxString::Format(wxT("%s\\backups"), wxStandardPaths::Get().GetUserDataDir());
    }

    if (!wxDirExists(backupsDirectory)) {
        bool success = wxMkDir(backupsDirectory);
        return !success;
    }

    return true;
}

bool Application::CreateDatabaseFile()
{
    if (!wxDirExists(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath())) {
        if (!wxMkdir(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath())) {
            pLogger->error("Unable to create database directory at specified location.");
        }
    }

    wxFile file;
    auto succeeded =
        file.Create(common::GetDatabaseFilePath(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath()));
    if (!succeeded) {
        pLogger->error("Unable to create database file at specified location.");
    }
    file.Close();

    return succeeded;
}

void Application::DeleteDatabaseFile()
{
    if (wxFileExists(common::GetDatabaseFilePath(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath()))) {
        if (!wxRemoveFile(
                common::GetDatabaseFilePath(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath()))) {
            pLogger->error("Unable to delete database file.");
        }
    }
}

bool Application::DatabaseFileExists()
{
    /* Check if the database directory is missing */
    if (!wxDirExists(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath())) {
        /* Create the database directory */
        if (!wxMkdir(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath())) {
            return false;
        }
    }

    /* Check if the database file is missing */
    if (!wxFileExists(
            common::GetDatabaseFilePath(cfg::ConfigurationProvider::Get().Configuration->GetDatabasePath()))) {
        /* Backups are enabled so run the database restore wizard to restore database */
        if (cfg::ConfigurationProvider::Get().Configuration->IsBackupEnabled()) {
            int ret = wxMessageBox(wxT("Error: Program cannot find database file\nRestore database from backup?"),
                common::GetProgramName(),
                wxYES_NO | wxICON_ERROR);
            if (ret == wxYES) {
                auto restoreDatabase = new wizard::DatabaseRestoreWizard(nullptr, pLogger, true);
                restoreDatabase->CenterOnParent();
                return restoreDatabase->Run();
            }
        } else { /* Backups are disabled so run the setup wizard to setup the database again */
            int ret = wxMessageBox(wxT("Error: Program cannot find database file\n"
                                       "and database backups are turned off.\n"
                                       "Run setup wizard?"),
                common::GetProgramName(),
                wxYES_NO | wxICON_ERROR);
            if (ret == wxYES) {
                if (!CreateDatabaseFile()) {
                    return false;
                }

                InitializeDatabaseConnectionProvider();

                if (!InitializeDatabaseTables()) {
                    return false;
                }

                if (!RunSetupWizard()) {
                    db::ConnectionProvider::Get().PurgeConnectionPool();
                    DeleteDatabaseFile();
                }
            } else {
                return false;
            }
        }
    }

    return true;
}

bool Application::CheckForDatabaseUpgrade()
{
#ifdef TASKABLE_DEBUG
    wxRegKey key(wxRegKey::HKCU, "Software\\Taskabled");
#else
    wxRegKey key(wxRegKey::HKCU, "Software\\Taskable");
#endif // TASKABLE_DEBUG

    if (key.HasValue(wxT("DatabaseUpgrade"))) {
        long value = 0;
        key.QueryValue(wxT("DatabaseUpgrade"), &value);

        return !!value;
    }
    return false;
}

bool Application::CompleteDatabaseUpgrade()
{
#ifdef TASKABLE_DEBUG
    wxRegKey key(wxRegKey::HKCU, "Software\\Taskabled");
#else
    wxRegKey key(wxRegKey::HKCU, "Software\\Taskable");
#endif // TASKABLE_DEBUG

    if (key.HasValue(wxT("DatabaseUpgrade"))) {
        long value = 0;
        return key.SetValue(wxT("DatabaseUpgrade"), value);
    }

    return false;
}

bool Application::InitializeDatabaseTables()
{
    svc::SetupTables tables(pLogger);
    return tables.CreateTables();
}
} // namespace app

wxIMPLEMENT_APP(app::Application);

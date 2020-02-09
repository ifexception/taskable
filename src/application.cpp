//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2019><Szymon Welgus>
//
//  This program is free software : you can redistribute it and /
//  or modify it under the terms of the GNU General Public License as published
//  by the Free Software Foundation
//  , either version 3 of the License
//  , or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful
//  , but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "application.h"

#include <algorithm>

#include <wx/stdpaths.h>
#include <wx/msw/registry.h>

#include "common/common.h"
#include "common/constants.h"
#include "frame/mainframe.h"
#include "services/databasebackup.h"
#include "wizards/setupwizard.h"
#include "wizards/databaserestorewizard.h"

namespace app
{
Application::Application()
    : pInstanceChecker(std::make_unique<wxSingleInstanceChecker>())
    , pConfig(nullptr)
    , pDatabase(nullptr)
{
}

bool Application::OnInit()
{
#ifndef _DEBUG
    bool isInstanceAlreadyRunning = pInstanceChecker->IsAnotherRunning();
    if (isInstanceAlreadyRunning) {
        wxMessageBox(wxT("Another instance of the application is already running."),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_WARNING);
        return false;
    }
#endif // _DEBUG

    if (!InitializeLogging()) {
        return false;
    }

    if (IsInstalled()) {
        if (!StartupInitialization()) {
            return false;
        }
    } else {
        if (!FirstStartupInitialization()) {
            return false;
        }
    }

    auto frame = new frm::MainFrame(pConfig, pLogger, pDatabase);
    frame->CreateFrame();
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}

bool Application::FirstStartupInitialization()
{
    if (!RunSetupWizard()) {
        return false;
    }

    if (!ConfigureRegistry()) {
        return false;
    }

    if (!ConfigurationFileExists()) {
        return false;
    }

    InitializeSqliteConnection();

    return true;
}

bool Application::StartupInitialization()
{
    if (!ConfigurationFileExists()) {
        return false;
    }

    if (!DatabaseFileExists()) {
        return false;
    }

    InitializeSqliteConnection();

    if (pConfig->IsBackupEnabled()) {
        if (!CreateBackupsDirectory()) {
            return false;
        }
    }

    return true;
}

bool Application::InitializeLogging()
{
    if (!CreateLogsDirectory()) {
        return false;
    }

#ifdef _DEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::warn);
#endif

    auto logDirectory =
        wxString::Format(wxT("%s\\logs\\%s"), wxStandardPaths::Get().GetUserDataDir(), constants::LogsFilename)
            .ToStdString();

    try {
        pLogger = spdlog::daily_logger_st(constants::LoggerName, logDirectory);
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

bool Application::IsInstalled()
{
#if _DEBUG
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\Taskabled");
#else
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\Taskable");
#endif // _DEBUG

    if (key.Exists()) {
        long value = -1;
        key.QueryValue(wxT("Installed"), &value);

        return !!value;
    }
    return false;
}

bool Application::RunSetupWizard()
{
    auto wizard = new wizard::SetupWizard(nullptr, pLogger);
    wizard->CenterOnScreen();
    bool wizardSetupSuccess = wizard->Run();

    return wizardSetupSuccess;
}

bool Application::ConfigureRegistry()
{
#if _DEBUG
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\Taskabled");
#else
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\Taskable");
#endif // _DEBUG

    bool result = key.Create();
    if (!result) {
        pLogger->critical("Unable to create registry: \"Taskable\"");
        return result;
    }
    result = key.SetValue("Installed", 1);
    if (!result) {
        pLogger->critical("Unable to set registry value: \"Installed\"");
    }
    return result;
}

bool Application::ConfigurationFileExists()
{
    bool configFileExists = wxFileExists(common::GetConfigFilePath());
    if (!configFileExists) {
        int res = wxMessageBox(wxT("Error: Program cannot locate configuration file!\nRecreate configuration file?"),
            common::GetProgramName(),
            wxYES_NO | wxICON_ERROR);
        if (res == wxYES) {
            pConfig->RecreateIfNotExists();
            return true;
        } else {
            return false;
        }
    }

    pConfig = std::make_shared<cfg::Configuration>();
    return configFileExists;
}

bool Application::CreateBackupsDirectory()
{
    wxString backupsDirectory;
    if (IsInstalled() && !pConfig->GetBackupPath().empty()) {
        backupsDirectory = pConfig->GetBackupPath();
    } else {
        backupsDirectory = wxString::Format(wxT("%s\\backups"), wxStandardPaths::Get().GetUserDataDir());
    }

    if (!wxDirExists(backupsDirectory)) {
        bool success = wxMkDir(backupsDirectory);
        return !success;
    }

    return true;
}

bool Application::DatabaseFileExists()
{
    /* Check if the 'data' directory is missing */
    if (!wxDirExists(common::GetDatabasePath())) {
        /* Backups are enabled so run the setup wizard to create the directory and to restore database */
        if (pConfig->IsBackupEnabled()) {
            int ret = wxMessageBox(wxT("Error: Program cannot find database file!\nRestore database from backup?"),
                common::GetProgramName(),
                wxYES_NO | wxICON_WARNING);
            if (ret == wxYES) {
                if (wxMkdir(common::GetDatabasePath())) {
                    return false;
                }
                auto restoreDatabase = new wizard::DatabaseRestoreWizard(nullptr, pLogger, pConfig, pDatabase, true);
                restoreDatabase->CenterOnParent();
                restoreDatabase->Run();
            } else {
                return false;
            }
        } else {
            /* Backups are disabled so run the setup wizard to create the directory and setup the database again */
            int ret = wxMessageBox(wxT("Error! Program cannot find database file\n"
                                       "and database backups are turned off.\n"
                                       "Run setup wizard?"),
                common::GetProgramName(),
                wxYES_NO | wxICON_ERROR);
            if (ret == wxYES) {
                return RunSetupWizard();
            } else {
                return false;
            }
        }
    }

    /* Check if the database file is missing */
    bool databaseFileExists = wxFileExists(common::GetDatabaseFilePath());

    if (!databaseFileExists) {
        /* Backups are enabled so run the database restore wizard to restore database */
        if (pConfig->IsBackupEnabled()) {
            int ret = wxMessageBox(wxT("Error: Program cannot find database file!\nRestore database from backup?"),
                common::GetProgramName(),
                wxYES_NO | wxICON_WARNING);
            if (ret == wxYES) {
                auto restoreDatabase = new wizard::DatabaseRestoreWizard(nullptr, pLogger, pConfig, pDatabase, true);
                restoreDatabase->CenterOnParent();
                restoreDatabase->Run();
            }
        }

         /* Backups are disabled so run the setup wizard to setup the database again */
        if (!pConfig->IsBackupEnabled()) {
            int ret = wxMessageBox(wxT("Error! Missing database file\n"
                                       "and database backups are turned off.\n"
                                       "Run setup wizard?"),
                common::GetProgramName(),
                wxYES_NO | wxICON_ERROR);
            if (ret == wxYES) {
                return RunSetupWizard();
            } else {
                return false;
            }
        }
    }
    return databaseFileExists;
}

void Application::InitializeSqliteConnection()
{
    auto config = sqlite::sqlite_config{ sqlite::OpenFlags::READWRITE, nullptr, sqlite::Encoding::UTF8 };
    pDatabase = new sqlite::database(common::GetDatabaseFilePath().ToStdString(), config);
}
} // namespace app

wxIMPLEMENT_APP(app::Application);

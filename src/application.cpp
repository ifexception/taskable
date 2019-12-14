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

#include <wx/msw/registry.h>

#include "common/common.h"
#include "common/constants.h"
#include "frame/mainframe.h"

namespace app
{
Application::Application()
    : pInstanceChecker(std::make_unique<wxSingleInstanceChecker>())
    , pConfig(std::make_shared<cfg::Configuration>())
{
}

Application::~Application() {}

bool Application::OnInit()
{
    /*bool isInstanceAlreadyRunning = pInstanceChecker->IsAnotherRunning();
    if (isInstanceAlreadyRunning) {
        wxMessageBox(wxT("Another instance of the application is already running."),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_WARNING);
        return false;
    }*/

    bool logDirectoryCreated = CreateLogsDirectory();
    if (!logDirectoryCreated) {
        return false;
    }

    bool loggingInitialized = InitializeLogging();
    if (!loggingInitialized) {
        return false;
    }

    auto frame = new frame::MainFrame(pConfig, pLogger);

    bool isInstalled = IsInstalled();
    if (!isInstalled) {
        bool wizardSetupSuccess = frame->RunWizard();
        if (!wizardSetupSuccess) {
            return false;
        }

        bool result = ConfigureRegistry();
        if (!result) {
            return false;
        }
    }

    bool dbFileExists = DatabaseFileExists();
    if (!dbFileExists) {
        return false;
    }

    frame->CreateFrame();
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

bool Application::OnInitializationChecks()
{

    return false;
}

bool Application::CreateLogsDirectory()
{
    wxString logDirectory(wxT("logs"));
    bool logDirectoryExists = wxDirExists(logDirectory);
    if (!logDirectoryExists) {
        bool success = wxMkDir(logDirectory);
        return !success;
    }
    return logDirectoryExists;
}

bool Application::InitializeLogging()
{
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::warn);
#endif

    std::string logDirectory =
        std::string(constants::LogsDirectory) + std::string("/") + std::string(constants::LogsFilename);
    spdlog::flush_every(std::chrono::seconds(3));
    try {
        pLogger = spdlog::daily_logger_st(constants::LoggerName, logDirectory);
    } catch (const spdlog::spdlog_ex& e) {
        wxMessageBox(wxString::Format(wxT("Error initializing logger: %s"), e.what()),
            wxT("Error"),
            wxOK_DEFAULT | wxICON_EXCLAMATION);
        return false;
    }

    //pLogger->enable_backtrace(32);

    return true;
}

bool Application::DatabaseFileExists()
{
    bool dbFileExists = wxFileExists(common::GetDbFileName());
    if (!dbFileExists) {
        int ret = wxMessageBox(wxT("Error: Missing database file!\nRestore from backup?"),
            common::GetProgramName(),
            wxYES_NO | wxICON_WARNING);
        if (ret == wxYES) {
            // run database restore wizard
        }
    }
    return dbFileExists;
}

bool Application::ConfigurationFileExists()
{
    bool configFileExists = wxFileExists(common::GetConfigFileName());
    if (!configFileExists) {
        wxMessageBox(wxT("Error: Missing configuration file!\nCannot proceed."),
            common::GetProgramName(),
            wxOK_DEFAULT | wxICON_ERROR);
    }
    return configFileExists;
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

bool Application::ConfigureRegistry()
{
#if _DEBUG
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\Taskabled");
#else
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\Taskable");
#endif // _DEBUG

    bool result = key.Create();
    result = key.SetValue("Installed", 1);
    if (!result) {
        pLogger->critical("Unable to set registry value: \"Installed\"");
        return false;
    }
    return true;
}
} // namespace app

wxIMPLEMENT_APP(app::Application);

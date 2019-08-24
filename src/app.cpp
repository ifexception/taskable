//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C)<2018><Szymon Welgus>
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

#include "app.h"

#include <wx/msw/registry.h>

#include "common/constants.h"
#include "frame/mainframe.h"

namespace app
{
App::App()
    : pInstanceChecker(std::make_unique<wxSingleInstanceChecker>())
    , pConfig(std::make_shared<cfg::Configuration>())
{
}

App::~App() {}

bool App::OnInit()
{
    bool isInstanceAlreadyRunning = pInstanceChecker->IsAnotherRunning();
    if (isInstanceAlreadyRunning) {
        wxMessageBox(wxT("Another instance of the application is already running."),
            wxT("Tasks Tracker"),
            wxOK_DEFAULT | wxICON_WARNING);
        return false;
    }

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

bool App::CreateLogsDirectory()
{
    wxString logDirectory(wxT("logs"));
    bool logDirectoryExists = wxDirExists(logDirectory);
    if (!logDirectoryExists) {
        bool success = wxMkDir(logDirectory);
        return success;
    }
    return logDirectoryExists;
}

bool App::InitializeLogging()
{
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::warn);
#endif

    std::string logDirectory =
        std::string(Constants::LogsDirectory) + std::string("/") + std::string(Constants::LogsFilename);
    spdlog::flush_every(std::chrono::seconds(3));
    try {
        pLogger = spdlog::daily_logger_st(Constants::LoggerName, logDirectory);
    } catch (const spdlog::spdlog_ex& e) {
        wxMessageBox(wxString::Format(wxT("Error initializing logger: %s"), e.what()),
            wxT("Error"),
            wxOK_DEFAULT | wxICON_EXCLAMATION);
        return false;
    }

    return true;
}

bool App::DatabaseFileExists()
{
    bool dbFileExists = wxFileExists(wxT("tasks-tracker.db")); // FIXME: remove hardcoded string
    return dbFileExists;
}

bool App::IsInstalled()
{
#if _DEBUG
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\TasksTrackerd");
#else
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\TasksTracker");
#endif // _DEBUG

    if (key.Exists()) {
        long value = -1;
#if _DEBUG
        key.QueryValue(wxT("Installedd"), &value);
#else
        key.QueryValue(wxT("Installed"), &value);
#endif // _DEBUG

        return !!value;
    }
    return false;
}

bool App::ConfigureRegistry()
{
#if _DEBUG
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\TasksTrackerd");
#else
    wxRegKey key(wxRegKey::HKCU, "SOFTWARE\\TasksTracker");
#endif // _DEBUG

    bool result = key.Create();
#if _DEBUG
    result = key.SetValue("Installedd", 1);
#else
    result = key.SetValue("Installed", 1);
#endif // _DEBUG
    if (!result) {
        pLogger->critical("Unable to set registry");
        wxMessageBox(wxT("This application requires you to run it as a Administrator"),
            wxT("Error"),
            wxOK_DEFAULT | wxICON_EXCLAMATION);
        return false;
    }
    return true;
}
} // namespace app

wxIMPLEMENT_APP(app::App);

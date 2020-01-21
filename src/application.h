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

#pragma once

#include <memory>

#include <sqlite_modern_cpp.h>

#include <wx/wx.h>
#include <wx/snglinst.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#include "config/configuration.h"

namespace app
{
class Application : public wxApp
{
public:
    Application();
    virtual ~Application();

    bool OnInit() override;

private:
    bool FirstStartupInitialization();
    bool StartupInitialization();

    bool CreateLogsDirectory();
    bool InitializeLogging();
    bool DatabaseFileExists();
    bool ConfigurationFileExists();
    void InitializeSqlite();
    bool IsInstalled();
    bool ConfigureRegistry();
    bool ConfigureBackups();
    bool CreateBackupDirectoryIfNotExists();

    std::shared_ptr<cfg::Configuration> pConfig;
    std::shared_ptr<spdlog::logger> pLogger;
    std::unique_ptr<wxSingleInstanceChecker> pInstanceChecker;

    sqlite::database* pDatabase;
};
} // namespace app

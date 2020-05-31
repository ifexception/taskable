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

#pragma once

#include <memory>

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
    virtual ~Application() = default;

    bool OnInit() override;

private:
    bool FirstStartupInitialization();
    bool StartupInitialization();

    bool InitializeLogging();
    bool CreateLogsDirectory();
    bool InitializeDatabaseConnectionProvider();

    bool IsSetup();
    bool RunSetupWizard();
    bool ConfigureRegistry();

    bool ConfigurationFileExists();

    bool CreateBackupsDirectory();

    bool CreateDatabaseFile();
    void DeleteDatabaseFile();
    bool DatabaseFileExists();

    bool InitializeDatabaseTables();

    std::shared_ptr<cfg::Configuration> pConfig;
    std::shared_ptr<spdlog::logger> pLogger;
    std::unique_ptr<wxSingleInstanceChecker> pInstanceChecker;
};
} // namespace app

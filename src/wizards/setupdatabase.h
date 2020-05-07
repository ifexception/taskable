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

#include <string>
#include <vector>

#include <spdlog/spdlog.h>
#include <sqlite_modern_cpp.h>
#include <wx/string.h>

#include "../data/employerdata.h"
#include "../data/clientdata.h"
#include "../data/projectdata.h"

namespace app::wizard
{
class SetupTables final
{
public:
    SetupTables(std::shared_ptr<spdlog::logger> logger);
    ~SetupTables() = default;

    bool CreateTables();

private:
    std::shared_ptr<spdlog::logger> pLogger;

    std::vector<std::string> ReadFile(wxString fileToRead);
    bool ExecuteDatabaseAction(std::vector<std::string> sqlTokens);

    bool Create();
    bool Seed();

    static const wxString CreateDatabaseFile;
    static const wxString SeedDatabaseFile;
};

class SetupEntities final
{
public:
    SetupEntities(std::shared_ptr<spdlog::logger> logger);
    ~SetupEntities() = default;

    bool CreateEntities(std::string employerName,
        std::string clientName,
        std::string projectName,
        std::string projectDisplayName);

private:
    std::shared_ptr<spdlog::logger> pLogger;
    data::EmployerData mEmployerData;
    data::ClientData mClientData;
    data::ProjectData mProjectData;

    int CreateEmployer(std::string employerName);
    int CreateClient(std::string clientName, int employerId);
    void CreateProject(std::string projectName, std::string projectDisplayName, int employerId, int clientId);
};
} // namespace app::wizard

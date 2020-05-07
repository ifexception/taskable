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

#include "setupdatabase.h"

#include <wx/textfile.h>
#include <wx/stdpaths.h>

#include "../common/util.h"

namespace app::wizard
{
const wxString SetupTables::CreateDatabaseFile = wxT("create-taskable.sql");
const wxString SetupTables::SeedDatabaseFile = wxT("seed-taskable.sql");

SetupTables::SetupTables(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
{
}

bool SetupTables::CreateTables()
{
    return Create() && Seed();
}

std::vector<std::string> SetupTables::ReadFile(wxString fileToRead)
{
    auto createScriptPath = wxString::Format(
        wxT("%s\\%s"), wxStandardPaths::Get().GetExecutablePath(), fileToRead);
    wxTextFile file;
    if (!file.Open(createScriptPath)) {
        // log error and abort
        return std::vector<std::string>();
    }

    wxString fileData;
    fileData = file.GetFirstLine();
    while (!file.Eof()) {
        fileData += file.GetNextLine();
    }

    auto fileDataString = fileData.ToStdString();
    return util::lib::split(fileDataString, ';');
}

bool SetupTables::ExecuteDatabaseAction(std::vector<std::string> sqlTokens)
{
    try {
        auto connectionHandle = db::ConnectionProvider::Get().Handle()->Acquire();
        auto databaseHandle = connectionHandle->DatabaseExecutableHandle();
        for (const auto& token : sqlTokens) {
            *databaseHandle << token;
        }

        db::ConnectionProvider::Get().Handle()->Release(connectionHandle);
        return true;

    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Create Table Procedure - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
}

bool SetupTables::Create()
{
    auto sqlTokens = ReadFile(SetupTables::CreateDatabaseFile);
    if (sqlTokens.empty()) {
        return false;
    }
    return ExecuteDatabaseAction(sqlTokens);
}

bool SetupTables::Seed()
{
    auto sqlTokens = ReadFile(SetupTables::SeedDatabaseFile);
    if (sqlTokens.empty()) {
        return false;
    }
    return ExecuteDatabaseAction(sqlTokens);
}

SetupEntities::SetupEntities(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
    , mEmployerData()
{
}

bool SetupEntities::CreateEntities(std::string employerName,
    std::string clientName,
    std::string projectName,
    std::string projectDisplayName)
{
    try {
        int employerId = CreateEmployer(employerName);
        int clientId = CreateClient(clientName, employerId);
        CreateProject(projectName, projectDisplayName, employerId, clientId);
        return true;
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Entity Setup Procedure - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
}

int SetupEntities::CreateEmployer(std::string employerName)
{
    mEmployerData.Create(std::make_unique<model::EmployerModel>(wxString(employerName)));
    return mEmployerData.GetLastInsertId();
}

int SetupEntities::CreateClient(std::string clientName, int employerId)
{
    if (clientName.empty()) {
        return 0;
    } else {
        mClientData.Create(std::make_unique<model::ClientModel>(wxString(clientName), employerId));
        return mClientData.GetLastInsertId();
    }
}

void SetupEntities::CreateProject(std::string projectName, std::string projectDisplayName, int employerId, int clientId)
{
    bool isAssociatedWithClient = clientId != 0;
    if (isAssociatedWithClient) {
        *pDatabase
            << "INSERT INTO projects(name, display_name, billable, is_active, employer_id, client_id) VALUES(?, ?, ?, "
               "1, ?, ?)"
            << projectName << projectDisplayName << false << employerId << clientId;
    } else {
        *pDatabase
            << "INSERT INTO projects(name, display_name, billable, is_active, employer_id, client_id) VALUES(?, ?, ?, "
               "1, ?, ?)"
            << projectName << projectDisplayName << false << employerId << nullptr;
    }
}
} // namespace app::wizard

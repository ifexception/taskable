// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
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

namespace app::svc
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
    auto scriptPath =
        wxString::Format(wxT("%s\\%s"), wxPathOnly(wxStandardPaths::Get().GetExecutablePath()), fileToRead);
    wxTextFile file;
    if (!file.Open(scriptPath)) {
        pLogger->error("Error occured: Unable to open file: {0}", scriptPath.ToStdString());
        return std::vector<std::string>();
    }

    wxString fileData;
    fileData = file.GetFirstLine();
    while (!file.Eof()) {
        fileData += file.GetNextLine();
    }

    /* Since some currencies symbols are UTF8 specific characters
       we cannot use ToStdString() as that (tries) to convert to regular
       ASCII and the conversion results in an error.
       The solution is to first convert to UTF8 explicitly and then
       initialize a std::string from that. This way std::string will handle
       the symbol character encodings correctly
    */
    /* TODO: Internationalization of Taskable might require using this approach everywhere */
    // Note that in the debugger, the encodings do not look correct
    auto fileDataString = fileData.ToUTF8();
    return util::lib::split(std::string(fileDataString), ';');
}

bool SetupTables::ExecuteDatabaseAction(std::vector<std::string> sqlTokens)
{
    auto connectionHandle = db::ConnectionProvider::Get().Handle()->Acquire();

    try {
        auto databaseHandle = connectionHandle->DatabaseExecutableHandle();
        for (const auto& token : sqlTokens) {
            *databaseHandle << token;
        }

    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Create Table Procedure - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    db::ConnectionProvider::Get().Handle()->Release(connectionHandle);
    return true;
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
} // namespace app::svc

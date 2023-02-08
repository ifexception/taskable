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

#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <sqlite_modern_cpp.h>
#include <wx/string.h>

#include "../database/connectionprovider.h"
#include "../database/sqliteconnection.h"

namespace app::svc
{
class DatabaseBackup final
{
public:
    DatabaseBackup() = delete;
    DatabaseBackup(std::shared_ptr<spdlog::logger> logger);
    ~DatabaseBackup();

    bool Execute();

private:
    wxString CreateBackupFileName();
    wxString GetBackupFullPath(const wxString& fileName);
    bool CreateBackupFile(const wxString& fileName);
    bool ExecuteBackup(const wxString& fileName);

    std::shared_ptr<spdlog::logger> pLogger;
    std::shared_ptr<db::SqliteConnection> pConnection;
};
} // namespace app::svc

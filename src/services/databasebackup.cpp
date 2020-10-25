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

#include "databasebackup.h"

#include <string>

#include <wx/datetime.h>
#include <wx/file.h>
#include <wx/stdpaths.h>

#include "../common/common.h"
#include "../config/configurationprovider.h"

namespace app::svc
{
DatabaseBackup::DatabaseBackup(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

DatabaseBackup::~DatabaseBackup()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

bool DatabaseBackup::Execute()
{
    wxString fileName = CreateBackupFileName();
    wxString filePath = GetBackupFullPath(fileName);
    if (fileName.empty() || filePath.empty()) {
        return false;
    }

    if (!CreateBackupFile(filePath)) {
        return false;
    }
    if (!ExecuteBackup(filePath)) {
        return false;
    }
    return true;
}

wxString DatabaseBackup::CreateBackupFileName()
{
    auto dateTime = wxDateTime::Now();
    auto dateTimeString = dateTime.FormatISODate();
    auto indexOfPeriod = common::GetDatabaseFileName().find(wxT("."), 0);
    wxString databaseFileName = "";
    if (indexOfPeriod != wxString::npos) {
        databaseFileName = common::GetDatabaseFileName().substr(0, indexOfPeriod);
    } else {
        return wxGetEmptyString();
    }

    auto backupFileName = wxString::Format(wxT("%s.%s.db"), databaseFileName, dateTimeString);

    return backupFileName;
}

wxString DatabaseBackup::GetBackupFullPath(const wxString& filename)
{
    auto backupDirectory = cfg::ConfigurationProvider::Get().Configuration->GetBackupPath();
    auto backupFilePath = wxString::Format(wxT("%s\\%s"), backupDirectory, filename);
    return backupFilePath;
}

bool DatabaseBackup::CreateBackupFile(const wxString& fileName)
{
    wxFile file;
    bool success = file.Create(fileName, true);
    if (!success) {
        pLogger->error("Failed to create file {0}", fileName.ToStdString());
    }
    file.Close();
    return success;
}

bool DatabaseBackup::ExecuteBackup(const wxString& fileName)
{
    try {
        auto config = sqlite::sqlite_config{ sqlite::OpenFlags::READWRITE, nullptr, sqlite::Encoding::UTF8 };
        sqlite::database backupConnection(fileName.ToStdString(), config);
        auto existingConnection = pConnection->DatabaseExecutableHandle()->connection();

        auto state = std::unique_ptr<sqlite3_backup, decltype(&sqlite3_backup_finish)>(
            sqlite3_backup_init(backupConnection.connection().get(), "main", existingConnection.get(), "main"),
            sqlite3_backup_finish);

        if (state) {
            int rc = -1;
            do {
                rc = sqlite3_backup_step(state.get(), -1);
            } while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
        }
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured when running database backup - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
    return true;
}
} // namespace app::svc

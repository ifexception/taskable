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

#include "databasebackupdeleter.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/regex.h>

#include "../common/common.h"

namespace app::svc
{
bool DatabaseBackupDeleter::Execute()
{
    auto filesToDelete = GetFilesForDeletion();
    if (filesToDelete.empty()) {
        return true;
    }

    auto result = DeleteFilesAfterSpecifiedDate(filesToDelete);

    return result;
}

wxArrayString DatabaseBackupDeleter::GetFilesForDeletion()
{
    auto backupPath = cfg::ConfigurationProvider::Get().Configuration->GetBackupPath();
    auto deleteBackupsAfter = cfg::ConfigurationProvider::Get().Configuration->GetDeleteBackupsAfter();

    wxArrayString backupPathFileNames;
    wxDir::GetAllFiles(backupPath, &backupPathFileNames, wxEmptyString, wxDIR_FILES);

    wxArrayString filesToDelete;

    for (const auto& file : backupPathFileNames) {
        wxRegEx dateRegex(wxT("([0-9]{4}-[0-9]{2}-[0-9]{2})"));
        if (dateRegex.IsValid()) {
            if (dateRegex.Matches(file)) {
                wxFileName filename(file);
                auto dateComponent = dateRegex.GetMatch(file, 0);
                wxDateTime date;
                if (!date.ParseDate(dateComponent)) {
                    return wxArrayString();
                }
                const time_t OneDay = 24 * 60 * 60;
                auto dateOffset = OneDay * deleteBackupsAfter;
                auto dateNow = wxDateTime::Now().GetTicks();
                auto olderDate = wxDateTime(dateNow - dateOffset);

                if (date.IsEarlierThan(olderDate)) {
                    filesToDelete.Add(file);
                }
            }
        }
    }

    return filesToDelete;
}

bool DatabaseBackupDeleter::DeleteFilesAfterSpecifiedDate(const wxArrayString& filesToDelete)
{
    for (const auto& file : filesToDelete) {
        if (!wxRemoveFile(file)) {
            return false;
        }
    }
    return true;
}
} // namespace app::svc

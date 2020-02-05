//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
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

namespace app::ids
{
enum class MenuIds : int {
    File_NewEmployerId = 1,
    File_NewClientId = 2,
    File_NewProjectId = 3,
    File_NewEntryTaskId = 4,
    File_NewTimedTaskId = 5,
    File_NewCategoryId = 6,
    Edit_EditEmployerId = 7,
    Edit_EditClientId = 8,
    Edit_EditProjectId = 9,
    Edit_EditCategoryId = 10,
    Edit_PreferencesId = 11,
    File_StopwatchTaskId = 12,
    Help_CheckForUpdateId = 13,
    Tools_RestoreDatabaseId = 14,
    Tools_BackupDatabaseId = 15,
};

static const int ID_NEW_ENTRY_TASK = static_cast<int>(ids::MenuIds::File_NewEntryTaskId);
static const int ID_NEW_TIMED_TASK = static_cast<int>(ids::MenuIds::File_NewTimedTaskId);
static const int ID_NEW_EMPLOYER = static_cast<int>(ids::MenuIds::File_NewEmployerId);
static const int ID_NEW_CLIENT = static_cast<int>(ids::MenuIds::File_NewClientId);
static const int ID_NEW_PROJECT = static_cast<int>(ids::MenuIds::File_NewProjectId);
static const int ID_NEW_CATEGORY = static_cast<int>(ids::MenuIds::File_NewCategoryId);
static const int ID_STOPWATCH_TASK = static_cast<int>(ids::MenuIds::File_StopwatchTaskId);

static const int ID_EDIT_EMPLOYER = static_cast<int>(MenuIds::Edit_EditEmployerId);
static const int ID_EDIT_CLIENT = static_cast<int>(MenuIds::Edit_EditClientId);
static const int ID_EDIT_PROJECT = static_cast<int>(MenuIds::Edit_EditProjectId);
static const int ID_EDIT_CATEGORY = static_cast<int>(MenuIds::Edit_EditCategoryId);

static const int ID_PREFERENCES = static_cast<int>(MenuIds::Edit_PreferencesId);

static const int ID_CHECK_FOR_UPDATE = static_cast<int>(MenuIds::Help_CheckForUpdateId);

static const int ID_RESTORE_DATABASE = static_cast<int>(MenuIds::Tools_RestoreDatabaseId);
static const int ID_BACKUP_DATABASE = static_cast<int>(MenuIds::Tools_BackupDatabaseId);

enum class DialogIds : int { ErrorOccured = 5999 };

static const int ID_ERROR_OCCURED = static_cast<int>(ids::DialogIds::ErrorOccured);
} // namespace app::ids

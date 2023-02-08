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

namespace app::ids
{
enum class MenuIds : int {
    File_NewEmployerId = 1,
    File_NewClientId,
    File_NewProjectId,
    File_NewEntryTaskId,
    File_NewTimedTaskId,
    File_NewCategoryId,
    File_View_WeeklyView,
    File_View_MeetingsView,
    File_StopwatchTaskId,
    Edit_EditEmployerId,
    Edit_EditClientId,
    Edit_EditProjectId,
    Edit_EditCategoryId,
    Edit_PreferencesId,
    Export_ToCsv,
    Help_CheckForUpdateId,
    Tools_RestoreDatabaseId,
    Tools_BackupDatabaseId,

    Unp_ReturnToCurrentDate = 32,
};

static const int ID_NEW_ENTRY_TASK = static_cast<int>(ids::MenuIds::File_NewEntryTaskId);
static const int ID_NEW_TIMED_TASK = static_cast<int>(ids::MenuIds::File_NewTimedTaskId);
static const int ID_NEW_EMPLOYER = static_cast<int>(ids::MenuIds::File_NewEmployerId);
static const int ID_NEW_CLIENT = static_cast<int>(ids::MenuIds::File_NewClientId);
static const int ID_NEW_PROJECT = static_cast<int>(ids::MenuIds::File_NewProjectId);
static const int ID_NEW_CATEGORY = static_cast<int>(ids::MenuIds::File_NewCategoryId);
static const int ID_STOPWATCH_TASK = static_cast<int>(ids::MenuIds::File_StopwatchTaskId);
static const int ID_WEEKLY_VIEW = static_cast<int>(ids::MenuIds::File_View_WeeklyView);
static const int ID_MEETINGS_VIEW = static_cast<int>(ids::MenuIds::File_View_MeetingsView);

static const int ID_EDIT_EMPLOYER = static_cast<int>(MenuIds::Edit_EditEmployerId);
static const int ID_EDIT_CLIENT = static_cast<int>(MenuIds::Edit_EditClientId);
static const int ID_EDIT_PROJECT = static_cast<int>(MenuIds::Edit_EditProjectId);
static const int ID_EDIT_CATEGORY = static_cast<int>(MenuIds::Edit_EditCategoryId);

static const int ID_PREFERENCES = static_cast<int>(MenuIds::Edit_PreferencesId);

static const int ID_CHECK_FOR_UPDATE = static_cast<int>(MenuIds::Help_CheckForUpdateId);

static const int ID_RESTORE_DATABASE = static_cast<int>(MenuIds::Tools_RestoreDatabaseId);
static const int ID_BACKUP_DATABASE = static_cast<int>(MenuIds::Tools_BackupDatabaseId);

static const int ID_EXPORT_CSV = static_cast<int>(MenuIds::Export_ToCsv);

static const int ID_RETURN_TO_CURRENT_DATE = static_cast<int>(MenuIds::Unp_ReturnToCurrentDate);

enum class DialogIds : int { ErrorOccured = 5999 };

static const int ID_ERROR_OCCURED = static_cast<int>(ids::DialogIds::ErrorOccured);
} // namespace app::ids

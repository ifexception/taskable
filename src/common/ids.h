//  Tasks Tracker is a desktop that aids you in tracking your timesheets
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

#include <wx/wx.h>

namespace app::ids
{
enum class MenuIds : int {
    NewEmployerId = 1,
    NewClientId = 2,
    NewProjectId = 3,
    NewEntryTaskId = 4,
    NewTimedTaskId = 5,
    NewCategoryId = 6,
    EditEmployerId = 7,
    EditClientId = 8,
    EditProjectId = 9,
    EditCategoryId = 10,
    SettingsId = 11,
    StopwatchTaskId = 12,
    CheckForUpdateId=13
};

static const int ID_NEW_ENTRY_TASK = static_cast<int>(ids::MenuIds::NewEntryTaskId);
static const int ID_NEW_TIMED_TASK = static_cast<int>(ids::MenuIds::NewTimedTaskId);
static const int ID_NEW_EMPLOYER = static_cast<int>(ids::MenuIds::NewEmployerId);
static const int ID_NEW_CLIENT = static_cast<int>(ids::MenuIds::NewClientId);
static const int ID_NEW_PROJECT = static_cast<int>(ids::MenuIds::NewProjectId);
static const int ID_NEW_CATEGORY = static_cast<int>(ids::MenuIds::NewCategoryId);
static const int ID_STOPWATCH_TASK = static_cast<int>(ids::MenuIds::StopwatchTaskId);

static const int ID_EDIT_EMPLOYER = static_cast<int>(MenuIds::EditEmployerId);
static const int ID_EDIT_CLIENT = static_cast<int>(MenuIds::EditClientId);
static const int ID_EDIT_PROJECT = static_cast<int>(MenuIds::EditProjectId);
static const int ID_EDIT_CATEGORY = static_cast<int>(MenuIds::EditCategoryId);

static const int ID_SETTINGS = static_cast<int>(MenuIds::SettingsId);

static const int ID_CHECK_FOR_UPDATE = static_cast<int>(MenuIds::CheckForUpdateId);

enum class Events : int { ID_SAVE = wxID_HIGHEST + 1, ID_TASK_INSERTED, ID_START_PAUSE, ID_STOP };

static const int ID_SAVE = static_cast<int>(Events::ID_SAVE);
static const int ID_TASK_INSERTED = static_cast<int>(Events::ID_TASK_INSERTED);
static const int ID_START_PAUSE = static_cast<int>(Events::ID_START_PAUSE);
static const int ID_STOP = static_cast<int>(Events::ID_STOP);
} // namespace app::ids

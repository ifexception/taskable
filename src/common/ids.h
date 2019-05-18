//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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
enum class MenuIds : int
{
    EmployerMenuId = 1,
    ProjectMenuId = 2,
    ClientMenuId = 3,
    TaskDetailMenuId = 4,
    CategoryMenuId = 5,
    EmployerEditId = 6,
    ClientEditId = 7,
    ProjectEditId = 8,
    CategoryEditId = 9,
};

static const int ID_EDIT_EMPLOYER = static_cast<int>(MenuIds::EmployerEditId);
static const int ID_EDIT_CLIENT = static_cast<int>(MenuIds::ClientEditId);
static const int ID_EDIT_PROJECT = static_cast<int>(MenuIds::ProjectEditId);
static const int ID_EDIT_CATEGORY = static_cast<int>(MenuIds::CategoryEditId);

enum class Events : int
{
    ID_OFFSET = 1,
    ID_SAVE = wxID_HIGHEST + ID_OFFSET,
    ID_TASK_INSERTED = ID_SAVE + ID_OFFSET
};

static const int ID_SAVE = static_cast<int>(Events::ID_SAVE);
static const int ID_TASK_INSERTED = static_cast<int>(Events::ID_TASK_INSERTED);
}

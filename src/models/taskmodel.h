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

#include <wx/datetime.h>

namespace app::model
{
class TaskModel final
{
public:
    TaskModel();
    TaskModel(int taskId, wxString date, int dateCreated, int dateModified, bool isActive);
    ~TaskModel() = default;

    const int GetTaskId() const;
    const wxString GetTaskDate() const;
    const wxDateTime GetDateCreated();
    const wxDateTime GetDateModified();
    const bool IsActive() const;

    void SetTaskId(const int taskId);
    void SetTaskDate(const wxString& date);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateUpdated(const wxDateTime& dateModified);
    void IsActive(const bool isActive);

private:
    int mTaskId;
    wxString mTaskDate;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;
};
} // namespace app::model

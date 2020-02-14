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

#pragma once

#include <memory>

#include <wx/datetime.h>

namespace app::model
{
class TaskModel final
{
public:
    TaskModel();
    TaskModel(int taskId, bool initializeFromDatabase);
    TaskModel(int taskId, wxString date, int dateCreated, int dateModified, bool isActive);
    ~TaskModel() = default;

    const int GetTaskId() const;
    const wxDateTime GetTaskDate() const;
    const wxDateTime GetDateCreated();
    const wxDateTime GetDateModified();
    const bool IsActive() const;

    void SetTaskId(const int taskId);
    void SetTaskDate(const wxDateTime& date);
    void SetDateCreated(const wxDateTime& dateCreated);
    void SetDateUpdated(const wxDateTime& dateModified);
    void IsActive(const bool isActive);

    static std::unique_ptr<TaskModel> GetByDate(const wxDateTime date);
    static std::unique_ptr<TaskModel> GetById(const int taskId);
    static void Create(const wxDateTime date);

private:
    int mTaskId;
    wxDateTime mTaskDate;
    wxDateTime mDateCreated;
    wxDateTime mDateModified;
    bool bIsActive;

    static int GetId(const wxDateTime date);

    static const std::string getTaskId;
    static const std::string getTaskByDate;
    static const std::string getTaskById;
    static const std::string createTask;
};
} // namespace app::model

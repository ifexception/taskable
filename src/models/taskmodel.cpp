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

#include "taskmodel.h"

#include "../common/util.h"

namespace app::model
{
TaskModel::TaskModel()
    : mTaskId(-1)
    , mTaskDate(wxGetEmptyString())
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
{
}

TaskModel::TaskModel(int taskId, wxString date, int dateCreated, int dateModified, bool isActive)
    : TaskModel()
{
    mTaskId = taskId;
    mTaskDate= date;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

const int TaskModel::GetTaskId() const
{
    return mTaskId;
}

const wxString TaskModel::GetTaskDate() const
{
    return mTaskDate;
}

const wxDateTime TaskModel::GetDateCreated()
{
    return mDateCreated;
}

const wxDateTime TaskModel::GetDateModified()
{
    return mDateModified;
}

const bool TaskModel::IsActive() const
{
    return bIsActive;
}

void TaskModel::SetTaskId(const int taskId)
{
    mTaskId = taskId;
}

void TaskModel::SetTaskDate(const wxString& date)
{
    mTaskDate = date;
}

void TaskModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void TaskModel::SetDateUpdated(const wxDateTime& dateModified)
{
    mDateModified = dateModified;
}

void TaskModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}
} // namespace app::model

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

#include "taskitemtypemodel.h"

namespace app::model
{
TaskItemTypeModel::TaskItemTypeModel()
    : mTaskItemTypeId(-1)
    , mName(wxGetEmptyString())
    , mType()
{
}

TaskItemTypeModel::TaskItemTypeModel(const int taskItemTypeId)
    : TaskItemTypeModel()
{
    mTaskItemTypeId = taskItemTypeId;
    mType = static_cast<constants::TaskItemTypes>(mTaskItemTypeId);
}

TaskItemTypeModel::TaskItemTypeModel(int taskItemTypeId, wxString name)
    : TaskItemTypeModel()
{
    mTaskItemTypeId = taskItemTypeId;
    mName = name;
    mType = static_cast<constants::TaskItemTypes>(mTaskItemTypeId);
}

const int TaskItemTypeModel::GetTaskItemTypeId() const
{
    return mTaskItemTypeId;
}

const wxString TaskItemTypeModel::GetName() const
{
    return mName;
}

const constants::TaskItemTypes TaskItemTypeModel::GetType() const
{
    return mType;
}

void TaskItemTypeModel::SetTaskItemTypeId(const int taskItemTypeId)
{
    mTaskItemTypeId = taskItemTypeId;
}

void TaskItemTypeModel::SetName(const wxString& name)
{
    mName = name;
}
} // namespace app::model

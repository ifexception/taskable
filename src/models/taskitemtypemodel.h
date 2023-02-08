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
#include <wx/string.h>

#include "../common/constants.h"

namespace app::model
{
class TaskItemTypeModel final
{
public:
    TaskItemTypeModel();
    TaskItemTypeModel(const int taskItemTypeId);
    TaskItemTypeModel(int taskItemTypeId, wxString name);
    ~TaskItemTypeModel() = default;

    const int GetTaskItemTypeId() const;
    const wxString GetName() const;
    const constants::TaskItemTypes GetType() const;

    void SetTaskItemTypeId(const int taskItemTypeId);
    void SetName(const wxString& name);

private:
    int mTaskItemTypeId;
    wxString mName;
    constants::TaskItemTypes mType;
};
} // namespace app::model

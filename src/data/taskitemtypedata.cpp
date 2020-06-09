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

#include "taskitemtypedata.h"

#include <spdlog/spdlog.h>

#include <wx/string.h>

namespace app::data
{
TaskItemTypeData::TaskItemTypeData()
{
    spdlog::get("msvc")->debug("ACQUIRE connection in TaskItemTypeData");
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

TaskItemTypeData::~TaskItemTypeData()
{
    spdlog::get("msvc")->debug("RELEASE connection in TaskItemTypeData");
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

std::unique_ptr<model::TaskItemTypeModel> TaskItemTypeData::GetById(const int taskItemTypeId)
{
    std::unique_ptr<model::TaskItemTypeModel> taskItemType = nullptr;

    *pConnection->DatabaseExecutableHandle() << TaskItemTypeData::getTaskItemTypeById << taskItemTypeId >>
        [&](int taskItemTypeId, std::string name) {
            taskItemType = std::make_unique<model::TaskItemTypeModel>(taskItemTypeId, wxString(name));
        };

    return std::move(taskItemType);
}

std::vector<std::unique_ptr<model::TaskItemTypeModel>> TaskItemTypeData::GetAll()
{
    std::vector<std::unique_ptr<model::TaskItemTypeModel>> taskItemTypes;

    *pConnection->DatabaseExecutableHandle() << TaskItemTypeData::getTaskItemTypes >>
        [&](int taskItemTypeId, std::string name) {
            auto taskItemType = std::make_unique<model::TaskItemTypeModel>(taskItemTypeId, wxString(name));
            taskItemTypes.push_back(std::move(taskItemType));
        };

    return taskItemTypes;
}

const std::string TaskItemTypeData::getTaskItemTypeById = "SELECT task_item_type_id, "
                                                          "name "
                                                          "FROM task_item_types "
                                                          "WHERE task_item_type_id = ?";

const std::string TaskItemTypeData::getTaskItemTypes = "SELECT task_item_type_id, "
                                                       "name "
                                                       "FROM task_item_types ";
} // namespace app::data

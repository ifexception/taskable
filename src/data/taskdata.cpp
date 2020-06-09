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

#include "taskdata.h"

#include <spdlog/spdlog.h>

namespace app::data
{
TaskData::TaskData()
{
    spdlog::get("msvc")->debug("ACQUIRE connection in TaskData");
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

TaskData::~TaskData()
{
    spdlog::get("msvc")->debug("RELEASE connection in TaskData");
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

int TaskData::GetId(const wxDateTime date)
{
    int rTaskId = 0;
    bool taskDoesNotExistYet = true;

    *pConnection->DatabaseExecutableHandle() << TaskData::getTaskId << date.FormatISODate().ToStdString() >>
        [&](std::unique_ptr<int> taskId) {
            if (taskId != nullptr) {
                taskDoesNotExistYet = false;
                rTaskId = *taskId;
            }
        };

    if (taskDoesNotExistYet) {
        rTaskId = Create(date);
    }

    return rTaskId;
}

std::unique_ptr<model::TaskModel> TaskData::GetByDate(const wxDateTime date)
{
    std::unique_ptr<model::TaskModel> taskModel = nullptr;

    int taskId = GetId(date);

    *pConnection->DatabaseExecutableHandle() << TaskData::getTaskByDate << date.FormatISODate().ToStdString() >>
        [&](int taskId, std::string date, int dateCreated, int dateModified, bool isActive) {
            taskModel = std::make_unique<model::TaskModel>(taskId, wxString(date), dateCreated, dateModified, isActive);
        };

    return std::move(taskModel);
}

std::unique_ptr<model::TaskModel> TaskData::GetById(const int taskId)
{
    std::unique_ptr<model::TaskModel> taskModel = nullptr;

    *pConnection->DatabaseExecutableHandle() << TaskData::getTaskById << taskId >>
        [&](int taskId, std::string date, int dateCreated, int dateModified, bool isActive) {
            taskModel = std::make_unique<model::TaskModel>(taskId, wxString(date), dateCreated, dateModified, isActive);
        };

    return std::move(taskModel);
}

int64_t TaskData::Create(const wxDateTime date)
{
    *pConnection->DatabaseExecutableHandle() << TaskData::createTask << date.FormatISODate().ToStdString();
    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

const std::string TaskData::getTaskId = "SELECT task_id "
                                        "FROM tasks "
                                        "WHERE task_date = ?";

const std::string TaskData::getTaskByDate = "SELECT task_id, "
                                            "task_date, "
                                            "date_created, "
                                            "date_modified, "
                                            "is_active "
                                            "FROM tasks "
                                            "WHERE task_date = ?";

const std::string TaskData::getTaskById = "SELECT task_id, "
                                          "task_date, "
                                          "date_created, "
                                          "date_modified, "
                                          "is_active "
                                          "FROM tasks "
                                          "WHERE task_id = ?";

const std::string TaskData::createTask = "INSERT INTO "
                                         "tasks (task_date, is_active) "
                                         "VALUES (?, 1)";
} // namespace app::data

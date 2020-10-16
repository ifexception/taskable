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

#include <cstdint>

#include <wx/datetime.h>

#include "../database/connectionprovider.h"
#include "../database/sqliteconnection.h"
#include "../models/taskmodel.h"

namespace app::data
{
class TaskData final
{
public:
    TaskData();
    ~TaskData();

    std::unique_ptr<model::TaskModel> GetByDate(const wxDateTime& date);
    std::unique_ptr<model::TaskModel> GetById(const int taskId);
    int64_t Create(const wxDateTime& date);

private:
    int GetId(const wxDateTime& date);

    std::shared_ptr<db::SqliteConnection> pConnection;

    static const std::string getTaskId;
    static const std::string getTaskByDate;
    static const std::string getTaskById;
    static const std::string createTask;
};
} // namespace app::data

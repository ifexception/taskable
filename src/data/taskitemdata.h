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

#include <wx/string.h>

#include "../database/connectionprovider.h"
#include "../database/sqliteconnection.h"
#include "../models/TaskItemModel.h"

namespace app::data
{
class TaskItemData final
{
public:
    TaskItemData();
    ~TaskItemData();

    int64_t Create(std::unique_ptr<model::TaskItemModel> taskItem);
    std::unique_ptr<model::TaskItemModel> GetById(const int taskItemId);
    void Update(std::unique_ptr<model::TaskItemModel> taskItem);
    void Delete(std::unique_ptr<model::TaskItemModel> taskItem);
    void Delete(int taskItemId);
    std::vector<std::unique_ptr<model::TaskItemModel>> GetByDate(const wxString& date);
    std::vector<wxString> GetHours(const wxString& date);
    int GetTaskItemTypeIdByTaskItemId(const int taskItemId);
    std::vector<std::unique_ptr<model::TaskItemModel>> GetByWeek(const wxString& fromDate, const wxString& toDate);
    wxString GetDescriptionById(const int taskItemId);
    std::vector<wxString> GetHoursByWeek(const wxString& fromDate, const wxString& toDate);

private:
    std::shared_ptr<db::SqliteConnection> pConnection;

    static const std::string createTaskItem;
    static const std::string getTaskItemsByDate;
    static const std::string getTaskItemById;
    static const std::string updateTaskItem;
    static const std::string deleteTaskItem;
    static const std::string getTaskHoursByTaskId;
    static const std::string getTaskItemTypeIdByTaskItemId;
    static const std::string getTaskItemsByWeek;
    static const std::string getDescriptionById;
    static const std::string getTaskHoursByWeek;
};
}

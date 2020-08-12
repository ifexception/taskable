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

#include "taskitemdata.h"

#include <spdlog/spdlog.h>

#include "../common/util.h"

#include "projectdata.h"
#include "taskitemtypedata.h"
#include "taskdata.h"
#include "categorydata.h"

namespace app::data
{
TaskItemData::TaskItemData()
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
    spdlog::get("msvc")->debug("ACQUIRE connection in TaskItemData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

TaskItemData::~TaskItemData()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
    spdlog::get("msvc")->debug("RELEASE connection in TaskItemData|ConnectionTally: {0:d}",
        db::ConnectionProvider::Get().Handle()->ConnectionsInUse());
}

int64_t TaskItemData::Create(std::unique_ptr<model::TaskItemModel> taskItem)
{
    auto ps = *pConnection->DatabaseExecutableHandle() << TaskItemData::createTaskItem;

    if (taskItem->IsEntryTask()) {
        ps << nullptr << nullptr;
    }
    if (taskItem->IsTimedTask()) {
        ps << taskItem->GetStartTime()->FormatISOTime().ToStdString()
           << taskItem->GetEndTime()->FormatISOTime().ToStdString();
    }

    ps << taskItem->GetDuration().ToStdString() << taskItem->GetDescription().ToStdString();

    data::ProjectData projectData(pConnection);
    taskItem->SetProject(std::move(projectData.GetById(taskItem->GetProjectId())));
    if (taskItem->GetProject()->IsNonBillableScenario()) {
        ps << taskItem->IsBillable() << nullptr;
    }

    if (taskItem->GetProject()->IsBillableWithUnknownRateScenario()) {
        ps << taskItem->IsBillable() << nullptr;
    }

    if (taskItem->GetProject()->IsBillableScenarioWithHourlyRate()) {
        ps << taskItem->IsBillable() << *taskItem->GetCalculatedRate();
    }

    ps << taskItem->GetTaskItemTypeId() << taskItem->GetProjectId() << taskItem->GetCategoryId()
       << taskItem->GetTaskId();

    ps.execute();

    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

std::unique_ptr<model::TaskItemModel> TaskItemData::GetById(const int taskItemId)
{
    std::unique_ptr<model::TaskItemModel> taskItem = nullptr;

    *pConnection->DatabaseExecutableHandle() << TaskItemData::getTaskItemById << taskItemId >>
        [&](int taskItemId,
            std::unique_ptr<std::string> startTime,
            std::unique_ptr<std::string> endTime,
            std::string duration,
            std::string description,
            bool billable,
            std::unique_ptr<double> calculatedRate,
            int dateCreated,
            int dateModified,
            bool isActive,
            int taskItemTypeId,
            int projectId,
            int categoryId,
            int taskId) {
            taskItem = std::make_unique<model::TaskItemModel>(
                taskItemId, duration, description, billable, dateCreated, dateModified, isActive);

            if (startTime == nullptr && endTime == nullptr) {
                taskItem->SetDurationTime(wxString(duration));
            }

            if (startTime != nullptr && endTime != nullptr) {
                taskItem->SetStartTime(wxString(*startTime));
                taskItem->SetEndTime(wxString(*endTime));
            }

            if (calculatedRate != nullptr) {
                taskItem->SetCalculatedRate(std::move(calculatedRate));
            }

            taskItem->SetTaskItemTypeId(taskItemTypeId);
            data::TaskItemTypeData taskItemTypeData;
            auto taskItemType = taskItemTypeData.GetById(taskItemTypeId);
            taskItem->SetTaskItemType(std::move(taskItemType));

            taskItem->SetProjectId(projectId);
            data::ProjectData projectData(pConnection);
            auto project = projectData.GetById(projectId);
            taskItem->SetProject(std::move(project));

            taskItem->SetCategoryId(categoryId);
            data::CategoryData categoryData(pConnection);
            auto category = categoryData.GetById(categoryId);
            taskItem->SetCategory(std::move(category));

            taskItem->SetTaskId(taskId);

            data::TaskData taskData(pConnection);
            auto task = taskData.GetById(taskId);
            taskItem->SetTask(std::move(task));
        };

    return std::move(taskItem);
}

void TaskItemData::Update(std::unique_ptr<model::TaskItemModel> taskItem)
{
    auto ps = *pConnection->DatabaseExecutableHandle() << TaskItemData::updateTaskItem;

    if (taskItem->IsEntryTask()) {
        ps << nullptr << nullptr;
    }
    if (taskItem->IsTimedTask()) {
        ps << taskItem->GetStartTime()->FormatISOTime().ToStdString()
           << taskItem->GetEndTime()->FormatISOTime().ToStdString();
    }

    ps << taskItem->GetDuration().ToStdString() << taskItem->GetDescription().ToStdString();

    if (taskItem->GetProject()->IsNonBillableScenario()) {
        ps << taskItem->IsBillable() << nullptr;
    }

    if (taskItem->GetProject()->IsBillableWithUnknownRateScenario()) {
        ps << taskItem->IsBillable() << nullptr;
    }

    if (taskItem->GetProject()->IsBillableScenarioWithHourlyRate()) {
        ps << taskItem->IsBillable() << *taskItem->GetCalculatedRate();
    }

    ps << util::UnixTimestamp();

    ps << taskItem->GetProjectId() << taskItem->GetCategoryId();

    ps << taskItem->GetTaskItemId();

    ps.execute();
}

void TaskItemData::Delete(std::unique_ptr<model::TaskItemModel> taskItem)
{
    *pConnection->DatabaseExecutableHandle()
        << TaskItemData::deleteTaskItem << util::UnixTimestamp() << taskItem->GetTaskItemId();
}

void TaskItemData::Delete(int taskItemId)
{
    *pConnection->DatabaseExecutableHandle() << TaskItemData::deleteTaskItem << util::UnixTimestamp() << taskItemId;
}

std::vector<std::unique_ptr<model::TaskItemModel>> TaskItemData::GetByDate(const wxString& date)
{
    std::vector<std::unique_ptr<model::TaskItemModel>> taskItems;

    *pConnection->DatabaseExecutableHandle() << TaskItemData::getTaskItemsByDate << date >>
        [&](int taskItemId,
            std::string taskDate,
            std::unique_ptr<std::string> startTime,
            std::unique_ptr<std::string> endTime,
            std::string duration,
            std::string description,
            bool billable,
            std::unique_ptr<double> calculatedRate,
            int dateCreated,
            int dateModified,
            bool isActive,
            int taskItemTypeId,
            int projectId,
            int categoryId,
            int taskId) {
            auto taskItem = std::make_unique<model::TaskItemModel>(
                taskItemId, duration, description, billable, dateCreated, dateModified, isActive);

            if (startTime == nullptr && endTime == nullptr) {
                taskItem->SetDurationTime(wxString(duration));
            }

            if (startTime != nullptr && endTime != nullptr) {
                taskItem->SetStartTime(wxString(*startTime));
                taskItem->SetEndTime(wxString(*endTime));
            }

            if (calculatedRate != nullptr) {
                taskItem->SetCalculatedRate(std::move(calculatedRate));
            }

            taskItem->SetTaskItemTypeId(taskItemTypeId);
            data::TaskItemTypeData taskItemTypeData;
            auto taskItemType = taskItemTypeData.GetById(taskItemTypeId);
            taskItem->SetTaskItemType(std::move(taskItemType));

            taskItem->SetProjectId(projectId);
            data::ProjectData projectData(pConnection);
            auto project = projectData.GetById(projectId);
            taskItem->SetProject(std::move(project));

            taskItem->SetCategoryId(categoryId);
            data::CategoryData categoryData(pConnection);
            auto category = categoryData.GetById(categoryId);
            taskItem->SetCategory(std::move(category));

            taskItem->SetTaskId(taskId);
            data::TaskData taskData(pConnection);
            auto task = taskData.GetById(taskId);
            taskItem->SetTask(std::move(task));

            taskItems.push_back(std::move(taskItem));
        };

    return taskItems;
}

std::vector<wxString> TaskItemData::GetHours(const wxString& date)
{
    std::vector<wxString> taskDurations;

    *pConnection->DatabaseExecutableHandle() << TaskItemData::getTaskHoursByTaskId << date.ToStdString() >>
        [&](std::string duration) { taskDurations.push_back(wxString(duration)); };

    return taskDurations;
}

int TaskItemData::GetTaskItemTypeIdByTaskItemId(const int taskItemId)
{
    int taskItemTypeId = 0;

    *pConnection->DatabaseExecutableHandle() << TaskItemData::getTaskItemTypeIdByTaskItemId << taskItemId >>
        [&](int taskItemType) { taskItemTypeId = taskItemType; };
    return taskItemTypeId;
}

std::vector<std::unique_ptr<model::TaskItemModel>> TaskItemData::GetByWeek(const wxString& fromDate,
    const wxString& toDate)
{
    std::vector<std::unique_ptr<model::TaskItemModel>> taskItems;
    std::vector<int> taskItemIds;

    *pConnection->DatabaseExecutableHandle()
            << TaskItemData::getTaskItemsByWeek << fromDate.ToStdString() << toDate.ToStdString() >>
        [&](int taskItemId) { taskItemIds.push_back(taskItemId); };

    for (auto taskItemId : taskItemIds) {
        taskItems.push_back(GetById(taskItemId));
    }

    return taskItems;
}

wxString TaskItemData::GetDescriptionById(const int taskItemId)
{
    wxString rDescription = wxGetEmptyString();
    *pConnection->DatabaseExecutableHandle() << TaskItemData::getDescriptionById << taskItemId >>
        [&](std::string description) { rDescription = wxString(description); };
    return rDescription;
}

std::vector<wxString> TaskItemData::GetHoursByWeek(const wxString& fromDate, const wxString& toDate)
{
    std::vector<wxString> taskDurations;

    *pConnection->DatabaseExecutableHandle()
            << TaskItemData::getTaskHoursByWeek << fromDate.ToStdString() << toDate.ToStdString() >>
        [&](std::string duration) { taskDurations.push_back(wxString(duration)); };

    return taskDurations;
}

const std::string TaskItemData::createTaskItem = "INSERT INTO task_items "
                                                 "(start_time, end_time, duration, description, "
                                                 "billable, calculated_rate, is_active, "
                                                 "task_item_type_id, project_id, category_id, task_id) "
                                                 "VALUES (?, ?, ?, ?, ?, ?, 1, ?, ?, ?, ?)";

const std::string TaskItemData::getTaskItemById = "SELECT task_items.task_item_id, "
                                                  "task_items.start_time, "
                                                  "task_items.end_time, "
                                                  "task_items.duration, "
                                                  "task_items.description as description, "
                                                  "task_items.billable, "
                                                  "task_items.calculated_rate, "
                                                  "task_items.date_created, "
                                                  "task_items.date_modified, "
                                                  "task_items.is_active, "
                                                  "task_items.task_item_type_id, "
                                                  "task_items.project_id, "
                                                  "task_items.category_id,"
                                                  "task_items.task_id "
                                                  "FROM task_items "
                                                  "WHERE task_item_id = ?";

const std::string TaskItemData::updateTaskItem = "UPDATE task_items "
                                                 "SET start_time = ?, end_time = ?, duration = ?, "
                                                 "description = ?, billable = ?, calculated_rate = ?, "
                                                 "date_modified = ?, "
                                                 "project_id = ?, category_id = ? "
                                                 "WHERE task_item_id = ?";

const std::string TaskItemData::deleteTaskItem = "UPDATE task_items "
                                                 "SET is_active = 0, date_modified = ? "
                                                 "WHERE task_item_id = ?";

const std::string TaskItemData::getTaskItemsByDate =
    "SELECT task_items.task_item_id, "
    "tasks.task_date, "
    "task_items.start_time, "
    "task_items.end_time, "
    "task_items.duration, "
    "task_items.description as description, "
    "task_items.billable, "
    "task_items.calculated_rate, "
    "task_items.date_created, "
    "task_items.date_modified, "
    "task_items.is_active, "
    "task_items.task_item_type_id, "
    "task_items.project_id, "
    "task_items.category_id,"
    "task_items.task_id "
    "FROM task_items "
    "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
    "INNER JOIN categories ON task_items.category_id = categories.category_id "
    "INNER JOIN projects ON task_items.project_id = projects.project_id "
    "INNER JOIN task_item_types ON task_items.task_item_type_id = task_item_types.task_item_type_id "
    "WHERE task_date = ? "
    "AND task_items.is_active = 1";

const std::string TaskItemData::getTaskHoursByTaskId = "SELECT task_items.duration "
                                                       "FROM task_items "
                                                       "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
                                                       "WHERE task_date = ?";

const std::string TaskItemData::getTaskItemTypeIdByTaskItemId = "SELECT task_items.task_item_type_id "
                                                                "FROM task_items "
                                                                "WHERE task_item_id = ?";

const std::string TaskItemData::getTaskItemsByWeek = "SELECT task_items.task_item_id "
                                                     "FROM task_items "
                                                     "INNER JOIN tasks "
                                                     "ON task_items.task_id = tasks.task_id "
                                                     "WHERE tasks.task_date >= ? "
                                                     "AND tasks.task_date <= ? "
                                                     "AND task_items.is_active = 1";

const std::string TaskItemData::getDescriptionById = "SELECT description "
                                                     "FROM task_items "
                                                     "WHERE task_item_id = ?";

const std::string TaskItemData::getTaskHoursByWeek = "SELECT task_items.duration "
                                                     "FROM task_items "
                                                     "INNER JOIN tasks "
                                                     "ON task_items.task_id = tasks.task_id "
                                                     "WHERE tasks.task_date >= ? "
                                                     "AND tasks.task_date <= ? "
                                                     "AND task_items.is_active = 1";
} // namespace app::data

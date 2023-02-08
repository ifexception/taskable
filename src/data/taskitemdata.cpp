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

#include "taskitemdata.h"

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
}

TaskItemData::~TaskItemData()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
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

    ps << false << nullptr;

    ps << taskItem->GetTaskItemTypeId() << taskItem->GetProjectId() << taskItem->GetCategoryId()
       << taskItem->GetTaskId();

    if (taskItem->GetMeetingId() != nullptr) {
        ps << *taskItem->GetMeetingId();
    } else {
        ps << nullptr;
    }

    ps.execute();

    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

std::unique_ptr<model::TaskItemModel> TaskItemData::GetById(const int taskItemId)
{
    std::unique_ptr<model::TaskItemModel> taskItem = nullptr;

    *pConnection->DatabaseExecutableHandle() << TaskItemData::getTaskItemById << taskItemId >>
        [&](int taskItemsTaskItemId,
            std::unique_ptr<std::string> taskItemsStartTime,
            std::unique_ptr<std::string> taskItemsEndTime,
            std::string taskItemsDuration,
            std::string taskItemsDescription,
            int taskItemsDateCreated,
            int taskItemsDateModified,
            bool taskItemsIsActive,
            int taskItemsTaskItemTypeId,
            int taskItemsProjectId,
            int taskItemsCategoryId,
            int taskItemsTaskId,
            std::unique_ptr<int64_t> taskItemsMeetingId,
            int taskItemTypesTaskItemTypeId,
            std::string taskItemTypesName,
            int projectsProjectId,
            std::string projectsName,
            std::string projectsDisplayName,
            int projectsIsDefault,
            int projectsDateCreated,
            int projectsDateModified,
            int projectsIsActive,
            int projectsEmployerId,
            std::unique_ptr<int> projectsClientId,
            int categoriesCategoryId,
            std::string categoriesName,
            unsigned int categoriesColor,
            int categoriesDateCreated,
            int categoriesDateModified,
            int categoriesIsActive,
            int categoriesProjectId,
            int tasksTaskId,
            std::string tasksDate,
            int tasksDateCreated,
            int tasksDateModified,
            bool tasksIsActive,
            std::unique_ptr<int> meetingsMeetingId,
            std::unique_ptr<bool> meetingsAttended,
            std::unique_ptr<int> meetingsDuration,
            std::unique_ptr<std::string> meetingsStarting,
            std::unique_ptr<std::string> meetingsEnding,
            std::unique_ptr<std::string> meetingsLocation,
            std::unique_ptr<std::string> meetingsSubject,
            std::unique_ptr<std::string> meetingsBody,
            std::unique_ptr<int> meetingsDateCreated,
            std::unique_ptr<int> meetingsDateModified,
            std::unique_ptr<bool> meetingsIsActive,
            std::unique_ptr<int> meetingsTaskId) {
            taskItem = std::make_unique<model::TaskItemModel>(taskItemsTaskItemId,
                taskItemsDuration,
                taskItemsDescription,
                taskItemsDateCreated,
                taskItemsDateModified,
                taskItemsIsActive);

            if (taskItemsStartTime == nullptr && taskItemsEndTime == nullptr) {
                taskItem->SetDurationTime(wxString(taskItemsDuration));
            }

            if (taskItemsStartTime != nullptr && taskItemsEndTime != nullptr) {
                taskItem->SetStartTime(wxString(*taskItemsStartTime));
                taskItem->SetEndTime(wxString(*taskItemsEndTime));
            }

            taskItem->SetTaskItemTypeId(taskItemsTaskItemTypeId);

            auto taskItemType =
                std::make_unique<model::TaskItemTypeModel>(taskItemTypesTaskItemTypeId, wxString(taskItemTypesName));
            taskItem->SetTaskItemType(std::move(taskItemType));

            taskItem->SetProjectId(taskItemsProjectId);

            auto project = std::make_unique<model::ProjectModel>(projectsProjectId,
                wxString(projectsName),
                wxString(projectsDisplayName),
                projectsIsDefault,
                projectsDateCreated,
                projectsDateModified,
                projectsIsActive);

            project->SetEmployerId(projectsEmployerId);

            if (projectsClientId != nullptr) {
                project->SetClientId(*projectsClientId);
            }

            taskItem->SetProject(std::move(project));

            taskItem->SetCategoryId(taskItemsCategoryId);

            auto category = std::make_unique<model::CategoryModel>(categoriesCategoryId,
                categoriesName,
                categoriesColor,
                categoriesDateCreated,
                categoriesDateModified,
                categoriesIsActive);
            taskItem->SetCategory(std::move(category));

            taskItem->SetTaskId(taskItemsTaskId);

            auto task = std::make_unique<model::TaskModel>(
                tasksTaskId, wxString(tasksDate), tasksDateCreated, tasksDateModified, tasksIsActive);
            taskItem->SetTask(std::move(task));

            if (taskItemsMeetingId != nullptr) {
                taskItem->SetMeetingId(std::move(taskItemsMeetingId));

                auto meeting = std::make_unique<model::MeetingModel>(*meetingsMeetingId,
                    *meetingsDuration,
                    *meetingsLocation,
                    *meetingsSubject,
                    *meetingsBody,
                    *meetingsDateCreated,
                    *meetingsDateModified,
                    *meetingsIsActive);

                meeting->SetStart(wxString(*meetingsStarting));
                meeting->SetEnd(wxString(*meetingsEnding));

                meeting->SetTaskId(*meetingsTaskId);

                if (meetingsAttended != nullptr) {
                    meeting->Attended(std::move(meetingsAttended));
                }

                taskItem->SetMeeting(std::move(meeting));
            }
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

    ps << false << nullptr;

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

    *pConnection->DatabaseExecutableHandle() << TaskItemData::getTaskItemsByDate << date.ToStdString() >>
        [&](int taskItemsTaskItemId,
            std::unique_ptr<std::string> taskItemsStartTime,
            std::unique_ptr<std::string> taskItemsEndTime,
            std::string taskItemsDuration,
            std::string taskItemsDescription,
            int taskItemsDateCreated,
            int taskItemsDateModified,
            bool taskItemsIsActive,
            int taskItemsTaskItemTypeId,
            int taskItemsProjectId,
            int taskItemsCategoryId,
            int taskItemsTaskId,
            std::unique_ptr<int64_t> taskItemsMeetingId,
            int taskItemTypesTaskItemTypeId,
            std::string taskItemTypesName,
            int projectsProjectId,
            std::string projectsName,
            std::string projectsDisplayName,
            int projectsIsDefault,
            int projectsDateCreated,
            int projectsDateModified,
            int projectsIsActive,
            int projectsEmployerId,
            std::unique_ptr<int> projectsClientId,
            int categoriesCategoryId,
            std::string categoriesName,
            unsigned int categoriesColor,
            int categoriesDateCreated,
            int categoriesDateModified,
            int categoriesIsActive,
            int categoriesProjectId,
            int tasksTaskId,
            std::string tasksDate,
            int tasksDateCreated,
            int tasksDateModified,
            bool tasksIsActive,
            std::unique_ptr<int> meetingsMeetingId,
            std::unique_ptr<bool> meetingsAttended,
            std::unique_ptr<int> meetingsDuration,
            std::unique_ptr<std::string> meetingsStarting,
            std::unique_ptr<std::string> meetingsEnding,
            std::unique_ptr<std::string> meetingsLocation,
            std::unique_ptr<std::string> meetingsSubject,
            std::unique_ptr<std::string> meetingsBody,
            std::unique_ptr<int> meetingsDateCreated,
            std::unique_ptr<int> meetingsDateModified,
            std::unique_ptr<bool> meetingsIsActive,
            std::unique_ptr<int> meetingsTaskId) {
            auto taskItem = std::make_unique<model::TaskItemModel>(taskItemsTaskItemId,
                taskItemsDuration,
                taskItemsDescription,
                taskItemsDateCreated,
                taskItemsDateModified,
                taskItemsIsActive);

            if (taskItemsStartTime == nullptr && taskItemsEndTime == nullptr) {
                taskItem->SetDurationTime(wxString(taskItemsDuration));
            }

            if (taskItemsStartTime != nullptr && taskItemsEndTime != nullptr) {
                taskItem->SetStartTime(wxString(*taskItemsStartTime));
                taskItem->SetEndTime(wxString(*taskItemsEndTime));
            }

            taskItem->SetTaskItemTypeId(taskItemsTaskItemTypeId);

            auto taskItemType =
                std::make_unique<model::TaskItemTypeModel>(taskItemTypesTaskItemTypeId, wxString(taskItemTypesName));
            taskItem->SetTaskItemType(std::move(taskItemType));

            taskItem->SetProjectId(taskItemsProjectId);

            auto project = std::make_unique<model::ProjectModel>(projectsProjectId,
                wxString(projectsName),
                wxString(projectsDisplayName),
                projectsIsDefault,
                projectsDateCreated,
                projectsDateModified,
                projectsIsActive);

            project->SetEmployerId(projectsEmployerId);

            if (projectsClientId != nullptr) {
                project->SetClientId(*projectsClientId);
            }
            taskItem->SetProject(std::move(project));

            taskItem->SetCategoryId(taskItemsCategoryId);

            auto category = std::make_unique<model::CategoryModel>(categoriesCategoryId,
                categoriesName,
                categoriesColor,
                categoriesDateCreated,
                categoriesDateModified,
                categoriesIsActive);
            taskItem->SetCategory(std::move(category));

            taskItem->SetTaskId(taskItemsTaskId);

            auto task = std::make_unique<model::TaskModel>(
                tasksTaskId, wxString(tasksDate), tasksDateCreated, tasksDateModified, tasksIsActive);
            taskItem->SetTask(std::move(task));

            if (taskItemsMeetingId != nullptr) {
                taskItem->SetMeetingId(std::move(taskItemsMeetingId));

                auto meeting = std::make_unique<model::MeetingModel>(*meetingsMeetingId,
                    *meetingsDuration,
                    *meetingsLocation,
                    *meetingsSubject,
                    *meetingsBody,
                    *meetingsDateCreated,
                    *meetingsDateModified,
                    *meetingsIsActive);

                meeting->SetStart(wxString(*meetingsStarting));
                meeting->SetEnd(wxString(*meetingsEnding));

                meeting->SetTaskId(*meetingsTaskId);

                if (meetingsAttended != nullptr) {
                    meeting->Attended(std::move(meetingsAttended));
                }

                taskItem->SetMeeting(std::move(meeting));
            }

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

void TaskItemData::UpdateTaskItemWithMeetingId(const int64_t taskItemId, const int64_t meetingId)
{
    *pConnection->DatabaseExecutableHandle() << TaskItemData::updateTaskItemWithMeetingId << meetingId << taskItemId;
}

const std::string TaskItemData::createTaskItem = "INSERT INTO task_items "
                                                 "(start_time, end_time, duration, description, "
                                                 "billable, calculated_rate, is_active, "
                                                 "task_item_type_id, project_id, category_id, task_id, meeting_id) "
                                                 "VALUES (?, ?, ?, ?, ?, ?, 1, ?, ?, ?, ?, ?)";

const std::string TaskItemData::getTaskItemById = "SELECT "
                                                  "  task_items.task_item_id "
                                                  ", task_items.start_time "
                                                  ", task_items.end_time "
                                                  ", task_items.duration "
                                                  ", task_items.description "
                                                  ", task_items.date_created "
                                                  ", task_items.date_modified "
                                                  ", task_items.is_active "
                                                  ", task_items.task_item_type_id "
                                                  ", task_items.project_id "
                                                  ", task_items.category_id "
                                                  ", task_items.task_id "
                                                  ", task_items.meeting_id "
                                                  ", task_item_types.task_item_type_id "
                                                  ", task_item_types.name "
                                                  ", projects.project_id "
                                                  ", projects.name "
                                                  ", projects.display_name "
                                                  ", projects.is_default "
                                                  ", projects.date_created "
                                                  ", projects.date_modified "
                                                  ", projects.is_active "
                                                  ", projects.employer_id "
                                                  ", projects.client_id "
                                                  ", categories.category_id "
                                                  ", categories.name "
                                                  ", categories.color "
                                                  ", categories.date_created "
                                                  ", categories.date_modified "
                                                  ", categories.is_active "
                                                  ", categories.project_id "
                                                  ", tasks.task_id "
                                                  ", tasks.task_date "
                                                  ", tasks.date_created "
                                                  ", tasks.date_modified "
                                                  ", tasks.is_active "
                                                  ", meetings.meeting_id "
                                                  ", meetings.attended "
                                                  ", meetings.duration "
                                                  ", meetings.starting "
                                                  ", meetings.ending "
                                                  ", meetings.location "
                                                  ", meetings.subject "
                                                  ", meetings.body "
                                                  ", meetings.date_created "
                                                  ", meetings.date_modified "
                                                  ", meetings.is_active "
                                                  ", meetings.task_id "
                                                  "FROM task_items "
                                                  "INNER JOIN task_item_types "
                                                  "ON task_items.task_item_type_id = task_item_types.task_item_type_id "
                                                  "INNER JOIN projects "
                                                  "ON task_items.project_id = projects.project_id "
                                                  "INNER JOIN categories "
                                                  "ON task_items.category_id = categories.category_id "
                                                  "INNER JOIN tasks "
                                                  "ON task_items.task_id = tasks.task_id "
                                                  "LEFT JOIN meetings "
                                                  "ON task_items.meeting_id = meetings.meeting_id "
                                                  "WHERE task_item_id = ?;";

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
    "SELECT "
    "  task_items.task_item_id "
    ", task_items.start_time "
    ", task_items.end_time "
    ", task_items.duration "
    ", task_items.description "
    ", task_items.date_created "
    ", task_items.date_modified "
    ", task_items.is_active "
    ", task_items.task_item_type_id "
    ", task_items.project_id "
    ", task_items.category_id "
    ", task_items.task_id "
    ", task_items.meeting_id "
    ", task_item_types.task_item_type_id "
    ", task_item_types.name "
    ", projects.project_id "
    ", projects.name "
    ", projects.display_name "
    ", projects.is_default "
    ", projects.date_created "
    ", projects.date_modified "
    ", projects.is_active "
    ", projects.employer_id "
    ", projects.client_id "
    ", categories.category_id "
    ", categories.name "
    ", categories.color "
    ", categories.date_created "
    ", categories.date_modified "
    ", categories.is_active "
    ", categories.project_id "
    ", tasks.task_id "
    ", tasks.task_date "
    ", tasks.date_created "
    ", tasks.date_modified "
    ", tasks.is_active "
    ", meetings.meeting_id "
    ", meetings.attended "
    ", meetings.duration "
    ", meetings.starting "
    ", meetings.ending "
    ", meetings.location "
    ", meetings.subject "
    ", meetings.body "
    ", meetings.date_created "
    ", meetings.date_modified "
    ", meetings.is_active "
    ", meetings.task_id "
    "FROM task_items "
    "INNER JOIN task_item_types "
    "ON task_items.task_item_type_id = task_item_types.task_item_type_id "
    "INNER JOIN projects "
    "ON task_items.project_id = projects.project_id "
    "INNER JOIN categories "
    "ON task_items.category_id = categories.category_id "
    "INNER JOIN tasks "
    "ON task_items.task_id = tasks.task_id "
    "LEFT JOIN meetings "
    "ON task_items.meeting_id = meetings.meeting_id "
    "WHERE task_date = ? "
    "AND task_items.is_active = 1";

const std::string TaskItemData::getTaskHoursByTaskId = "SELECT task_items.duration "
                                                       "FROM task_items "
                                                       "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
                                                       "WHERE task_date = ?"
                                                       "AND task_items.is_active = 1";

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

const std::string TaskItemData::updateTaskItemWithMeetingId = "UPDATE task_items "
                                                              "SET meeting_id = ? "
                                                              "WHERE task_item_id = ?";
} // namespace app::data

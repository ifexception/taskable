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

#include "taskitemmodel.h"

#include "../common/util.h"
#include "../services/databaseconnection.h"

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

TaskItemTypeModel::TaskItemTypeModel(const int taskItemTypeId, bool initializeFromDatabase)
    : TaskItemTypeModel()
{
    assert(initializeFromDatabase == true);
    auto taskItemType = TaskItemTypeModel::GetById(taskItemTypeId);
    mTaskItemTypeId = taskItemType->GetTaskItemTypeId();
    mName = taskItemType->GetName();
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

std::unique_ptr<TaskItemTypeModel> TaskItemTypeModel::GetById(const int taskItemTypeId)
{
    std::unique_ptr<TaskItemTypeModel> taskItemType = nullptr;
    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << TaskItemTypeModel::getTaskItemTypeById << taskItemTypeId >> [&](int taskItemTypeId, std::string name) {
        taskItemType = std::make_unique<TaskItemTypeModel>(taskItemTypeId, wxString(name));
    };

    return std::move(taskItemType);
}

std::vector<std::unique_ptr<TaskItemTypeModel>> TaskItemTypeModel::GetAll()
{
    std::vector<std::unique_ptr<TaskItemTypeModel>> taskItemTypes;
    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << TaskItemTypeModel::getTaskItemTypes >> [&](int taskItemTypeId, std::string name) {
        auto taskItemType = std::make_unique<TaskItemTypeModel>(taskItemTypeId, wxString(name));
        taskItemTypes.push_back(std::move(taskItemType));
    };

    return taskItemTypes;
}

const std::string TaskItemTypeModel::getTaskItemTypeById = "SELECT task_item_type_id, "
                                                           "name "
                                                           "FROM task_item_types "
                                                           "WHERE task_item_type_id = ?";

const std::string TaskItemTypeModel::getTaskItemTypes = "SELECT task_item_type_id, "
                                                        "name "
                                                        "FROM task_item_types ";

TaskItemModel::TaskItemModel()
    : mTaskItemId(-1)
    , pStartTime(nullptr)
    , pEndTime(nullptr)
    , mDuration(wxGetEmptyString())
    , mDescription(wxGetEmptyString())
    , bBillable(false)
    , pCalculatedRate(nullptr)
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
    , mTaskItemTypeId(-1)
    , mProjectId(-1)
    , mCategoryId(-1)
    , mTaskId(-1)
    , pTaskItemType(nullptr)
    , pProject(nullptr)
    , pCategory(nullptr)
    , pTask(nullptr)
{
}

TaskItemModel::TaskItemModel(int taskItemId)
    : TaskItemModel()
{
    mTaskItemId = taskItemId;
}

TaskItemModel::TaskItemModel(int taskItemId, bool initializeFromDatabase)
    : TaskItemModel()
{
}

TaskItemModel::TaskItemModel(int taskItemId,
    wxString duration,
    wxString description,
    bool billable,
    int dateCreated,
    int dateModified,
    bool isActive)
    : TaskItemModel()
{
    mTaskItemId = taskItemId;
    mDuration = duration;
    mDescription = description;
    bBillable = billable;
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

bool TaskItemModel::IsEntryTask()
{
    return pStartTime == nullptr && pEndTime == nullptr &&
           mTaskItemTypeId == static_cast<int>(constants::TaskItemTypes::EntryTask);
}

bool TaskItemModel::IsTimedTask()
{
    return pStartTime != nullptr && pEndTime != nullptr &&
           mTaskItemTypeId == static_cast<int>(constants::TaskItemTypes::TimedTask);
}

const int TaskItemModel::GetTaskItemId() const
{
    return mTaskItemId;
}

const wxDateTime* TaskItemModel::GetStartTime() const
{
    return pStartTime.get();
}

const wxDateTime* TaskItemModel::GetEndTime() const
{
    return pEndTime.get();
}

const wxDateTime* TaskItemModel::GetDurationTime() const
{
    return pDurationTime.get();
}

const wxString TaskItemModel::GetDuration() const
{
    return mDuration;
}

const wxString TaskItemModel::GetDescription() const
{
    return mDescription;
}

const bool TaskItemModel::IsBillable() const
{
    return bBillable;
}

const double* TaskItemModel::GetCalculatedRate() const
{
    return pCalculatedRate.get();
}

const wxDateTime TaskItemModel::GetDateCreated()
{
    return mDateCreated;
}

const wxDateTime TaskItemModel::GetDateModified()
{
    return mDateModified;
}

const bool TaskItemModel::IsActive() const
{
    return bIsActive;
}

const int TaskItemModel::GetTaskItemTypeId() const
{
    return mTaskItemTypeId;
}

const int TaskItemModel::GetProjectId() const
{
    return mProjectId;
}

const int TaskItemModel::GetCategoryId() const
{
    return mCategoryId;
}

const int TaskItemModel::GetTaskId() const
{
    return mTaskId;
}

TaskItemTypeModel* TaskItemModel::GetTaskItemType()
{
    return pTaskItemType.get();
}

ProjectModel* TaskItemModel::GetProject()
{
    return pProject.get();
}

CategoryModel* TaskItemModel::GetCategory()
{
    return pCategory.get();
}

TaskModel* TaskItemModel::GetTask()
{
    return pTask.get();
}

void TaskItemModel::SetTaskItemId(const int taskItemId)
{
    mTaskItemId = taskItemId;
}

void TaskItemModel::SetStartTime(std::unique_ptr<wxDateTime> startTime)
{
    pStartTime = std::move(startTime);
}

void TaskItemModel::SetEndTime(std::unique_ptr<wxDateTime> endTime)
{
    pEndTime = std::move(endTime);
}

void TaskItemModel::SetDurationTime(std::unique_ptr<wxDateTime> durationTime)
{
    pDurationTime = std::move(durationTime);
}

void TaskItemModel::SetStartTime(const wxString& startTime)
{
    wxDateTime startDateTime;
    startDateTime.ParseISOTime(startTime);
    pStartTime = std::make_unique<wxDateTime>(startDateTime);
}

void TaskItemModel::SetEndTime(const wxString& endTime)
{
    wxDateTime endDateTime;
    endDateTime.ParseISOTime(endTime);
    pEndTime = std::make_unique<wxDateTime>(endDateTime);
}

void TaskItemModel::SetDurationTime(const wxString& durationTime)
{
    wxDateTime durationDateTime;
    durationDateTime.ParseISOTime(durationTime);
    pDurationTime = std::make_unique<wxDateTime>(durationDateTime);
}

void TaskItemModel::SetDuration(const wxString& duration)
{
    mDuration = duration;
}

void TaskItemModel::SetDescription(const wxString& description)
{
    mDescription = description;
}

void TaskItemModel::IsBillable(const bool billable)
{
    bBillable = billable;
}

void TaskItemModel::SetCalculatedRate(std::unique_ptr<double> calculatedRate)
{
    pCalculatedRate = std::move(calculatedRate);
}

void TaskItemModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void TaskItemModel::SetDateUpdated(const wxDateTime& dateModified)
{
    mDateModified = dateModified;
}

void TaskItemModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}

void TaskItemModel::SetTaskItemTypeId(const int taskItemTypeId)
{
    mTaskItemTypeId = taskItemTypeId;
}

void TaskItemModel::SetProjectId(const int projectId)
{
    mProjectId = projectId;
}

void TaskItemModel::SetCategoryId(const int categoryId)
{
    mCategoryId = categoryId;
}

void TaskItemModel::SetTaskId(const int taskId)
{
    mTaskId = taskId;
}

void TaskItemModel::SetTaskItemType(std::unique_ptr<TaskItemTypeModel> taskItemType)
{
    pTaskItemType = std::move(taskItemType);
}

void TaskItemModel::SetProject(std::unique_ptr<ProjectModel> projcet)
{
    pProject = std::move(projcet);
}

void TaskItemModel::SetCategory(std::unique_ptr<CategoryModel> category)
{
    pCategory = std::move(category);
}

void TaskItemModel::SetTask(std::unique_ptr<TaskModel> task)
{
    pTask = std::move(task);
}

void TaskItemModel::Create(std::unique_ptr<TaskItemModel> taskItem)
{
    auto db = svc::DatabaseConnection::Get().GetHandle();
    auto ps = *db << TaskItemModel::createTaskItem;

    if (taskItem->IsEntryTask()) {
        ps << nullptr << nullptr;
    }
    if (taskItem->IsTimedTask()) {
        ps << taskItem->GetStartTime()->FormatISOTime().ToStdString()
           << taskItem->GetEndTime()->FormatISOTime().ToStdString();
    }

    ps << taskItem->GetDuration().ToStdString() << taskItem->GetDescription().ToStdString();

    taskItem->SetProject(std::move(ProjectModel::GetById(taskItem->GetProjectId())));
    if (taskItem->GetProject()->IsNonBillableScenario()) {
        ps << taskItem->IsBillable() << nullptr;
    }

    if (taskItem->GetProject()->IsBillableWithUnknownRateScenario()) {
        ps << taskItem->IsBillable() << nullptr;
    }

    if (taskItem->GetProject()->IsBillableScenarioWithHourlyRate()) {
        ps << taskItem->IsBillable() << *taskItem->GetCalculatedRate();
    }

    if (taskItem->GetProject()->IsBillableScenarioWithDailyRate()) {
        ps << taskItem->IsBillable() << nullptr;
    }

    ps << taskItem->GetTaskItemTypeId() << taskItem->GetProjectId() << taskItem->GetCategoryId()
       << taskItem->GetTaskId();

    ps.execute();
}

std::unique_ptr<TaskItemModel> TaskItemModel::GetById(const int taskItemId)
{
    std::unique_ptr<TaskItemModel> taskItem = nullptr;
    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << TaskItemModel::getTaskItemById << taskItemId >> [&](int taskItemId,
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
        taskItem = std::make_unique<TaskItemModel>(
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
        auto taskItemType = TaskItemTypeModel::GetById(taskItemTypeId);
        taskItem->SetTaskItemType(std::move(taskItemType));

        taskItem->SetProjectId(projectId);
        auto project = ProjectModel::GetById(projectId);
        taskItem->SetProject(std::move(project));

        taskItem->SetCategoryId(categoryId);
        auto category = CategoryModel::GetById(categoryId);
        taskItem->SetCategory(std::move(category));

        taskItem->SetTaskId(taskId);
        auto task = TaskModel::GetById(taskId);
        taskItem->SetTask(std::move(task));
    };

    return std::move(taskItem);
}

void TaskItemModel::Update(std::unique_ptr<TaskItemModel> taskItem)
{
    auto db = svc::DatabaseConnection::Get().GetHandle();
    auto ps = *db << TaskItemModel::updateTaskItem;

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

    if (taskItem->GetProject()->IsBillableScenarioWithDailyRate()) {
        ps << taskItem->IsBillable() << nullptr;
    }

    ps << util::UnixTimestamp();

    ps << taskItem->GetProjectId() << taskItem->GetCategoryId();

    ps << taskItem->GetTaskItemId();

    ps.execute();
}

void TaskItemModel::Delete(std::unique_ptr<TaskItemModel> taskItem)
{
    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << TaskItemModel::deleteTaskItem << util::UnixTimestamp() << taskItem->GetTaskItemId();
}

std::vector<std::unique_ptr<TaskItemModel>> TaskItemModel::GetByDate(const wxString& date)
{
    std::vector<std::unique_ptr<TaskItemModel>> taskItems;

    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << TaskItemModel::getTaskItemsByDate << date >> [&](int taskItemId,
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
        auto taskItem = std::make_unique<TaskItemModel>(
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
        auto taskItemType = TaskItemTypeModel::GetById(taskItemTypeId);
        taskItem->SetTaskItemType(std::move(taskItemType));

        taskItem->SetProjectId(projectId);
        auto project = ProjectModel::GetById(projectId);
        taskItem->SetProject(std::move(project));

        taskItem->SetCategoryId(categoryId);
        auto category = CategoryModel::GetById(categoryId);
        taskItem->SetCategory(std::move(category));

        taskItem->SetTaskId(taskId);
        auto task = TaskModel::GetById(taskId);
        taskItem->SetTask(std::move(task));

        taskItems.push_back(std::move(taskItem));
    };

    return taskItems;
}

std::vector<wxString> TaskItemModel::GetHours(const wxString& date)
{
    std::vector<wxString> taskDurations;

    auto db = svc::DatabaseConnection::Get().GetHandle();
    *db << TaskItemModel::getTaskHoursByTaskId << date >>
        [&](std::string duration) { taskDurations.push_back(wxString(duration)); };

    return taskDurations;
}

int TaskItemModel::GetTaskItemTypeIdByTaskItemId(const int taskItemId)
{
    int taskItemTypeId = 0;
    auto db = svc::DatabaseConnection::Get().GetHandle();

    *db << TaskItemModel::getTaskItemTypeIdByTaskItemId << taskItemId >>
        [&](int taskItemType) { taskItemTypeId = taskItemType; };
    return taskItemTypeId;
}

const std::string TaskItemModel::createTaskItem = "INSERT INTO task_items "
                                                  "(start_time, end_time, duration, description, "
                                                  "billable, calculated_rate, is_active, "
                                                  "task_item_type_id, project_id, category_id, task_id) "
                                                  "VALUES (?, ?, ?, ?, ?, ?, 1, ?, ?, ?, ?)";

const std::string TaskItemModel::getTaskItemById = "SELECT task_items.task_item_id, "
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

const std::string TaskItemModel::updateTaskItem = "UPDATE task_items "
                                                  "SET start_time = ?, end_time = ?, duration = ?, "
                                                  "description = ?, billable = ?, calculated_rate = ?, "
                                                  "date_modified = ?, "
                                                  "project_id = ?, category_id = ? "
                                                  "WHERE task_item_id = ?";

const std::string TaskItemModel::deleteTaskItem = "UPDATE task_items "
                                                  "SET is_active = 0, date_modified_utc = ? "
                                                  "WHERE task_item_id = ?";

const std::string TaskItemModel::getTaskItemsByDate =
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
    "WHERE task_date = ?";

const std::string TaskItemModel::getTaskHoursByTaskId = "SELECT task_items.duration "
                                                        "FROM task_items "
                                                        "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
                                                        "WHERE task_date = ?";

const std::string TaskItemModel::getTaskItemTypeIdByTaskItemId = "SELECT task_items.task_item_type_id "
                                                                 "FROM task_items "
                                                                 "WHERE task_item_id = ?";
;
} // namespace app::model

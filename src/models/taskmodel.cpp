//  Taskable is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2019> <Szymon Welgus>
//
//  This program is free software : you can redistribute it and /
//  or modify it under the terms of the GNU General Public License as published
//  by the Free Software Foundation
//  , either version 3 of the License
//  , or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful
//  , but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "taskmodel.h"

#include "../common/util.h"
#include "../services/db_connection.h"

namespace app::model
{
TaskModel::TaskModel()
    : mTaskId(-1)
    , mTaskDate(wxDefaultDateTime)
    , mDateCreated(wxDefaultDateTime)
    , mDateModified(wxDefaultDateTime)
    , bIsActive(false)
{
}

TaskModel::TaskModel(int taskId, bool initializeFromDatabase)
    : TaskModel()
{
    assert(initializeFromDatabase == true);

    auto task = TaskModel::GetById(taskId);
}

TaskModel::TaskModel(int taskId, wxString date, int dateCreated, int dateModified, bool isActive)
    : TaskModel()
{
    mTaskId = taskId;
    mTaskDate.ParseISODate(date);
    mDateCreated = util::ToDateTime(dateCreated);
    mDateModified = util::ToDateTime(dateModified);
    bIsActive = isActive;
}

const int TaskModel::GetTaskId() const
{
    return mTaskId;
}

const wxDateTime TaskModel::GetTaskDate() const
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

void TaskModel::SetTaskDate(const wxDateTime& date)
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

int TaskModel::GetId(const wxDateTime date)
{
    int rTaskId = 0;
    bool taskDoesNotExistYet = true;
    auto db = services::db_connection::get_instance().get_handle();

    db << TaskModel::getTaskId << date.FormatISODate().ToStdString() >> [&](std::unique_ptr<int> taskId) {
        if (taskId != nullptr) {
            taskDoesNotExistYet = false;
            rTaskId = *taskId;
        }
    };

    if (taskDoesNotExistYet) {
        TaskModel::Create(date);
        rTaskId = db.last_insert_rowid();
    }
    return rTaskId;
}

std::unique_ptr<TaskModel> TaskModel::GetByDate(const wxDateTime date)
{
    std::unique_ptr<TaskModel> taskModel = nullptr;
    auto db = services::db_connection::get_instance().get_handle();
    int taskId = GetId(date);

    db << TaskModel::getTaskByDate << date.FormatISODate().ToStdString() >>
        [&](int taskId, std::string date, int dateCreated, int dateModified, bool isActive) {
            taskModel = std::make_unique<TaskModel>(taskId, wxString(date), dateCreated, dateModified, isActive);
        };

    return std::move(taskModel);
}

std::unique_ptr<TaskModel> TaskModel::GetById(const int taskId)
{
    std::unique_ptr<TaskModel> taskModel = nullptr;
    auto db = services::db_connection::get_instance().get_handle();
    db << TaskModel::getTaskById << taskId >>
        [&](int taskId, std::string date, int dateCreated, int dateModified, bool isActive) {
            taskModel = std::make_unique<TaskModel>(taskId, wxString(date), dateCreated, dateModified, isActive);
        };

    return std::move(taskModel);
}

void TaskModel::Create(const wxDateTime date)
{
    auto db = services::db_connection::get_instance().get_handle();
    db << TaskModel::createTask << date.FormatISODate().ToStdString();
}

const std::string TaskModel::getTaskId = "SELECT task_id "
                                         "FROM tasks "
                                         "WHERE task_date = ?";

const std::string TaskModel::getTaskByDate = "SELECT task_id, "
                                             "task_date, "
                                             "date_created, "
                                             "date_modified, "
                                             "is_active "
                                             "FROM tasks "
                                             "WHERE task_date = ?";

const std::string TaskModel::getTaskById = "SELECT task_id, "
                                           "task_date, "
                                           "date_created, "
                                           "date_modified, "
                                           "is_active "
                                           "FROM tasks "
                                           "WHERE task_id = ?";

const std::string TaskModel::createTask = "INSERT INTO "
                                          "tasks (task_date, is_active) "
                                          "VALUES (?, 1)";
} // namespace app::model

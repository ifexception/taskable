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

#include "task_item.h"

namespace app::models
{
const std::string task_item::createNewTaskItem =
    "INSERT INTO task_items (start_time, end_time, duration, description, "
    "billable, task_item_type_id, project_id, task_id, category_id, is_active) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, 1)";
const std::string task_item::getAllTaskItemsByDate =
    "SELECT task_items.task_item_id, tasks.task_date, "
    "task_items.start_time, "
    "task_items.end_time, "
    "task_items.duration, "
    "task_items.description as description, "
    "categories.name as category_name, "
    "categories.color as category_color, "
    "projects.display_name as project_name, "
    "task_item_types.task_item_type_id "
    "FROM task_items "
    "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
    "INNER JOIN categories ON task_items.category_id = categories.category_id "
    "INNER JOIN projects ON task_items.project_id = projects.project_id "
    "INNER JOIN task_item_types ON task_items.task_item_type_id = task_item_types.task_item_type_id "
    "WHERE task_date = ?";
const std::string task_item::getTaskItemById =
    "SELECT task_items.task_item_id, "
    "projects.project_id,"
    "projects.display_name as project_name, "
    "task_items.start_time, "
    "task_items.end_time, "
    "task_items.duration, "
    "categories.category_id, "
    "categories.name as category_name, "
    "task_items.description as description, "
    "task_items.billable, "
    "task_items.date_created_utc, "
    "task_items.date_modified_utc, "
    "task_items.is_active "
    "FROM task_items "
    "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
    "INNER JOIN categories ON task_items.category_id = categories.category_id "
    "INNER JOIN projects ON task_items.project_id = projects.project_id "
    "WHERE task_item_id = ?";
const std::string task_item::updateTaskItem = "UPDATE task_items SET start_time = ?, end_time = ?, duration = ?, "
                                              "description = ?, billable = ?, date_modified_utc = ?, "
                                              "project_id = ?, category_id = ? WHERE task_item_id = ?";
const std::string task_item::deleteTaskItem =
    "UPDATE task_items SET is_active = 0, date_modified_utc = ? WHERE task_item_id = ?";

const std::string task::getTaskId = "SELECT task_id FROM tasks WHERE task_date = ?";
const std::string task::createTask = "INSERT INTO tasks (task_date, is_active) VALUES (?, 1)";
const std::string task::getTaskHoursById = "SELECT task_items.duration "
                                           "FROM task_items "
                                           "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
                                           "WHERE task_date = ?";

const std::string task_context::getEmployerAndClientByProjectId = "SELECT employers.name as employer_name, "
                                                                  "clients.name as client_name "
                                                                  "FROM employers "
                                                                  "LEFT JOIN clients "
                                                                  "ON employers.employer_id = clients.employer_id "
                                                                  "INNER JOIN projects as p1 "
                                                                  "ON p1.employer_id = employers.employer_id "
                                                                  "LEFT JOIN projects as p2 "
                                                                  "ON p2.client_id = clients.client_id "
                                                                  "WHERE p1.project_id = ?";

task_item::task_item(int taskItemId,
    std::string taskDate,
    std::unique_ptr<std::string> startTime,
    std::unique_ptr<std::string> endTime,
    std::string duration,
    std::string description,
    std::string categoryName,
    int categoryColor,
    std::string projectName,
    int taskItemTypeId)
    : task_item_id(taskItemId)
    , task_date(taskDate)
    , start_time(nullptr)
    , end_time(nullptr)
    , duration(duration)
    , description(description)
    , category_name(categoryName)
    , category_color(categoryColor)
    , project_name(projectName)
    , task_item_type_id(taskItemTypeId)
{
    if (startTime && endTime) {
        start_time = new std::string(*startTime);
        end_time = new std::string(*endTime);
    }
}

task_item::task_item(int taskItemId,
    int projectId,
    std::string projectName,
    std::unique_ptr<std::string> startTime,
    std::unique_ptr<std::string> endTime,
    std::string duration,
    int categoryId,
    std::string categoryName,
    std::string description,
    int billable,
    int dateCreatedUtc,
    int dateModifiedUtc,
    int isActive)
    : task_item_id(taskItemId)
    , project_id(projectId)
    , project_name(projectName)
    , start_time()
    , end_time()
    , duration(duration)
    , category_id(categoryId)
    , category_name(categoryName)
    , description(description)
    , billable(billable)
    , date_created_utc(dateCreatedUtc)
    , date_modified_utc(dateModifiedUtc)
    , is_active(isActive)
{
    if (startTime && endTime) {
        start_time = new std::string(*startTime);
        end_time = new std::string(*endTime);
    }
}

void task_item::cleanup()
{
    if (start_time) {
        delete start_time;
    }
    if (end_time) {
        delete end_time;
    }
}

task_context::task_context(std::string employerName, std::unique_ptr<std::string> clientName)
    : employer_name(employerName)
    , client_name(nullptr)
{
    if (clientName) {
        client_name = new std::string(*clientName);
    }
}

void task_context::cleanup()
{
    if (client_name) {
        delete client_name;
    }
}
} // namespace app::models

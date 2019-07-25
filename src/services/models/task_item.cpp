//  TimesheetsTracker is a desktop that aids you in tracking your timesheets
//  and seeing what work you have done.
//
//  Copyright(C) <2018> <Szymon Welgus>
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
const std::string task_item::createNewTaskItem = "INSERT INTO task_items (start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES (?, ?, ?, ?, 1, ?, ?, ?)";
const std::string task_item::getAllTaskItemsByDate = "SELECT task_items.task_item_id, tasks.task_date, "
                                                     "task_items.start_time, "
                                                     "task_items.end_time, "
                                                     "task_items.duration, "
                                                     "task_items.description as description, "
                                                     "categories.name as category_name, "
                                                     "projects.display_name as project_name "
                                                     "FROM task_items "
                                                     "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
                                                     "INNER JOIN categories ON task_items.category_id = categories.category_id "
                                                     "INNER JOIN projects ON task_items.project_id = projects.project_id "
                                                     "WHERE task_date = ?";
const std::string task_item::getTaskItemById = "SELECT task_items.task_item_id, "
                                               "projects.project_id,"
                                               "projects.display_name as project_name, "
                                               "task_items.start_time, "
                                               "task_items.end_time, "
                                               "task_items.duration, "
                                               "categories.category_id, "
                                               "categories.name as category_name, "
                                               "task_items.description as description, "
                                               "task_items.date_created_utc, "
                                               "task_items.date_modified_utc, "
                                               "task_items.is_active "
                                               "FROM task_items "
                                               "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
                                               "INNER JOIN categories ON task_items.category_id = categories.category_id "
                                               "INNER JOIN projects ON task_items.project_id = projects.project_id "
                                               "WHERE task_item_id = ?";
const std::string task_item::updateTaskItem = "UPDATE task_items SET start_time = ?, end_time = ?, duration = ?, description = ?, date_modified_utc = ?, project_id = ?, category_id = ? WHERE task_item_id = ?";
const std::string task_item::deleteTaskItem = "UPDATE task_items SET is_active = 0, date_modified_utc = ? WHERE task_item_id = ?";

const std::string task::getTaskId = "SELECT task_id FROM tasks WHERE task_date = ?";
const std::string task::createTask = "INSERT INTO tasks (task_date, is_active, project_id) VALUES (?, 1, ?)";
const std::string task::getTaskHoursById = "SELECT task_items.duration "
                                           "FROM task_items "
                                           "INNER JOIN tasks ON task_items.task_id = tasks.task_id "
                                           "WHERE task_date = ?"
;
}

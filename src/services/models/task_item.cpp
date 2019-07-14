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
const std::string task_item::createNewTaskItem = "INSERT INTO task_details (start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES (?, ?, ?, ?, 1, ?, ?, ?)";
const std::string task_item::getAllTaskItemsByDate = "SELECT task_details.task_detail_id, tasks.task_date, "
                                                     "task_details.start_time, "
                                                     "task_details.end_time, "
                                                     "task_details.duration, "
                                                     "task_details.description as description, "
                                                     "categories.name as category_name, "
                                                     "projects.display_name as project_name "
                                                     "FROM task_details "
                                                     "INNER JOIN tasks ON task_details.task_id = tasks.task_id "
                                                     "INNER JOIN categories ON task_details.category_id = categories.category_id "
                                                     "INNER JOIN projects ON task_details.project_id = projects.project_id "
                                                     "WHERE task_date = ?";
const std::string task_item::getTaskItemById = "SELECT task_details.task_detail_id, "
                                               "projects.project_id,"
                                               "projects.display_name as project_name, "
                                               "task_details.start_time, "
                                               "task_details.end_time, "
                                               "task_details.duration, "
                                               "categories.category_id, "
                                               "categories.name as category_name, "
                                               "task_details.description as description, "
                                               "task_details.date_created_utc, "
                                               "task_details.date_modified_utc, "
                                               "task_details.is_active "
                                               "FROM task_details "
                                               "INNER JOIN tasks ON task_details.task_id = tasks.task_id "
                                               "INNER JOIN categories ON task_details.category_id = categories.category_id "
                                               "INNER JOIN projects ON task_details.project_id = projects.project_id "
                                               "WHERE task_detail_id = ?";
const std::string task_item::updateTaskItem = "UPDATE task_details SET start_time = ?, end_time = ?, duration = ?, description = ?, date_modified_utc = ?, project_id = ?, category_id = ? WHERE task_detail_id = ?";
const std::string task_item::deleteTaskItem = "UPDATE task_details SET is_active = 0 WHERE task_detail_id = ?";

const std::string task::getTaskId = "SELECT task_id FROM tasks WHERE task_date = ?";
const std::string task::createTask = "INSERT INTO tasks (task_date, is_active, project_id) VALUES (?, 1, ?)";
const std::string task::getTaskHoursById = "SELECT task_details.duration "
                                           "FROM task_details "
                                           "INNER JOIN tasks ON task_details.task_id = tasks.task_id "
                                           "WHERE task_date = ?"
;
}

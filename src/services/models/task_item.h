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

#pragma once

#include <string>

namespace app::models
{
struct task_item
{
    task_item() = default;
    ~task_item() = default;

    int task_detail_id;
    int project_id;
    std::string project_name;
    std::string start_time;
    std::string end_time;
    std::string duration;
    int category_id;
    std::string category_name;
    std::string description;
    int date_created_utc;
    int date_modified_utc;
    int is_active;

    static const std::string createNewTaskItem;
    static const std::string getAllTaskItemsByDate;
    static const std::string getTaskItemById;
    static const std::string updateTaskItem;
    static const std::string deleteTaskItem;
};

struct task
{
    task() = default;
    ~task() = default;

    static const std::string getTaskId;
    static const std::string createTask;
    static const std::string getTaskHoursById;
};
}

//  Tasks Tracker is a desktop that aids you in tracking your timesheets
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

#pragma once

#include <memory>
#include <string>

namespace app::models
{
struct task_item {
    task_item() = default;
    explicit task_item(int taskItemId,
        std::string taskDate,
        std::unique_ptr<std::string> startTime,
        std::unique_ptr<std::string> endTime,
        std::string duration,
        std::string description,
        std::string categoryName,
        int categoryColor,
        std::string projectName,
        int taskItemTypeId);
    explicit task_item(int taskItemId,
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
        int isActive);
    ~task_item() = default;

    int task_item_id;
    int task_id;
    int project_id;
    std::string project_name;
    std::string task_date;
    std::string* start_time;
    std::string* end_time;
    std::string duration;
    int task_item_type_id;
    int category_id;
    std::string category_name;
    int category_color;
    std::string description;
    int billable;
    int date_created_utc;
    int date_modified_utc;
    int is_active;

    // This method exists to clean up memory as unfortunately the
    // destructor runs too soon and deallocates the memory
    // Might need to rethink how to make this work
    // with sqlite_modern_cpp (04/10/2019)
    void cleanup();

    static const std::string createNewTaskItem;
    static const std::string getAllTaskItemsByDate;
    static const std::string getTaskItemById;
    static const std::string updateTaskItem;
    static const std::string deleteTaskItem;
};

struct task {
    task() = default;
    ~task() = default;

    static const std::string getTaskId;
    static const std::string createTask;
    static const std::string getTaskHoursById;
};
} // namespace app::models

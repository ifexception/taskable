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

#include "db_service.h"

#include "db_connection.h"

// clang-format off

namespace app::services
{
int db_service::get_last_insert_rowid()
{
    auto db = db_connection::get_instance().get_handle();
    int id = (int) db.last_insert_rowid();
    return id;
}

void db_service::create_new_project(const std::string& name,
    const std::string& displayName,
    const int employerId,
    const int* clientId)
{
    auto db = db_connection::get_instance().get_handle();

    if (clientId == nullptr) {
        db << models::project::createNewProject
           << name
           << displayName
           << employerId
           << nullptr;
    } else {
        db << models::project::createNewProject
           << name
           << displayName
           << employerId
           << *clientId;
    }
}

std::vector<models::project> db_service::get_projects()
{
    std::vector<models::project> projects;

    auto db = db_connection::get_instance().get_handle();
    db << models::project::getProjects
       >> [&](int projectId,
           std::string projectName,
           std::string displayName,
           int dateCreatedUtc,
           int dateModifiedUtc,
           int isActive,
           std::string employerName,
           std::unique_ptr<std::string> clientName) {
           models::project project(
               projectId, projectName, displayName, dateCreatedUtc, dateModifiedUtc, isActive, employerName,
               clientName != nullptr ? *clientName : "");
           projects.push_back(project);
       };

    return projects;
}

models::project db_service::get_project_by_id(const int projectId)
{
    models::project project;

    auto db = db_connection::get_instance().get_handle();
    db << models::project::getProjectById
        << projectId
        >> [&](int projectId,
           std::string projectName,
           std::string displayName,
           int dateCreatedUtc,
           int dateModifiedUtc,
           int isActive,
           int employerId,
           std::string employerName,
           std::unique_ptr<std::string> clientName) {
           project = models::project(
               projectId, projectName, displayName, dateCreatedUtc, dateModifiedUtc, isActive, employerId, employerName,
                clientName != nullptr ? *clientName : "");
       };

    return project;
}

void db_service::update_project(models::project project)
{
    auto db = db_connection::get_instance().get_handle();
    if (project.client_id == 0) {
        db << models::project::updateProject
           << project.project_name
           << project.display_name
           << project.date_modified_utc
           << project.employer_id
           << nullptr
           << project.project_id;
    } else {
        db << models::project::updateProject
           << project.project_name
           << project.display_name
           << project.date_modified_utc
           << project.employer_id
           << project.client_id
           << project.project_id;
    }
}

void db_service::delete_project(const int projectId, const int dateModified)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::project::deleteProject
       << dateModified
       << projectId;
}

void db_service::create_new_category(const int projectId, const std::string& name, int color, const std::string& description)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::category::createNewCategory
       << name
       << color
       << description
       << projectId;
}

void db_service::create_new_category(const models::category category)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::category::createNewCategory
       << category.category_name
       << category.color
       << category.project_id;
}

std::vector<models::category> db_service::get_categories_by_project_id(const int projectId)
{
    std::vector<models::category> categories;

    auto db = db_connection::get_instance().get_handle();
    db << models::category::getCategoriesByProjectId
       << projectId
       >> [&](int categoryId,
           std::string categoryName,
           unsigned int color,
           int dateCreatedUtc,
           int dateModifiedUtc,
           int isActive,
           int projectId) {
           models::category category(categoryId,
               categoryName,
               color,
               dateCreatedUtc,
               dateModifiedUtc,
               isActive,
               projectId);
           categories.push_back(category);
       };

    return categories;
}

models::category db_service::get_category_by_id(const int categoryId)
{
    models::category category;

    auto db = db_connection::get_instance().get_handle();
    db << models::category::getCategoryById
       << categoryId
       >> [&](int categoryId,
           std::string categoryName,
           unsigned int color,
           int dateCreatedUtc,
           int dateModifiedUtc,
           int isActive,
           std::string projectName) {
           category = models::category(categoryId,
               categoryName,
               color,
               dateCreatedUtc,
               dateModifiedUtc,
               isActive,
               projectName);
       };

    return category;
}

std::vector<models::category> db_service::get_categories()
{
    std::vector<models::category> categories;

    auto db = db_connection::get_instance().get_handle();
    db << models::category::getCategories
       >> [&](int categoryId,
           std::string categoryName,
           int dateCreatedUtc,
           int dateModifiedUtc,
           int isActive,
           std::string projectName) {
            models::category category(categoryId,
                   categoryName,
                   dateCreatedUtc,
                   dateModifiedUtc,
                   isActive,
                   projectName);
            categories.push_back(category);
       };

    return categories;
}

void db_service::update_category(const models::category category)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::category::updateCategory
       << category.category_name
       << category.color
       << category.project_id
       << category.date_modified_utc
       << category.category_id;
}

void db_service::delete_category(const int categoryId, const int dateModified)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::category::deleteCategory
       << dateModified;
}

models::task_context db_service::get_employer_and_client_by_project_id(const int projectId)
{
    models::task_context taskContext;

    auto db = db_connection::get_instance().get_handle();
    db << models::task_context::getEmployerAndClientByProjectId
       << projectId
       >> [&](std::string employerName,
            std::unique_ptr<std::string> clientName) {
               taskContext = models::task_context(employerName, std::move(clientName));
    };

    return taskContext;
}

int db_service::create_or_get_task_id(const std::string& date)
{
    int aTaskId = 0;
    bool taskDoesNotExistYet = true;

    auto db = db_connection::get_instance().get_handle();
    db << models::task::getTaskId
       << date
       >> [&](std::unique_ptr<int> taskId) {
            taskDoesNotExistYet = false;
            aTaskId = *taskId;
          };

    if (taskDoesNotExistYet) {
        db << models::task::createTask
           << date;
        aTaskId = (int) db.last_insert_rowid();
    }

    return aTaskId;
}

void db_service::create_new_task_item(const models::task_item& taskItem)
{
    auto db = db_connection::get_instance().get_handle();
    if (taskItem.start_time == nullptr && taskItem.end_time == nullptr) {
         db << models::task_item::createNewTaskItem
            << nullptr
            << nullptr
            << taskItem.duration
            << taskItem.description
            << taskItem.billable
            << taskItem.task_item_type_id
            << taskItem.project_id
            << taskItem.task_id
            << taskItem.category_id;
    } else {
        db << models::task_item::createNewTaskItem
           << *taskItem.start_time
           << *taskItem.end_time
           << taskItem.duration
           << taskItem.description
           << taskItem.billable
           << taskItem.task_item_type_id
           << taskItem.project_id
           << taskItem.task_id
           << taskItem.category_id;
    }
}

std::vector<models::task_item> db_service::get_all_task_items_by_date(const std::string& date)
{
    std::vector<models::task_item> taskItems;

    auto db = db_connection::get_instance().get_handle();
    db << models::task_item::getAllTaskItemsByDate
       << date
       >> [&](int taskItemId,
           std::string taskDate,
           std::unique_ptr<std::string> startTime,
           std::unique_ptr<std::string> endTime,
           std::string duration,
           std::string description,
           std::string categoryName,
           int categoryColor,
           std::string projectName,
           int taskItemTypeId) {
           models::task_item taskItem(taskItemId,
               taskDate,
               std::move(startTime),
               std::move(endTime),
               duration,
               description,
               categoryName,
               categoryColor,
               projectName,
               taskItemTypeId);
           taskItems.push_back(taskItem);
       };

    return taskItems;
}

models::task_item db_service::get_task_item_by_id(const int taskId)
{
    models::task_item taskItem;

    auto db = db_connection::get_instance().get_handle();
    db << models::task_item::getTaskItemById
       << taskId
       >> [&](int taskItemId,
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
           int isActive) {
           taskItem = models::task_item(taskItemId,
               projectId,
               projectName,
               std::move(startTime),
               std::move(endTime),
               duration,
               categoryId,
               categoryName,
               description,
               billable,
               dateCreatedUtc,
               dateModifiedUtc,
               isActive);
       };

    return taskItem;
}

void db_service::update_task_item(models::task_item task)
{
    auto db = db_connection::get_instance().get_handle();
    if (task.start_time == nullptr && task.end_time == nullptr) {
         db << models::task_item::updateTaskItem
            << nullptr
            << nullptr
            << task.duration
            << task.description
            << task.billable
            << task.date_modified_utc
            << task.project_id
            << task.category_id
            << task.task_item_id;
    } else {
        db << models::task_item::updateTaskItem
           << *task.start_time
           << *task.end_time
           << task.duration
           << task.description
           << task.billable
           << task.date_modified_utc
           << task.project_id
           << task.category_id
           << task.task_item_id;
    }
}

void db_service::delete_task_item(const int taskId, const int dateModified)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::task_item::deleteTaskItem
       << dateModified
       << taskId;
}

std::vector<std::string> db_service::get_task_hours_by_id(const std::string& date)
{
    std::vector<std::string> taskDurations;

    auto db = db_connection::get_instance().get_handle();
    db << models::task::getTaskHoursById
       << date
       >> [&](std::string duration) {
           taskDurations.push_back(duration);
       };

    return taskDurations;
}

} // namespace app::services

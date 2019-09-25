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

#include "db_service.h"

#include "db_connection.h"

// clang-format off

namespace app::services
{
void db_service::create_employers_table()
{
    // TODO Move sql queries out db_service
    const std::string query = "CREATE TABLE employers"
                              "("
                              "    employer_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL,"
                              "    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    is_active INTEGER NOT NULL"
                              ");";
    auto db = db_connection::get_instance().get_handle();
    db << query;
}

void db_service::create_clients_table()
{
    const std::string query = "CREATE TABLE clients"
                              "("
                              "    client_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL,"
                              "    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    is_active INTEGER NOT NULL,"
                              "    employer_id INTEGER NOT NULL,"
                              "    FOREIGN KEY (employer_id) REFERENCES employers(employer_id)"
                              ");";

    auto db = db_connection::get_instance().get_handle();
    db << query;
}

void db_service::create_projects_table()
{
    const std::string query = "CREATE TABLE projects"
                              "("
                              "    project_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL UNIQUE,"
                              "    display_name TEXT NOT NULL,"
                              "    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    is_active INTEGER NOT NULL,"
                              "    employer_id INTEGER NOT NULL,"
                              "    client_id INTEGER NULL,"
                              "    FOREIGN KEY (employer_id) REFERENCES employers(employer_id),"
                              "    FOREIGN KEY (client_id) REFERENCES clients(client_id)"
                              ");";

    auto db = db_connection::get_instance().get_handle();
    db << query;
}

void db_service::create_categories_table()
{
    const std::string query = "CREATE TABLE categories"
                              "("
                              "    category_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL,"
                              "    description TEXT NULL,"
                              "    color INTEGER NOT NULL,"
                              "    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    is_active INTEGER NOT NULL,"
                              "    project_id INTEGER NOT NULL,"
                              "    FOREIGN KEY (project_id) REFERENCES projects(project_id)"
                              ");";

    auto db = db_connection::get_instance().get_handle();
    db << query;
}

void db_service::create_tasks_table()
{
    const std::string query = "CREATE TABLE tasks"
                              "("
                              "    task_id INTEGER PRIMARY KEY NOT NULL,"
                              "    task_date TEXT NOT NULL UNIQUE,"
                              "    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    is_active INTEGER NOT NULL"
                              ");";

    auto db = db_connection::get_instance().get_handle();
    db << query;
}

void db_service::create_task_items_table()
{
    const std::string query = "CREATE TABLE task_items"
                              "("
                              "    task_item_id INTEGER PRIMARY KEY NOT NULL,"
                              "    start_time TEXT NULL,"
                              "    end_time TEXT NULL,"
                              "    duration TEXT NOT NULL,"
                              "    description TEXT NOT NULL,"
                              "    billable INTEGER NOT NULL, "
                              "    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    is_active INTEGER NOT NULL,"
                              "    project_id INTEGER NOT NULL,"
                              "    task_id INTEGER NOT NULL,"
                              "    category_id INTEGER NOT NULL,"
                              "    FOREIGN KEY (project_id) REFERENCES projects(project_id),"
                              "    FOREIGN KEY (task_id) REFERENCES tasks(task_id),"
                              "    FOREIGN KEY (category_id) REFERENCES categories(category_id)"
                              ");";

    auto db = db_connection::get_instance().get_handle();
    db << query;
}

int db_service::get_last_insert_rowid()
{
    auto db = db_connection::get_instance().get_handle();
    int id = (int) db.last_insert_rowid();
    return id;
}

void db_service::create_new_employer(const std::string& employerName)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::employer::createNewEmployer
       << employerName;
}

std::vector<models::employer> db_service::get_employers()
{
    std::vector<models::employer> employers;

    auto db = db_connection::get_instance().get_handle();
    db << models::employer::getEmployers
       >> [&](int employerId, std::string employerName, int dateCreatedUtc, int dateModifiedUtc, int isActive) {
           models::employer employer(employerId, employerName, dateCreatedUtc, dateModifiedUtc, isActive);
           employers.push_back(employer);
       };

    return employers;
}

models::employer db_service::get_employer(const int employerId)
{
    models::employer employer;

    auto db = db_connection::get_instance().get_handle();
    db << models::employer::getEmployer
       << employerId
       >> [&](int employerId, std::string employerName, int dateCreatedUtc, int dateModifiedUtc, int isActive) {
           employer = models::employer(employerId, employerName, dateCreatedUtc, dateModifiedUtc, isActive);
       };

    return employer;
}

void db_service::update_employer(models::employer employer)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::employer::updateEmployer
       << employer.employer_name
       << employer.date_modified_utc
       << employer.employer_id;
}

void db_service::delete_employer(const int employerId, const int dateModified)
{
    auto db = db_connection::get_instance().get_handle();

    db << models::employer::deleteEmployer
       << dateModified
       << employerId;
}

void db_service::create_new_client(const std::string& name, const int employerId)
{
    auto db = db_connection::get_instance().get_handle();

    db << models::client::createNewClient
       << name
       << employerId;
}

std::vector<models::client> db_service::get_clients_by_employer_id(const int employerId)
{
    std::vector<models::client> clients;

    auto db = db_connection::get_instance().get_handle();
    db << models::client::getClientsByEmployerId
       << employerId
       >> [&](int clientId, std::string clientName, int dateCreatedUtc, int dateModifiedUtc, int isActive, int employerId) {
            models::client client(clientId, clientName, dateCreatedUtc, dateModifiedUtc, isActive, employerId);
            clients.push_back(client);
       };

    return clients;
}

std::vector<models::client> db_service::get_clients()
{
    std::vector<models::client> clients;

    auto db = db_connection::get_instance().get_handle();
    db << models::client::getClients
       >> [&](int clientId, std::string clientName, int dateCreatedUtc, int dateModifiedUtc, int isActive, std::string employerName) {
            models::client client(clientId, clientName, dateCreatedUtc, dateModifiedUtc, isActive, employerName);
            clients.push_back(client);
       };

    return clients;
}

models::client db_service::get_client_by_id(const int clientId)
{
    models::client client;

    auto db = db_connection::get_instance().get_handle();
    db << models::client::getClientById
       << clientId
       >> [&](int clientId, std::string clientName, int dateCreatedUtc, int dateModifiedUtc, int isActive, std::string employerName) {
            client = models::client(clientId, clientName, dateCreatedUtc, dateModifiedUtc, isActive, employerName);
       };

    return client;
}

void db_service::update_client(models::client client)
{
    auto db = db_connection::get_instance().get_handle();

    db << models::client::updateClient
       << client.client_name
       << client.date_modified_utc
       << client.employer_id
       << client.client_id;
}

void db_service::delete_client(const int clientId, const int dateModified)
{
    auto db = db_connection::get_instance().get_handle();

    db << models::client::deleteClient
       << dateModified
       << clientId;
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
           << project.employer_id
           << nullptr;
    } else {
        db << models::project::updateProject
           << project.project_name
           << project.display_name
           << project.employer_id
           << project.client_id;
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

std::vector<models::category> db_service::get_categories_by_project_id(const int projectId)
{
    std::vector<models::category> categories;

    auto db = db_connection::get_instance().get_handle();
    db << models::category::getCategoriesByProjectId
       << projectId
       >> [&](int categoryId,
           std::string categoryName,
           int color,
           std::unique_ptr<std::string> description,
           int dateCreatedUtc,
           int dateModifiedUtc,
           int isActive,
           int projectId) {
           models::category category(categoryId,
               categoryName,
               color,
               description != nullptr ? *description : "",
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
           int color,
           std::unique_ptr<std::string> description,
           int dateCreatedUtc,
           int dateModifiedUtc,
           int isActive,
           std::string projectName) {
           category = models::category(categoryId,
               categoryName,
               color,
               description != nullptr ? *description : "",
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
           std::unique_ptr<std::string> description,
           int dateCreatedUtc,
           int dateModifiedUtc,
           int isActive,
           std::string projectName) {
            models::category category(categoryId,
                   categoryName,
                   description != nullptr ? *description : "",
                   dateCreatedUtc,
                   dateModifiedUtc,
                   isActive,
                   projectName);
            categories.push_back(category);
       };

    return categories;
}

void db_service::update_category(models::category category)
{
    auto db = db_connection::get_instance().get_handle();
    db << models::category::updateCategory
       << category.category_name
       << category.color
       << category.description
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

void db_service::create_new_task_item(const int projectId,
    const int taskId,
    const std::string& startTime,
    const std::string& endTime,
    const std::string& duration,
    const int categoryId,
    const std::string& description,
    const int billable)
{
    auto db = db_connection::get_instance().get_handle();
    db <<  models::task_item::createNewTaskItem
       << startTime
       << endTime
       << duration
       << description
       << billable
       << projectId
       << taskId
       << categoryId;
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
           std::string projectName) {
           models::task_item taskItem(taskItemId,
               taskDate,
               startTime != nullptr ? *startTime : "",
               endTime != nullptr ? *endTime : "",
               duration,
               description,
               categoryName,
               categoryColor,
               projectName);
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
               startTime != nullptr ? *startTime : "",
               endTime != nullptr ? *endTime : "",
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
    db << models::task_item::updateTaskItem
       << task.start_time
       << task.end_time
       << task.duration
       << task.description
       << task.billable
       << task.date_modified_utc
       << task.project_id
       << task.category_id
       << task.task_item_id;
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

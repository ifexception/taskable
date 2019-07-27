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
#include "../db/command.h"
#include "../db/query.h"
#include "../db/copy_options.h"
#include "../db/column_type.h"

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

    db::command command(*db_connection::get_instance().get_database(), query);
    command.execute();
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

    db::command command(*db_connection::get_instance().get_database(), query);
    command.execute();
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

    db::command command(*db_connection::get_instance().get_database(), query);
    command.execute();
}

void db_service::create_categories_table()
{
    const std::string query = "CREATE TABLE categories"
        "("
        "    category_id INTEGER PRIMARY KEY NOT NULL,"
        "    name TEXT NOT NULL,"
        "    description TEXT NULL,"
        "    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
        "    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
        "    is_active INTEGER NOT NULL,"
        "    project_id INTEGER NOT NULL,"
        "    FOREIGN KEY (project_id) REFERENCES projects(project_id)"
        ");";

    db::command command(*db_connection::get_instance().get_database(), query);
    command.execute();
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

    db::command command(*db_connection::get_instance().get_database(), query);
    command.execute();
}

void db_service::create_task_items_table()
{
    const std::string query = "CREATE TABLE task_items"
        "("
        "    task_item_id INTEGER PRIMARY KEY NOT NULL,"
        "    start_time TEXT NOT NULL,"
        "    end_time TEXT NOT NULL,"
        "    duration TEXT NOT NULL,"
        "    description TEXT NOT NULL,"
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

    db::command command(*db_connection::get_instance().get_database(), query);
    command.execute();
}

int db_service::get_last_insert_rowid()
{
    int id = (int) db_connection::get_instance().get_database()->get_last_rowid();
    return id;
}

void db_service::create_new_employer(const std::string& employerName)
{
    auto instance = db_connection::get_instance().get_database();
    db::command command(*instance, models::employer::createNewEmployer);
    command.bind(1, employerName, db::copy_options::NoCopy);
    command.execute();
}

std::vector<models::employer> db_service::get_employers()
{
    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, models::employer::getEmployers);

    std::vector<models::employer> employers;
    while (query.run()) {
        db::column column(query.get_handle());
        models::employer employer;

        employer.employer_id = column.get<int>(0);
        employer.employer_name = column.get<std::string>(1);
        employer.date_created_utc = column.get<int>(2);
        employer.date_modified_utc = column.get<int>(3);
        employer.is_active = column.get<int>(4);
        employers.push_back(employer);
    }

    return employers;
}

models::employer db_service::get_employer(const int employerId)
{
    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, models::employer::getEmployer);
    query.bind(1, employerId);
    query.run();
    db::column column(query.get_handle());
    models::employer employer;

    employer.employer_id = column.get<int>(0);
    employer.employer_name = column.get<std::string>(1);
    employer.date_created_utc = column.get<int>(2);
    employer.date_modified_utc = column.get<int>(3);
    employer.is_active = column.get<int>(4);

    return employer;
}

void db_service::update_employer(models::employer employer)
{
    db::command command(*db_connection::get_instance().get_database(), models::employer::updateEmployer);
    command.binder() << employer.employer_name << employer.date_modified_utc << employer.employer_id;
    command.execute();
}

void db_service::delete_employer(const int employerId, const int dateModified)
{
    db::command command(*db_connection::get_instance().get_database(), models::employer::deleteEmployer);
    command.binder() << dateModified << employerId;
    command.execute();
}

void db_service::create_new_client(const std::string& name, const int employerId)
{
    db::command command(*db_connection::get_instance().get_database(), models::client::createNewClient);
    command.binder() << name << employerId;
    command.execute();
}

std::vector<models::client> db_service::get_clients_by_employer_id(const int employerId)
{
    db::query query(*db_connection::get_instance().get_database(), models::client::getClientsByEmployerId);
    query.bind(1, employerId);

    std::vector<models::client> clients;
    while (query.run()) {
        db::column column(query.get_handle());
        models::client client;

        client.client_id = column.get<int>(0);
        client.client_name = column.get<std::string>(1);
        client.date_created_utc = column.get<int>(2);
        client.date_modified_utc = column.get<int>(3);
        client.is_active = column.get<int>(4);
        client.employer_id = column.get<int>(5);
        clients.push_back(client);
    }

    return clients;
}

std::vector<models::client> db_service::get_clients()
{
    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, models::client::getClients);

    std::vector<models::client> clients;
    while (query.run()) {
        db::column column(query.get_handle());
        models::client client;

        client.client_id = column.get<int>(0);
        client.client_name = column.get<std::string>(1);
        client.date_created_utc = column.get<int>(2);
        client.date_modified_utc = column.get<int>(3);
        client.is_active = column.get<int>(4);
        client.employer_name = column.get<std::string>(5);
        clients.push_back(client);
    }

    return clients;
}

models::client db_service::get_client_by_id(const int clientId)
{
    db::query query(*db_connection::get_instance().get_database(), models::client::getClientById);
    query.bind(1, clientId);
    query.run();
    db::column column(query.get_handle());
    models::client client;

    client.client_id = column.get<int>(0);
    client.client_name = column.get<std::string>(1);
    client.date_created_utc = column.get<int>(2);
    client.date_modified_utc = column.get<int>(3);
    client.is_active = column.get<int>(4);
    client.employer_name = column.get<std::string>(5);

    return client;
}

void db_service::update_client(models::client client)
{
    db::command command(*db_connection::get_instance().get_database(), models::client::updateClient);
    command.binder() << client.client_name << client.date_modified_utc << client.employer_id << client.client_id;
    command.execute();
}

void db_service::delete_client(const int clientId, const int dateModified)
{
    db::command command(*db_connection::get_instance().get_database(), models::client::deleteClient);
    command.binder() << dateModified << clientId;
    command.execute();
}

void db_service::create_new_project(const std::string& name, const std::string& displayName, const int employerId, const int* clientId)
{
    db::command command(*db_connection::get_instance().get_database(), models::project::createNewProject);
    if (clientId == nullptr) {
        command.binder() << name << displayName << employerId << ( void*) 0;
    } else {
        command.binder() << name << displayName << employerId << *clientId;
    }

    command.execute();
}

std::vector<models::project> db_service::get_projects()
{
    db::query query(*db_connection::get_instance().get_database(), models::project::getProjects);

    std::vector<models::project> projects;
    while (query.run()) {
        db::column column(query.get_handle());
        models::project project;

        project.project_id = column.get<int>(0);
        project.project_name = column.get<std::string>(1);
        project.display_name = column.get<std::string>(2);
        project.date_created_utc = column.get<int>(3);
        project.date_modified_utc = column.get<int>(4);
        project.is_active = column.get<int>(5);
        project.employer_name = column.get < std::string >(6);
        if (column.get_type(7) == db::column_type::Null) {
            project.client_name = "";
        } else {
            project.client_name = column.get<std::string>(7);
        }

        projects.push_back(project);
    }

    return projects;
}

models::project db_service::get_project_by_id(const int projectId)
{
    db::query query(*db_connection::get_instance().get_database(), models::project::getProjectById);
    query.bind(1, projectId);
    query.run();
    db::column column(query.get_handle());
    models::project project;

    project.project_id = column.get<int>(0);
    project.project_name = column.get<std::string>(1);
    project.display_name = column.get<std::string>(2);
    project.date_created_utc = column.get<int>(3);
    project.date_modified_utc = column.get<int>(4);
    project.is_active = column.get<int>(5);
    project.employer_id = column.get<int>(6);
    project.employer_name = column.get<std::string>(7);
    if (column.get_type(8) == db::column_type::Null) {
        project.client_name = "";
    } else {
        project.client_name = column.get<std::string>(8);
    }

    return project;
}

void db_service::update_project(models::project project)
{
    db::command command(*db_connection::get_instance().get_database(), models::project::updateProject);
    if (project.client_id == 0) {
        command.binder() << project.project_name << project.display_name << project.employer_id << ( void*) 0;
    } else {
        command.binder() << project.project_name << project.display_name << project.employer_id << project.client_id;
    }

    command.execute();
}

void db_service::delete_project(const int projectId, const int dateModified)
{
    db::command command(*db_connection::get_instance().get_database(), models::project::deleteProject);
    command.binder() << dateModified << projectId;
    command.execute();
}

void db_service::create_new_category(const int projectId, const std::string& name, const std::string& description)
{
    db::command command(*db_connection::get_instance().get_database(), models::category::createNewCategory);
    command.binder() << name << description << projectId;
    command.execute();
}

std::vector<models::category> db_service::get_categories_by_project_id(const int projectId)
{
    db::query query(*db_connection::get_instance().get_database(), models::category::getCategoriesByProjectId);
    query.bind(1, projectId);

    std::vector<models::category> categories;
    while (query.run()) {
        db::column column(query.get_handle());
        models::category category;

        category.category_id = column.get<int>(0);
        category.category_name = column.get<std::string>(1);
        category.description = column.get<std::string>(2);
        category.date_created_utc = column.get<int>(3);
        category.date_modified_utc = column.get<int>(4);
        category.is_active = column.get<int>(5);
        category.project_id = column.get<int>(6);

        categories.push_back(category);
    }

    return categories;
}

models::category db_service::get_category_by_id(const int categoryId)
{
    db::query query(*db_connection::get_instance().get_database(), models::category::getCategoryById);
    query.bind(1, categoryId);
    query.run();
    db::column column(query.get_handle());
    models::category category;

    category.category_id = column.get<int>(0);
    category.category_name = column.get<std::string>(1);
    category.description = column.get<std::string>(2);
    category.date_created_utc = column.get<int>(3);
    category.date_modified_utc = column.get<int>(4);
    category.is_active = column.get<int>(5);
    category.project_name = column.get<std::string>(6);

    return category;
}

std::vector<models::category> db_service::get_categories()
{
    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, models::category::getCategories);

    std::vector<models::category> categories;
    while (query.run()) {
        db::column column(query.get_handle());
        models::category category;

        category.category_id = column.get<int>(0);
        category.category_name = column.get<std::string>(1);
        category.description = column.get<std::string>(2);
        category.date_created_utc = column.get<int>(3);
        category.date_modified_utc = column.get<int>(4);
        category.is_active = column.get<int>(5);
        category.project_name = column.get<std::string>(6);

        categories.push_back(category);
    }

    return categories;
}

void db_service::update_category(models::category category)
{
    db::command command(*db_connection::get_instance().get_database(), models::category::updateCategory);
    command.binder() << category.category_name << category.description << category.project_id << category.date_modified_utc;
    command.execute();
}

void db_service::delete_category(const int categoryId, const int dateModified)
{
    db::command command(*db_connection::get_instance().get_database(), models::category::deleteCategory);
    command.binder() << dateModified;
    command.execute();
}

int db_service::create_or_get_task_id(const std::string& date, const int projectId)
{
    db::query query(*db_connection::get_instance().get_database(), models::task::getTaskId);
    query.bind(1, date, db::copy_options::NoCopy);
    int taskId = 0;
    query.run();
    db::column column(query.get_handle());
    if (column.get_type(0) == db::column_type::Null) {
        db::command command(*db_connection::get_instance().get_database(), models::task::createTask);
        command.binder() << date << projectId;
        command.execute();
        taskId = (int) db_connection::get_instance().get_database()->get_last_rowid();
    } else {
        taskId = column.get<int>(0);
    }

    return taskId;
}

void db_service::create_new_task_item(const int projectId, const int taskId, const std::string& startTime, const std::string& endTime, const std::string& duration, const int categoryId, const std::string& description)
{
    db::command command(*db_connection::get_instance().get_database(), models::task_item::createNewTaskItem);
    command.binder() << startTime << endTime << duration << description << projectId << taskId << categoryId;
    command.execute();
}

std::vector<models::task_item> db_service::get_all_task_items_by_date(const std::string& date)
{
    db::query query(*db_connection::get_instance().get_database(), models::task_item::getAllTaskItemsByDate);
    query.bind(1, date, db::copy_options::NoCopy);

    std::vector<models::task_item> taskItems;
    while (query.run()) {
        db::column column(query.get_handle());
        models::task_item taskItem;

        taskItem.task_item_id = column.get<int>(0);
        taskItem.task_date = column.get<std::string>(1);
        taskItem.start_time = column.get<std::string>(2);
        taskItem.end_time = column.get<std::string>(3);
        taskItem.duration = column.get<std::string>(4);
        taskItem.description = column.get<std::string>(5);
        taskItem.category_name = column.get<std::string>(6);
        taskItem.project_name = column.get<std::string>(7);

        taskItems.push_back(taskItem);
    }

    return taskItems;
}

models::task_item db_service::get_task_item_by_id(const int taskId)
{
    db::query query(*db_connection::get_instance().get_database(), models::task_item::getTaskItemById);
    query.bind(1, taskId);

    query.run();

    db::column column(query.get_handle());
    models::task_item taskDetail;

    taskDetail.task_item_id = column.get<int>(0);
    taskDetail.project_id = column.get<int>(1);
    taskDetail.project_name = column.get<std::string>(2);
    taskDetail.start_time = column.get<std::string>(3);
    taskDetail.end_time = column.get<std::string>(4);
    taskDetail.duration = column.get<std::string>(5);
    taskDetail.category_id = column.get<int>(6);
    taskDetail.category_name = column.get<std::string>(7);
    taskDetail.description = column.get<std::string>(8);
    taskDetail.date_created_utc = column.get<int>(9);
    taskDetail.date_modified_utc = column.get<int>(10);
    taskDetail.is_active = column.get<int>(11);

    return taskDetail;
}

void db_service::update_task_item(models::task_item task)
{
    db::command command(*db_connection::get_instance().get_database(), models::task_item::updateTaskItem);

    command.binder() << task.start_time << task.end_time << task.duration << task.description << task.date_modified_utc << task.project_id << task.category_id << task.task_item_id;
    command.execute();
}

void db_service::delete_task_item(const int taskId, const int dateModified)
{
    db::command command(*db_connection::get_instance().get_database(), models::task_item::deleteTaskItem);

    command.binder() << dateModified << taskId;
    command.execute();
}

std::vector<std::string> db_service::get_task_hours_by_id(const std::string& date)
{
    db::query query(*db_connection::get_instance().get_database(), models::task::getTaskHoursById);
    query.bind(1, date, db::copy_options::NoCopy);

    std::vector<std::string> taskDurations;

    while (query.run()) {
        db::column column(query.get_handle());
        taskDurations.push_back(column.get<std::string>(0));
    }

    return taskDurations;
}

} // namespace app::services

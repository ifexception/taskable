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
void db_service::create_new_employer(const std::string& employerName)
{
    std::string cmd("INSERT INTO employers (name, is_active) VALUES (?, 1);");
    auto instance = db_connection::get_instance().get_database();
    db::command command(*instance, cmd);
    command.bind(1, employerName, db::copy_options::NoCopy);
    command.execute();
}

std::vector<models::employer> db_service::get_employers()
{
    std::string qry("SELECT * FROM employers WHERE is_active = 1;");
    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, qry);

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
    std::string qry("SELECT * FROM employers WHERE employer_id = ?");
    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, qry);
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
    std::string cmd("UPDATE employers SET name = ?, date_modified_utc = ? WHERE employer_id = ?");
    db::command command(*db_connection::get_instance().get_database(), cmd);
    command.binder() << employer.employer_name << employer.date_modified_utc << employer.employer_id;
    command.execute();
}

void db_service::delete_employer(const int employerId)
{
    std::string cmd("UPDATE employers SET is_active = 0 WHERE employer_id = ?");
    db::command command(*db_connection::get_instance().get_database(), cmd);
    command.binder() << employerId;
    command.execute();
}

void db_service::create_new_client(const std::string& name, const int employerId)
{
    std::string cmd("INSERT INTO clients (name, is_active, employer_id) VALUES (?, 1, ?)");
    db::command command(*db_connection::get_instance().get_database(), cmd);
    command.binder() << name << employerId;
    command.execute();
}

std::vector<models::client> db_service::get_clients_by_employer_id(const int employerId)
{
    std::string cmd("SELECT * FROM clients WHERE employer_id = ?");
    db::query query(*db_connection::get_instance().get_database(), cmd);
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
    std::string select("SELECT clients.client_id,");
    std::string clientName("clients.name AS client_name,");
    std::string dateCreated("clients.date_created_utc,");
    std::string dateModified("clients.date_modified_utc,");
    std::string isActive("clients.is_active,");
    std::string employerName("employers.name AS employer_name");
    std::string from("FROM clients,");
    std::string innerJoin("INNER JOIN employers ON clients.employer_id = employers.employer_id");
    std::string where("WHERE clients.is_active = 1");

    std::string qry = select + clientName + dateCreated + dateModified + isActive + employerName + from + innerJoin + where;
    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, qry);

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

    std::string select("SELECT clients.client_id,");
    std::string clientName("clients.name AS client_name,");
    std::string dateCreated("clients.date_created_utc,");
    std::string dateModified("clients.date_modified_utc,");
    std::string isActive("clients.is_active,");
    std::string employerName("employers.name AS employer_name");
    std::string from("FROM clients,");
    std::string innerJoin("INNER JOIN employers ON clients.employer_id = employers.employer_id");
    std::string where("WHERE clients.client_id = ?");

    std::string qry = select + clientName + dateCreated + dateModified + isActive + employerName + from + innerJoin + where;
    db::query query(*db_connection::get_instance().get_database(), qry);
    query.bind(1, clientId);
    query.run();
    db::column column(query.get_handle());
    models::client client;

    client.client_id = column.get<int>(0);
    client.client_name = column.get<std::string>(1);
    client.date_created_utc = column.get<int>(2);
    client.date_modified_utc = column.get<int>(3);
    client.is_active = column.get<int>(4);
    client.employer_name = column.get<int>(5);

    return client;
}

void db_service::create_new_project(const std::string& name, const std::string& displayName, const int employerId, const int* clientId)
{
    std::string cmd("INSERT INTO projects (name, display_name, is_active, employer_id, client_id) VALUES (?, ?, 1, ?, ?)");
    db::command command(*db_connection::get_instance().get_database(), cmd);
    if (clientId == nullptr) {
        command.binder() << name << displayName << employerId << (void*)0;
    } else {
        command.binder() << name << displayName << employerId << *clientId;
    }

    command.execute();
}

std::vector<models::project> db_service::get_projects()
{
    std::string select("SELECT projects.project_id,");
    std::string projectName("projects.name AS project_name,");
    std::string displayName("projects.display_name,");
    std::string dateCreated("projects.date_created_utc,");
    std::string dateModified("projects.date_modified_utc,");
    std::string isActive("projects.is_active,");
    std::string employerName("employers.name AS employer_name,");
    std::string clientName("clients.name AS client_name");
    std::string from("FROM projects");
    std::string innerJoinEmployers("INNER JOIN employers ON projects.employer_id = employers.employer_id");
    std::string innerJoinClients("INNER JOIN clients ON projects.client_id = clients.client_id");
    std::string where("WHERE projects.is_active = 1");
    std::string qry = select + projectName + displayName + dateCreated + dateModified + isActive + employerName + clientName + from + innerJoinEmployers + innerJoinClients + where;

    db::query query(*db_connection::get_instance().get_database(), qry);

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
        project.employer_name = column.get<int>(6);
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
    std::string select("SELECT projects.project_id,");
    std::string projectName("projects.name AS project_name,");
    std::string displayName("projects.display_name,");
    std::string dateCreated("projects.date_created_utc,");
    std::string dateModified("projects.date_modified_utc,");
    std::string isActive("projects.is_active,");
    std::string employerName("employers.name AS employer_name,");
    std::string clientName("clients.name AS client_name");
    std::string from("FROM projects");
    std::string innerJoinEmployers("INNER JOIN employers ON projects.employer_id = employers.employer_id");
    std::string innerJoinClients("INNER JOIN clients ON projects.client_id = clients.client_id");
    std::string where("WHERE projects.project_id = ?");
    std::string qry = select + projectName + displayName + dateCreated + dateModified + isActive + employerName + clientName + from + innerJoinEmployers + innerJoinClients + where;

    db::query query(*db_connection::get_instance().get_database(), qry);
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
    project.employer_name = column.get<std::string>(6);
    if (column.get_type(7) == db::column_type::Null) {
        project.client_name = "";
    } else {
        project.client_name = column.get<std::string>(7);
    }

    return project;
}

void db_service::create_new_category(const int projectId, const std::string& name, const std::string& description)
{
    std::string cmd("INSERT INTO categories (name, description, is_active, project_id) VALUES (?, ?, 1, ?)");
    db::command command(*db_connection::get_instance().get_database(), cmd);
    command.binder() << name << description << projectId;
    command.execute();
}

std::vector<models::category> db_service::get_categories_by_project_id(const int projectId)
{
    std::string qry("SELECT * FROM categories WHERE project_id = ?");
    db::query query(*db_connection::get_instance().get_database(), qry);
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
    std::string select("SELECT categories.category_id,");
    std::string categoryName("categories.name AS category_name,");
    std::string description("categories.description,");
    std::string dateCreated("categories.date_created_utc,");
    std::string dateModified("categories.date_modified_utc,");
    std::string isActive("categories.is_active,");
    std::string projectName("projects.name AS project_name");
    std::string from("FROM categories");
    std::string innerJoin("INNER JOIN projects ON categories.project_id = projects.project_id");
    std::string where("WHERE categories.category_id = ?");
    std::string qry = select + categoryName + description + dateCreated + dateModified + isActive + projectName + from + innerJoin + where;

    db::query query(*db_connection::get_instance().get_database(), qry);
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
    category.project_name = column.get<int>(6);

    return category;
}

std::vector<models::category> db_service::get_categories()
{
    std::string select("SELECT categories.category_id,");
    std::string categoryName("categories.name AS category_name,");
    std::string description("categories.description,");
    std::string dateCreated("categories.date_created_utc,");
    std::string dateModified("categories.date_modified_utc,");
    std::string isActive("categories.is_active,");
    std::string projectName("projects.name AS project_name");
    std::string from("FROM categories");
    std::string innerJoin("INNER JOIN projects ON categories.project_id = projects.project_id");
    std::string where("WHERE categories.is_active = 1");
    std::string qry = select + categoryName + description + dateCreated + dateModified + isActive + projectName + from + innerJoin + where;

    auto instance = db_connection::get_instance().get_database();
    db::query query(*instance, qry);

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

int db_service::create_or_get_task_id(const std::string& date, const int projectId)
{
    std::string cmd("SELECT task_id FROM tasks WHERE task_date = ?");
    db::query query(*db_connection::get_instance().get_database(), cmd);
    query.bind(1, date, db::copy_options::NoCopy);
    int taskId = 0;
    query.run();
    db::column column(query.get_handle());
    if (column.get_type(0) == db::column_type::Null) {
        std::string cmd2("INSERT INTO tasks (task_date, is_active, project_id) VALUES (?, 1, ?)");
        db::command command(*db_connection::get_instance().get_database(), cmd2);
        command.binder() << date << projectId;
        command.execute();
        taskId = (int)db_connection::get_instance().get_database()->get_last_rowid();
    } else {
        taskId = column.get<int>(0);
    }

    return taskId;
}

void db_service::create_new_task(const int projectId, const int taskId, const std::string& startTime, const std::string& endTime, const std::string& duration, const int categoryId, const std::string& description)
{
    std::string cmd("INSERT INTO task_details (start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES (?, ?, ?, ?, 1, ?, ?, ?)");
    db::command command(*db_connection::get_instance().get_database(), cmd);
    command.binder() << startTime << endTime << duration << description << projectId << taskId << categoryId;
    command.execute();
}

std::vector<models::detailed_task> db_service::get_all_tasks_by_date(const std::string& date)
{
    std::string select("SELECT task_details.task_detail_id, tasks.task_date, ");
    std::string startTime("task_details.start_time, ");
    std::string endTime("task_details.end_time, ");
    std::string duration("task_details.duration, ");
    std::string description("task_details.description as description, ");
    std::string category("categories.name as category_name, ");
    std::string project("projects.display_name as project_name ");
    std::string from("FROM task_details ");
    std::string innerJoinTasks("INNER JOIN tasks ON task_details.task_id = tasks.task_id ");
    std::string innerJoinCategories("INNER JOIN categories ON task_details.category_id = categories.category_id ");
    std::string innerJoinProjects("INNER JOIN projects ON task_details.project_id = projects.project_id ");
    std::string where("WHERE task_date = ?");
    std::string qry = select + startTime + endTime + duration + description + category + project + from + innerJoinTasks + innerJoinCategories + innerJoinProjects + where;
    db::query query(*db_connection::get_instance().get_database(), qry);
    query.bind(1, date, db::copy_options::NoCopy);

    std::vector<models::detailed_task> detailedTasks;
    while (query.run()) {
        db::column column(query.get_handle());
        models::detailed_task detailedTask;

        detailedTask.task_detail_id = column.get<int>(0);
        detailedTask.task_date = column.get<std::string>(1);
        detailedTask.start_time = column.get<std::string>(2);
        detailedTask.end_time = column.get<std::string>(3);
        detailedTask.duration = column.get<std::string>(4);
        detailedTask.description = column.get<std::string>(5);
        detailedTask.category_name = column.get<std::string>(6);
        detailedTask.project_name = column.get<std::string>(7);

        detailedTasks.push_back(detailedTask);
    }

    return detailedTasks;
}

models::task_detail db_service::get_task_by_id(const int taskDetailId)
{
    std::string select("SELECT task_details.task_detail_id, ");
    std::string projectId("projects.project_id,");
    std::string project("projects.display_name as project_name, ");
    std::string startTime("task_details.start_time, ");
    std::string endTime("task_details.end_time, ");
    std::string duration("task_details.duration, ");
    std::string categoryId("categories.category_id, ");
    std::string category("categories.name as category_name, ");
    std::string description("task_details.description as description, ");
    std::string dateCreatedUtc("task_details.date_created_utc, ");
    std::string dateModifiedUtc("task_details.date_modified_utc, ");
    std::string isActive("task_details.is_active ");
    std::string from("FROM task_details ");
    std::string innerJoinTasks("INNER JOIN tasks ON task_details.task_id = tasks.task_id ");
    std::string innerJoinCategories("INNER JOIN categories ON task_details.category_id = categories.category_id ");
    std::string innerJoinProjects("INNER JOIN projects ON task_details.project_id = projects.project_id ");
    std::string where("WHERE task_detail_id = ?");

    std::string qry = select + projectId + project + startTime + endTime + duration + categoryId + category + description + dateCreatedUtc + dateModifiedUtc + isActive + from + innerJoinTasks + innerJoinCategories + innerJoinProjects + where;
    db::query query(*db_connection::get_instance().get_database(), qry);
    query.bind(1, taskDetailId);

    query.run();

    db::column column(query.get_handle());
    models::task_detail taskDetail;

    taskDetail.task_detail_id = column.get<int>(0);
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

void db_service::update_task_detail(models::task_detail taskDetail)
{
    std::string cmd("UPDATE task_details SET start_time = ?, end_time = ?, duration = ?, description = ?, date_modified_utc = ?, project_id = ?, category_id = ? WHERE task_detail_id = ?");
    db::command command(*db_connection::get_instance().get_database(), cmd);

    command.binder() << taskDetail.start_time << taskDetail.end_time << taskDetail.duration << taskDetail.description << taskDetail.date_modified_utc << taskDetail.project_id << taskDetail.category_id << taskDetail.task_detail_id;
    command.execute();
}

void db_service::delete_task_detail(const int taskDetailId)
{
    std::string cmd("UPDATE task_details SET is_active = 0 WHERE task_detail_id = ?");
    db::command command(*db_connection::get_instance().get_database(), cmd);

    command.binder() << taskDetailId;
    command.execute();
}

} // namespace app::services

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

std::vector<models::employer> db_service::get_active_employers()
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
        client.name = column.get<std::string>(1);
        client.date_created_utc = column.get<int>(2);
        client.date_updated_utc = column.get<int>(3);
        client.is_active = column.get<int>(4);
        client.employer_id = column.get<int>(5);
        clients.push_back(client);
    }

    return clients;
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
    std::string cmd("SELECT * FROM projects WHERE is_active = 1;");
    db::query query(*db_connection::get_instance().get_database(), cmd);

    std::vector<models::project> projects;
    while (query.run()) {
        db::column column(query.get_handle());
        models::project project;

        project.project_id = column.get<int>(0);
        project.name = column.get<std::string>(1);
        project.display_name = column.get<std::string>(2);
        project.date_created_utc = column.get<int>(3);
        project.date_updated_utc = column.get<int>(4);
        project.is_active = column.get<int>(5);
        project.employer_id = column.get<int>(6);
        if (column.get_type(7) == db::column_type::Null) {
            project.client_id = 0;
        } else {
            project.client_id = column.get<int>(7);
        }

        projects.push_back(project);
    }

    return projects;
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
        category.name = column.get<std::string>(1);
        category.description = column.get<std::string>(2);
        category.date_created_utc = column.get<int>(3);
        category.date_updated_utc = column.get<int>(4);
        category.is_active = column.get<int>(5);
        category.project_id = column.get<int>(6);

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
        db::command command(*db_connection::get_instance().get_database(), cmd);
        command.binder() << date << projectId;
        command.execute();
        taskId = (int) db_connection::get_instance().get_database()->get_last_rowid();
    } else {
        taskId = column.get<int>(0);
    }

    return taskId;
}

void db_service::create_new_task(const int projectId, const int taskId, const std::string& startTime, const std::string& endTime, const std::string& duration, const int categoryId, const std::string& description)
{
    std::string cmd("INSERT INTO task_details (start_time, end_time, duration, description, is_active, project_id, task_id, category_id) VALUES (?, ?, ?, ?, 1, ?, ?, ?");
    db::command command(*db_connection::get_instance().get_database(), cmd);
    command.binder() << startTime << endTime << duration << description << projectId << taskId << categoryId;
    command.execute();
}

} // namespace app::services

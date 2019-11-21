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

#include "setupdatabase.h"

namespace app::wizard
{
SetupTables::SetupTables(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
{
}

bool SetupTables::CreateTables()
{
    bool createSuccess = Create();
    bool seedSuccess = Seed();
    return createSuccess && seedSuccess;
}

bool SetupTables::Create()
{
    try {
        CreateEmployersTable();
        CreateClientsTable();
        CreateProjectsTable();
        CreateCategoriesTable();
        CreateTasksTable();
        CreateTaskItemTypesTable();
        CreateTaskItemsTable();
        return true;

    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Setup Procedure - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
}

bool SetupTables::Seed()
{
    try {
        SeedTaskItemTypesTable();
        return true;
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Setup Procedure - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
}

void SetupTables::CreateEmployersTable()
{
    const std::string query = "CREATE TABLE employers"
                              "("
                              "    employer_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL,"
                              "    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    is_active INTEGER NOT NULL"
                              ");";
    auto db = services::db_connection::get_instance().get_handle();
    db << query;
}

void SetupTables::CreateClientsTable()
{
    const std::string query = "CREATE TABLE clients"
                              "("
                              "    client_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL,"
                              "    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    is_active INTEGER NOT NULL,"
                              "    employer_id INTEGER NOT NULL,"
                              "    FOREIGN KEY (employer_id) REFERENCES employers(employer_id)"
                              ");";

    auto db = services::db_connection::get_instance().get_handle();
    db << query;
}

void SetupTables::CreateProjectsTable()
{
    const std::string query = "CREATE TABLE projects"
                              "("
                              "    project_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL UNIQUE,"
                              "    display_name TEXT NOT NULL,"
                              "    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    is_active INTEGER NOT NULL,"
                              "    employer_id INTEGER NOT NULL,"
                              "    client_id INTEGER NULL,"
                              "    FOREIGN KEY (employer_id) REFERENCES employers(employer_id),"
                              "    FOREIGN KEY (client_id) REFERENCES clients(client_id)"
                              ");";

    auto db = services::db_connection::get_instance().get_handle();
    db << query;
}

void SetupTables::CreateCategoriesTable()
{
    const std::string query = "CREATE TABLE categories"
                              "("
                              "    category_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL,"
                              "    color INTEGER NOT NULL,"
                              "    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    is_active INTEGER NOT NULL,"
                              "    project_id INTEGER NOT NULL,"
                              "    FOREIGN KEY (project_id) REFERENCES projects(project_id)"
                              ");";

    auto db = services::db_connection::get_instance().get_handle();
    db << query;
}

void SetupTables::CreateTasksTable()
{
    const std::string query = "CREATE TABLE tasks"
                              "("
                              "    task_id INTEGER PRIMARY KEY NOT NULL,"
                              "    task_date TEXT NOT NULL UNIQUE,"
                              "    date_created_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    date_modified_utc INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    is_active INTEGER NOT NULL"
                              ");";

    auto db = services::db_connection::get_instance().get_handle();
    db << query;
}

void SetupTables::CreateTaskItemTypesTable()
{
    const std::string query = "CREATE TABLE task_item_types"
                              "("
                              "    task_item_type_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL"
                              ");";

    auto db = services::db_connection::get_instance().get_handle();
    db << query;
}

void SetupTables::CreateTaskItemsTable()
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
                              "    task_item_type_id INTEGER NOT NULL,"
                              "    project_id INTEGER NOT NULL,"
                              "    task_id INTEGER NOT NULL,"
                              "    category_id INTEGER NOT NULL,"
                              "    FOREIGN KEY (task_item_type_id) REFERENCES task_item_types(task_item_type_id),"
                              "    FOREIGN KEY (project_id) REFERENCES projects(project_id),"
                              "    FOREIGN KEY (task_id) REFERENCES tasks(task_id),"
                              "    FOREIGN KEY (category_id) REFERENCES categories(category_id)"
                              ");";

    auto db = services::db_connection::get_instance().get_handle();
    db << query;
}

void SetupTables::SeedTaskItemTypesTable()
{
    const std::string query1 = "INSERT INTO task_item_types (name) VALUES ('Entry');";
    const std::string query2 = "INSERT INTO task_item_types (name) VALUES ('Timed');";

    auto db = services::db_connection::get_instance().get_handle();
    db << query1;
    db << query2;
}

SetupEntities::SetupEntities(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
{
}

bool SetupEntities::CreateEntities(std::string employerName,
    std::string clientName,
    std::string projectName,
    std::string projectDisplayName)
{
    auto db = services::db_connection::get_instance().get_handle();
    // db << "begin transaction";
    try {
        int employerId = CreateEmployer(employerName);
        int clientId = CreateClient(clientName, employerId);
        CreateProject(projectName, projectDisplayName, employerId, clientId);
        return true;
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Setup Procedure - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
}

int SetupEntities::CreateEmployer(std::string employerName)
{
    auto db = services::db_connection::get_instance().get_handle();
    db << "INSERT INTO employers (name, is_active) VALUES (?, 1)" << employerName;
    return db.last_insert_rowid();
}

int SetupEntities::CreateClient(std::string clientName, int employerId)
{
    auto db = services::db_connection::get_instance().get_handle();
    if (clientName.empty()) {
        return 0;
    } else {
        db << "INSERT INTO clients (name, is_active, employer_id) VALUES (?, 1, ?)" << clientName << employerId;
        return db.last_insert_rowid();
    }
}

void SetupEntities::CreateProject(std::string projectName, std::string projectDisplayName, int employerId, int clientId)
{
    auto db = services::db_connection::get_instance().get_handle();
    bool isAssociatedWithClient = clientId != 0;
    if (isAssociatedWithClient) {
        db << "INSERT INTO projects(name, display_name, is_active, employer_id, client_id) VALUES(? , ? , 1, ? , ? )"
           << projectName << projectDisplayName << employerId << clientId;
    } else {
        db << "INSERT INTO projects(name, display_name, is_active, employer_id, client_id) VALUES(? , ? , 1, ? , ? )"
           << projectName << projectDisplayName << employerId << nullptr;
    }
}
} // namespace app::wizard

// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2020  Szymon Welgus
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//  Contact:
//    szymonwelgus at gmail dot com

#include "setupdatabase.h"

namespace app::wizard
{
SetupTables::SetupTables(std::shared_ptr<spdlog::logger> logger, sqlite::database* database)
    : pLogger(logger)
    , pDatabase(database)
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
        CreateRateTypesTable();
        CreateCurrenciesTable();
        CreateProjectsTable();
        CreateCategoriesTable();
        CreateTasksTable();
        CreateTaskItemTypesTable();
        CreateTaskItemsTable();
        return true;

    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Create Table Procedure - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
}

bool SetupTables::Seed()
{
    try {
        SeedRateTypesTable();
        SeedCurrenciesTable();
        SeedTaskItemTypesTable();
        return true;
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Seed Table Procedure - {0:d} : {1}", e.get_code(), e.what());
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

    *pDatabase << query;
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

    *pDatabase << query;
}

void SetupTables::CreateRateTypesTable()
{
    const std::string query = "CREATE TABLE rate_types"
                              "("
                              "    rate_type_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL"
                              ");";

    *pDatabase << query;
}

void SetupTables::CreateCurrenciesTable()
{
    const std::string query = "CREATE TABLE currencies"
                              "("
                              "    currency_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL,"
                              "    code TEXT NOT NULL,"
                              "    symbol TEXT NOT NULL"
                              ");";

    *pDatabase << query;
}

void SetupTables::CreateProjectsTable()
{
    const std::string query = "CREATE TABLE projects"
                              "("
                              "    project_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL UNIQUE,"
                              "    display_name TEXT NOT NULL,"
                              "    billable INTEGER NOT NULL,"
                              "    rate REAL NULL,"
                              "    hours INTEGER NULL,"
                              "    is_default INTEGER NOT NULL DEFAULT (0),"
                              "    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    is_active INTEGER NOT NULL,"
                              "    employer_id INTEGER NOT NULL,"
                              "    client_id INTEGER NULL,"
                              "    rate_type_id INTEGER NULL,"
                              "    currency_id INTEGER NULL,"
                              "    FOREIGN KEY (employer_id) REFERENCES employers(employer_id),"
                              "    FOREIGN KEY (client_id) REFERENCES clients(client_id),"
                              "    FOREIGN KEY (rate_type_id) REFERENCES rate_types(rate_type_id),"
                              "    FOREIGN KEY (currency_id) REFERENCES currencies(currency_id)"
                              ");";

    *pDatabase << query;
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

    *pDatabase << query;
}

void SetupTables::CreateTasksTable()
{
    const std::string query = "CREATE TABLE tasks"
                              "("
                              "    task_id INTEGER PRIMARY KEY NOT NULL,"
                              "    task_date TEXT NOT NULL UNIQUE,"
                              "    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now', 'localtime')),"
                              "    is_active INTEGER NOT NULL"
                              ");";

    *pDatabase << query;
}

void SetupTables::CreateTaskItemTypesTable()
{
    const std::string query = "CREATE TABLE task_item_types"
                              "("
                              "    task_item_type_id INTEGER PRIMARY KEY NOT NULL,"
                              "    name TEXT NOT NULL"
                              ");";

    *pDatabase << query;
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
                              "    calculated_rate REAL NULL,"
                              "    date_created INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
                              "    date_modified INTEGER NOT NULL DEFAULT (strftime('%s','now')),"
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

    *pDatabase << query;
}

void SetupTables::SeedRateTypesTable()
{
    const std::string query1 = "INSERT INTO rate_types (name) VALUES ('Unknown');";
    const std::string query2 = "INSERT INTO rate_types (name) VALUES ('Hourly');";
    const std::string query3 = "INSERT INTO rate_types (name) VALUES ('Daily');";

    *pDatabase << query1;
    *pDatabase << query2;
    *pDatabase << query3;
}

void SetupTables::SeedCurrenciesTable()
{
    const std::string query1 = "INSERT INTO currencies (name, code, symbol) VALUES ('Zlotych', 'PLN', 'zł');";
    const std::string query2 = "INSERT INTO currencies (name, code, symbol) VALUES ('Rand', 'ZAR', 'R');";
    const std::string query3 = "INSERT INTO currencies (name, code, symbol) VALUES ('Dollars', 'USD', '$');";

    *pDatabase << query1;
    *pDatabase << query2;
    *pDatabase << query3;
}

void SetupTables::SeedTaskItemTypesTable()
{
    const std::string query1 = "INSERT INTO task_item_types (name) VALUES ('Entry');";
    const std::string query2 = "INSERT INTO task_item_types (name) VALUES ('Timed');";

    *pDatabase << query1;
    *pDatabase << query2;
}

SetupEntities::SetupEntities(std::shared_ptr<spdlog::logger> logger, sqlite::database* database)
    : pLogger(logger)
    , pDatabase(database)
{
}

bool SetupEntities::CreateEntities(std::string employerName,
    std::string clientName,
    std::string projectName,
    std::string projectDisplayName)
{

    try {
        int employerId = CreateEmployer(employerName);
        int clientId = CreateClient(clientName, employerId);
        CreateProject(projectName, projectDisplayName, employerId, clientId);
        return true;
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured: Database Entity Setup Procedure - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }
}

int SetupEntities::CreateEmployer(std::string employerName)
{
    *pDatabase << "INSERT INTO employers (name, is_active) VALUES (?, 1)" << employerName;
    return (int) pDatabase->last_insert_rowid();
}

int SetupEntities::CreateClient(std::string clientName, int employerId)
{

    if (clientName.empty()) {
        return 0;
    } else {
        *pDatabase << "INSERT INTO clients (name, is_active, employer_id) VALUES (?, 1, ?)" << clientName << employerId;
        return (int) pDatabase->last_insert_rowid();
    }
}

void SetupEntities::CreateProject(std::string projectName, std::string projectDisplayName, int employerId, int clientId)
{

    bool isAssociatedWithClient = clientId != 0;
    if (isAssociatedWithClient) {
        *pDatabase << "INSERT INTO projects(name, display_name, billable, is_active, employer_id, client_id) VALUES(?, ?, ?, "
              "1, ?, ?)"
           << projectName << projectDisplayName << false << employerId << clientId;
    } else {
        *pDatabase << "INSERT INTO projects(name, display_name, billable, is_active, employer_id, client_id) VALUES(?, ?, ?, "
              "1, ?, ?)"
           << projectName << projectDisplayName << false << employerId << nullptr;
    }
}
} // namespace app::wizard

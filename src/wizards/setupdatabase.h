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

#pragma once

#include <string>

#include <spdlog/spdlog.h>
#include <sqlite_modern_cpp.h>

namespace app::wizard
{
class SetupTables final
{
public:
    SetupTables(std::shared_ptr<spdlog::logger> logger, sqlite::database* database);
    ~SetupTables() = default;

    bool CreateTables();

private:
    std::shared_ptr<spdlog::logger> pLogger;
    sqlite::database* pDatabase;

    bool Create();
    bool Seed();

    void CreateEmployersTable();
    void CreateClientsTable();
    void CreateRateTypesTable();
    void CreateCurrenciesTable();
    void CreateProjectsTable();
    void CreateCategoriesTable();
    void CreateTasksTable();
    void CreateTaskItemTypesTable();
    void CreateTaskItemsTable();

    void SeedRateTypesTable();
    void SeedCurrenciesTable();
    void SeedTaskItemTypesTable();
};

class SetupEntities final
{
public:
    SetupEntities(std::shared_ptr<spdlog::logger> logger, sqlite::database* database);
    ~SetupEntities() = default;

    bool CreateEntities(std::string employerName,
        std::string clientName,
        std::string projectName,
        std::string projectDisplayName);

private:
    std::shared_ptr<spdlog::logger> pLogger;
    sqlite::database* pDatabase;

    int CreateEmployer(std::string employerName);
    int CreateClient(std::string clientName, int employerId);
    void CreateProject(std::string projectName, std::string projectDisplayName, int employerId, int clientId);
};
} // namespace app::wizard

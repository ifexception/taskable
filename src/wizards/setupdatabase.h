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

#pragma once

#include <string>

#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>

#include "../services/db_connection.h"

namespace app::wizard
{
class SetupTables final
{
public:
    SetupTables(std::shared_ptr<spdlog::logger> logger);
    ~SetupTables() = default;

    bool CreateTables();

private:
    std::shared_ptr<spdlog::logger> pLogger;

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
    SetupEntities(std::shared_ptr<spdlog::logger> logger);
    ~SetupEntities() = default;

    bool CreateEntities(std::string employerName,
        std::string clientName,
        std::string projectName,
        std::string projectDisplayName);

private:
    std::shared_ptr<spdlog::logger> pLogger;

    int CreateEmployer(std::string employerName);
    int CreateClient(std::string clientName, int employerId);
    void CreateProject(std::string projectName, std::string projectDisplayName, int employerId, int clientId);
};
} // namespace app::wizard

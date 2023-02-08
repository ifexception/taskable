// Productivity tool to help you track the time you spend on tasks
// Copyright (C) 2023  Szymon Welgus
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

#include <memory>
#include <vector>

#include "../database/connectionprovider.h"
#include "../database/sqliteconnection.h"
#include "../models/projectmodel.h"

namespace app::data
{
class ProjectData final
{
public:
    ProjectData();
    ~ProjectData();

    int64_t Create(std::unique_ptr<model::ProjectModel> project);
    std::unique_ptr<model::ProjectModel> GetById(const int projectId);
    void Update(std::unique_ptr<model::ProjectModel> project);
    void Delete(const int projectId);
    std::vector<std::unique_ptr<model::ProjectModel>> GetAll();
    void UnmarkDefaultProjects();

private:
    std::shared_ptr<db::SqliteConnection> pConnection;

    static const std::string createProject;
    static const std::string getProject;
    static const std::string updateProject;
    static const std::string deleteProject;
    static const std::string getProjects;
    static const std::string unmarkDefaultProjects;
};
} // namespace app::data

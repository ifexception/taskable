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

#include "entitycompositor.h"

namespace app::wizard
{
EntityCompositor::EntityCompositor(std::shared_ptr<spdlog::logger> logger)
    : pLogger(logger)
    , mEmployerId(-1)
    , mClientId(-1)
{
}

bool EntityCompositor::ComposeEmployerEntity(std::unique_ptr<model::EmployerModel> employer)
{
    data::EmployerData employerData;
    try {
        mEmployerId = employerData.Create(std::move(employer));
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in EmployerData::Create() - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    return true;
}

bool EntityCompositor::ComposeClientEntity(std::unique_ptr<model::ClientModel> client)
{
    client->SetEmployerId(mEmployerId);

    data::ClientData clientData;
    try {
        mClientId = clientData.Create(std::move(client));
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ClientData::Create() - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    return true;
}

bool EntityCompositor::ComposeProjectEntity(std::unique_ptr<model::ProjectModel> project)
{
    project->SetEmployerId(mEmployerId);
    if (mClientId != -1) {
        project->SetClientId(mClientId);
    }

    data::ProjectData projectData;
    try {
        projectData.Create(std::move(project));
    } catch (const sqlite::sqlite_exception& e) {
        pLogger->error("Error occured in ProjectData::Create() - {0:d} : {1}", e.get_code(), e.what());
        return false;
    }

    return true;
}
} // namespace app::wizard

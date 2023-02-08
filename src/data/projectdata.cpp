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

#include "projectdata.h"

#include <wx/string.h>

#include "../common/util.h"
#include "employerdata.h"
#include "clientdata.h"

namespace app::data
{
ProjectData::ProjectData()
{
    pConnection = db::ConnectionProvider::Get().Handle()->Acquire();
}

ProjectData::~ProjectData()
{
    db::ConnectionProvider::Get().Handle()->Release(pConnection);
}

int64_t ProjectData::Create(std::unique_ptr<model::ProjectModel> project)
{
    auto ps = *pConnection->DatabaseExecutableHandle() << ProjectData::createProject;
    ps << project->GetName().ToStdString() << project->GetDisplayName().ToStdString() << false << project->IsDefault()
       << project->GetEmployerId();

    if (project->HasClientLinked())
        ps << project->GetClientId();
    else
        ps << nullptr;

    ps << nullptr << nullptr << nullptr;

    ps.execute();

    return pConnection->DatabaseExecutableHandle()->last_insert_rowid();
}

std::unique_ptr<model::ProjectModel> ProjectData::GetById(const int projectId)
{
    std::unique_ptr<model::ProjectModel> project = nullptr;

    *pConnection->DatabaseExecutableHandle() << ProjectData::getProject << projectId >>
        [&](int projectsProjectId,
            std::string projectsName,
            std::string projectsDisplayName,
            int projectsIsDefault,
            int projectsDateCreated,
            int projectsDateModified,
            int projectsIsActive,
            int projectsEmployerId,
            std::unique_ptr<int> projectsClientId,
            int employersEmployerId,
            std::string employersName,
            int employersDateCreated,
            int employersDateModified,
            int employersIsActive,
            std::unique_ptr<int> clientsClientId,
            std::unique_ptr<std::string> clientsName,
            std::unique_ptr<int> clientsDateCreated,
            std::unique_ptr<int> clientsDateModified,
            std::unique_ptr<int> clientIsActive,
            std::unique_ptr<int> clientsEmployerId) {
            project = std::make_unique<model::ProjectModel>(projectsProjectId,
                wxString(projectsName),
                wxString(projectsDisplayName),
                projectsIsDefault,
                projectsDateCreated,
                projectsDateModified,
                projectsIsActive);

            project->SetEmployerId(projectsEmployerId);
            auto employer = std::make_unique<model::EmployerModel>(employersEmployerId,
                wxString(employersName),
                employersDateCreated,
                employersDateModified,
                employersIsActive);
            project->SetEmployer(std::move(employer));

            if (projectsClientId != nullptr) {
                project->SetClientId(*projectsClientId);
                auto client = std::make_unique<model::ClientModel>(*clientsClientId,
                    wxString(*clientsName),
                    *clientsDateCreated,
                    *clientsDateModified,
                    *clientIsActive);
                project->SetClient(std::move(client));
            }
        };

    return project;
}

void ProjectData::Update(std::unique_ptr<model::ProjectModel> project)
{
    auto ps = *pConnection->DatabaseExecutableHandle()
              << ProjectData::updateProject << project->GetName().ToStdString()
              << project->GetDisplayName().ToStdString() << false << project->IsDefault() << util::UnixTimestamp()
              << project->GetEmployerId();

    if (project->HasClientLinked())
        ps << project->GetClientId();
    else
        ps << nullptr;

    ps << nullptr << nullptr << nullptr;

    ps << project->GetProjectId();

    ps.execute();
}

void ProjectData::Delete(const int projectId)
{
    *pConnection->DatabaseExecutableHandle() << ProjectData::deleteProject << util::UnixTimestamp() << projectId;
}

std::vector<std::unique_ptr<model::ProjectModel>> ProjectData::GetAll()
{
    std::vector<std::unique_ptr<model::ProjectModel>> projects;

    *pConnection->DatabaseExecutableHandle() << ProjectData::getProjects >>
        [&](int projectsProjectId,
            std::string projectsName,
            std::string projectsDisplayName,
            int projectsIsDefault,
            int projectsDateCreated,
            int projectsDateModified,
            int projectsIsActive,
            int projectsEmployerId,
            std::unique_ptr<int> projectsClientId,
            int employersEmployerId,
            std::string employersName,
            int employersDateCreated,
            int employersDateModified,
            int employersIsActive,
            std::unique_ptr<int> clientsClientId,
            std::unique_ptr<std::string> clientsName,
            std::unique_ptr<int> clientsDateCreated,
            std::unique_ptr<int> clientsDateModified,
            std::unique_ptr<int> clientIsActive,
            std::unique_ptr<int> clientsEmployerId) {
            auto project = std::make_unique<model::ProjectModel>(projectsProjectId,
                wxString(projectsName),
                wxString(projectsDisplayName),
                projectsIsDefault,
                projectsDateCreated,
                projectsDateModified,
                projectsIsActive);

            project->SetEmployerId(projectsEmployerId);
            auto employer = std::make_unique<model::EmployerModel>(employersEmployerId,
                wxString(employersName),
                employersDateCreated,
                employersDateModified,
                employersIsActive);
            project->SetEmployer(std::move(employer));

            if (projectsClientId != nullptr) {
                project->SetClientId(*projectsClientId);
                auto client = std::make_unique<model::ClientModel>(*clientsClientId,
                    wxString(*clientsName),
                    *clientsDateCreated,
                    *clientsDateModified,
                    *clientIsActive);
                project->SetClient(std::move(client));
            }

            projects.push_back(std::move(project));
        };

    return projects;
}

void ProjectData::UnmarkDefaultProjects()
{
    *pConnection->DatabaseExecutableHandle() << ProjectData::unmarkDefaultProjects << util::UnixTimestamp();
}

const std::string ProjectData::createProject = "INSERT INTO "
                                               "projects(name, display_name, billable, is_default, is_active, "
                                               "employer_id, client_id, rate, rate_type_id, currency_id) "
                                               "VALUES(?, ?, ?, ?, 1, ?, ?, ?, ?, ?)";

const std::string ProjectData::getProject = "SELECT projects.project_id, "
                                            "projects.name AS project_name, "
                                            "projects.display_name, "
                                            "projects.is_default, "
                                            "projects.date_created, "
                                            "projects.date_modified, "
                                            "projects.is_active, "
                                            "projects.employer_id, "
                                            "projects.client_id, "
                                            "employers.employer_id, "
                                            "employers.name, "
                                            "employers.date_created, "
                                            "employers.date_modified, "
                                            "employers.is_active, "
                                            "clients.client_id, "
                                            "clients.name, "
                                            "clients.date_created, "
                                            "clients.date_modified, "
                                            "clients.is_active, "
                                            "clients.employer_id "
                                            "FROM projects "
                                            "INNER JOIN employers ON projects.employer_id = employers.employer_id "
                                            "LEFT JOIN clients ON projects.client_id = clients.client_id "
                                            "WHERE projects.project_id = ?;";

const std::string ProjectData::updateProject =
    "UPDATE projects "
    "SET name = ?, display_name = ?, billable = ?, is_default = ?, date_modified = ?, "
    "employer_id = ?, client_id = ?, rate = ?, rate_type_id = ?, currency_id = ? "
    "WHERE project_id = ?";

const std::string ProjectData::deleteProject = "UPDATE projects"
                                               "SET is_active = 0, date_modified = ?"
                                               "WHERE project_id = ?";

const std::string ProjectData::getProjects = "SELECT projects.project_id, "
                                             "projects.name AS project_name, "
                                             "projects.display_name, "
                                             "projects.is_default, "
                                             "projects.date_created, "
                                             "projects.date_modified, "
                                             "projects.is_active, "
                                             "projects.employer_id, "
                                             "projects.client_id, "
                                             "employers.employer_id, "
                                             "employers.name, "
                                             "employers.date_created, "
                                             "employers.date_modified, "
                                             "employers.is_active, "
                                             "clients.client_id, "
                                             "clients.name, "
                                             "clients.date_created, "
                                             "clients.date_modified, "
                                             "clients.is_active, "
                                             "clients.employer_id "
                                             "FROM projects "
                                             "INNER JOIN employers ON projects.employer_id = employers.employer_id "
                                             "LEFT JOIN clients ON projects.client_id = clients.client_id "
                                             "WHERE projects.is_active = 1";

const std::string ProjectData::unmarkDefaultProjects = "UPDATE projects "
                                                       "SET is_default = 0, "
                                                       "date_modified = ?";

} // namespace app::data

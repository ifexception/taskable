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

#include "project.h"

namespace app::models
{
const std::string project::createNewProject =
    "INSERT INTO projects(name, display_name, is_active, employer_id, client_id) VALUES(? , ? , 1, ? , ? )";
const std::string project::getProjects = "SELECT projects.project_id, "
                                         "projects.name AS project_name, "
                                         "projects.display_name, "
                                         "projects.date_created_utc, "
                                         "projects.date_modified_utc, "
                                         "projects.is_active, "
                                         "employers.name AS employer_name, "
                                         "clients.name AS client_name "
                                         "FROM projects "
                                         "INNER JOIN employers ON projects.employer_id = employers.employer_id "
                                         "LEFT JOIN clients ON projects.client_id = clients.client_id "
                                         "WHERE projects.is_active = 1";
const std::string project::getProjectById = "SELECT projects.project_id, "
                                            "projects.name AS project_name, "
                                            "projects.display_name, "
                                            "projects.date_created_utc, "
                                            "projects.date_modified_utc, "
                                            "projects.is_active, "
                                            "employers.employer_id, "
                                            "employers.name AS employer_name, "
                                            "clients.name AS client_name "
                                            "FROM projects "
                                            "INNER JOIN employers ON projects.employer_id = employers.employer_id "
                                            "LEFT JOIN clients ON projects.client_id = clients.client_id "
                                            "WHERE projects.project_id = ?";
const std::string project::updateProject =
    "UPDATE projects SET name = ?, display_name = ?, date_modified_utc = ?, employer_id = ?, client_id = ?";
const std::string project::deleteProject =
    "UPDATE projects SET is_active = 0, date_modified_utc = ? WHERE project_id = ?";

project::project(int projectId,
    std::string projectName,
    std::string displayName,
    int dateCreatedUtc,
    int dateModifiedUtc,
    int isActive,
    std::string employerName,
    std::string clientName)
    : project_id(projectId)
    , project_name(projectName)
    , display_name(displayName)
    , date_created_utc(dateCreatedUtc)
    , date_modified_utc(dateModifiedUtc)
    , is_active(isActive)
    , employer_name(employerName)
    , client_name(clientName)
{
}
project::project(int projectId,
    std::string projectName,
    std::string displayName,
    int dateCreatedUtc,
    int dateModifiedUtc,
    int isActive,
    int employerId,
    std::string employerName,
    std::string clientName)
    : project_id(projectId)
    , project_name(projectName)
    , display_name(displayName)
    , date_created_utc(dateCreatedUtc)
    , date_modified_utc(dateModifiedUtc)
    , is_active(isActive)
    , employer_id(employerId)
    , employer_name(employerName)
    , client_name(clientName)
{
}
} // namespace app::models

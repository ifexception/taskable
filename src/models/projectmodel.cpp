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

#include "projectmodel.h"

#include "../services/db_connection.h"

namespace app::model
{
ProjectModel::ProjectModel()
    : mProjectId(0)
    , mName(wxGetEmptyString())
    , mDisplayName(wxGetEmptyString())
    , mDateCreated(wxDefaultDateTime)
    , mDateUpdated(wxDefaultDateTime)
    , bIsActive(false)
{
}

ProjectModel::ProjectModel(int projectId, wxString name, wxString displayName)
    : ProjectModel()
{
    mProjectId = projectId;
    mName = name;
    mDisplayName = displayName;
}

void ProjectModel::Reset()
{
    mProjectId = 0;
    mName = wxGetEmptyString();
    mDisplayName = wxGetEmptyString();
    mDateCreated = wxDefaultDateTime;
    mDateUpdated = wxDefaultDateTime;
    bIsActive = false;
}

const int ProjectModel::GetProjectId() const
{
    return mProjectId;
}

const wxString ProjectModel::GetName() const
{
    return mName;
}

const wxString ProjectModel::GetDisplayName() const
{
    return mDisplayName;
}

const wxDateTime ProjectModel::GetDateCreated()
{
    return mDateCreated;
}

const wxDateTime ProjectModel::GetDateUpdated()
{
    return mDateUpdated;
}

const bool ProjectModel::IsActive() const
{
    return bIsActive;
}

void ProjectModel::SetProjectId(const int projectId)
{
    mProjectId = projectId;
}

void ProjectModel::SetName(const wxString& name)
{
    mName = name;
}

void ProjectModel::SetDisplayName(const wxString& displayName)
{
    mDisplayName = displayName;
}

void ProjectModel::SetDateCreated(const wxDateTime& dateCreated)
{
    mDateCreated = dateCreated;
}

void ProjectModel::SetDateUpdated(const wxDateTime& dateUpdated)
{
    mDateUpdated = dateUpdated;
}

void ProjectModel::IsActive(const bool isActive)
{
    bIsActive = isActive;
}

const std::string ProjectModel::getProjects = "SELECT projects.project_id, "
                                              "projects.name, "
                                              "projects.display_name "
                                              "FROM projects "
                                              "WHERE projects.is_active = 1";

std::vector<model::ProjectModel> ProjectModel::GetAllProjects()
{
    std::vector<model::ProjectModel> projects;

    auto db = services::db_connection::get_instance().get_handle();
    db << ProjectModel::getProjects >> [&](int projectId, std::string name, std::string displayName) {
        model::ProjectModel project(projectId, wxString(name), wxString(displayName));
        projects.push_back(project);
    };

    return projects;
}
} // namespace app::model
